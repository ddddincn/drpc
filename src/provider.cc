#include "drpc/provider.h"
// 替换序列化后字符串中的'\n'为"\\n" 便于日志记录
std::string argsLog(const std::string &str) {
  std::string result;
  for (char c : str) {
    if (c == '\n') {
      result += "\\n";
    } else {
      result += c;
    }
  }
  return result;
}

drpc::Provider::Provider() {
  // 获取logger_;
  logger_ = drpc::Application::getInstance().getLogger();

  // server初始化
  //  获取配置信息
  std ::string ip = drpc::Application::getInstance().getConfig()["server_ip"];
  uint16_t port =
      atoi(drpc::Application::getInstance().getConfig()["server_port"].c_str());
  muduo::net::InetAddress address(ip, port);
  event_loop_ = std::make_shared<muduo::net::EventLoop>();
  // 创建TcpServer对象
  server_ = std::make_shared<muduo::net::TcpServer>(event_loop_.get(), address,
                                                    "RpcProvider");
  // 关闭muduo日志
  muduo::Logger::setLogLevel(muduo::Logger::NUM_LOG_LEVELS);
  // 绑定连接回调函数和读写事件函数
  server_->setConnectionCallback(
      std::bind(&drpc::Provider::onConnection_, this, std::placeholders::_1));
  server_->setMessageCallback(
      std::bind(&drpc::Provider::onMessage_, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
  // 设置线程池大小
  std::string thread_num_str =
      drpc::Application::getInstance().getConfig()["server_thread_num"];
  // 默认线程数为4
  int thread_num = 4;
  if (thread_num_str != "") thread_num = atoi(thread_num_str.c_str());
  server_->setThreadNum(thread_num);
}

void drpc::Provider::notifyService(google::protobuf::Service *service) {
  // 用于存储服务对象的信息
  struct ServiceInfo service_info;
  // 获取服务对象描述
  const google::protobuf::ServiceDescriptor *pServiceDesc =
      service->GetDescriptor();
  // 获取服务对象名称
  std::string service_name = pServiceDesc->name();
  // 获取服务对象中方法数量,通过这个数量来遍历获取所有方法描述
  int method_count = pServiceDesc->method_count();
  // 遍历获取所有方法描述
  for (int i = 0; i < method_count; ++i) {
    const google::protobuf::MethodDescriptor *pMethodDesc =
        pServiceDesc->method(i);
    std::string method_name = pMethodDesc->name();
    service_info.method_map_.insert({method_name, pMethodDesc});
  }
  service_info.service_ = service;
  service_map_.insert({service_name, service_info});
}

void drpc::Provider::run() {
  std::string ip = drpc::Application::getInstance().getConfig()["server_ip"];
  uint16_t port =
      atoi(drpc::Application::getInstance().getConfig()["server_port"].c_str());
  // 向zkserver注册节点
  drpc::ZKClient zkclient;
  // 初始化zk连接
  zkclient.init();
  // 向zkserver注册服务
  for (const auto &[service_name, service_info] : service_map_) {
    // service作为根节点，method作为子节点
    std::string service_path = "/" + service_name;
    // 创建父节点
    zkclient.create(service_path.c_str(), nullptr, 0);
    for (const auto &[method_name, method_desc] : service_info.method_map_) {
      // 拼接方法路径
      std::string method_path = service_path + "/" + method_name;
      // 将该方法所在的服务器的ip端口作为data存入zk节点
      std::string host = ip + ":" + std::to_string(port);
      // 在zkserver上创建临时节点
      zkclient.create(method_path.c_str(), host.data(), host.length(),
                      ZOO_EPHEMERAL);
    }
  }
  // 启动服务
  logger_->info("server start service at ip: {} port:{}", ip, port);
  server_->start();
  event_loop_->loop();
}

void drpc::Provider::onConnection_(const muduo::net::TcpConnectionPtr &conn) {
  if (!conn->connected()) {
    conn->shutdown();  // 断开连接
  }
}

void drpc::Provider::onMessage_(const muduo::net::TcpConnectionPtr &conn,
                                muduo::net::Buffer *buffer,
                                muduo::Timestamp time_stamp) {
  // 取出缓冲区中所有数据
  std::string recv_buf = buffer->retrieveAllAsString();
  // 从前四个字节的内容
  uint32_t header_size = 0;
  // 获取长度
  recv_buf.copy(reinterpret_cast<char *>(&header_size), 4, 0);
  // 根据长度获取rpc_header
  std::string header_str = recv_buf.substr(4, header_size);

  // 反序列化
  drpc::Header header;
  std::string service_name;
  std::string method_name;
  uint32_t args_size;
  if (header.ParseFromString(header_str)) {
    service_name = header.service_name();
    method_name = header.method_name();
    args_size = header.args_size();
  } else {
    logger_->error("header parse error! header: {}", header_str);
    return;
  }
  std::string args_str = recv_buf.substr(4 + header_size, args_size);

  // 获取客户端ip并记录访问日志
  const muduo::net::InetAddress &addr = conn->peerAddress();
  std::string clientip = addr.toIp();

  logger_->info("{} request {}.{}, args:{}", clientip, service_name,
                method_name, argsLog(args_str));

  // 获取service对象和method对象
  // 获取service对象
  if (service_map_.count(service_name) == 0) {
    logger_->error("{} is not exist", service_name);
    return;
  }
  google::protobuf::Service *service = service_map_[service_name].service_;

  // 获取method对象
  if (service_map_[service_name].method_map_.count(method_name) == 0) {
    logger_->error("{}.{} is not exist", service_name, method_name);
    return;
  }
  const google::protobuf::MethodDescriptor *method =
      service_map_[service_name].method_map_[method_name];

  // 生成rpc调用方法的参数
  google::protobuf::Message *request =
      service->GetRequestPrototype(method).New();
  // 解析参数
  if (!request->ParseFromString(args_str)) {
    logger_->error("args parse error! args: {}", args_str);
    return;
  }
  google::protobuf::Message *response =
      service->GetResponsePrototype(method).New();

  google::protobuf::Closure *done =
      google::protobuf::NewCallback<drpc::Provider,
                                    const muduo::net::TcpConnectionPtr &,
                                    google::protobuf::Message *>(
          this, &drpc::Provider::sendResponse_, conn, response);

  // 根据远端rpc请求，调整当前节点上发布的方法
  service->CallMethod(method, nullptr, request, response, done);
}

void drpc::Provider::sendResponse_(const muduo::net::TcpConnectionPtr &conn,
                                   google::protobuf::Message *response) {
  std::string response_str;
  if (!response->SerializePartialToString(&response_str)) {
    logger_->error("response serialize error! response: {}", response_str);
  } else {
    // 发送响应
    conn->send(response_str);
  }
  conn->shutdown();
}
