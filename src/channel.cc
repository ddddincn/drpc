#include "drpc/channel.h"

void drpc::Channel::CallMethod(const google::protobuf::MethodDescriptor* method,
                               google::protobuf::RpcController* controller,
                               const google::protobuf::Message* request,
                               google::protobuf::Message* response,
                               google::protobuf::Closure* done) {
  const google::protobuf::ServiceDescriptor* service_desc = method->service();
  std::string service_name = service_desc->name();
  std::string method_name = method->name();

  // 获取序列化后的参数长度args_size
  uint32_t args_size = 0;
  std::string args_str;
  if (!request->SerializeToString(&args_str)) {
    controller->SetFailed("serialize error");
    return;
  } else {
    args_size = args_str.size();
  }
  // 定义header
  drpc::Header header;
  header.set_service_name(service_name);
  header.set_method_name(method_name);
  header.set_args_size(args_size);

  uint32_t header_size = 0;
  std::string header_str;
  if (!header.SerializePartialToString(&header_str)) {
    controller->SetFailed("serialize error");
    return;
  } else {
    header_size = header_str.size();
  }

  // 组织待发送的header
  std::string send_str;
  send_str.insert(0, reinterpret_cast<char*>(&header_size), 4);
  send_str += header_str;
  send_str += args_str;
  // 创建socket，准备连接服务器
  int client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd == -1) {
    controller->SetFailed("creat socket error, errno:" +
                          std::string(strerror(errno)));
    return;
  }
  // 从zkserver获取提供service的服务器ip端口
  ZKClient zkclient;
  // 初始化连接
  zkclient.init();
  // 拼接路径
  std::string method_path = "/" + service_name + "/" + method_name;
  // 获取host
  std::string host = zkclient.getData(method_path.c_str());
  // 从host中解析出ip和port
  if (host == "") {
    controller->SetFailed(method_path + " is not exist!");
    close(client_fd);
    return;
  }
  int pos = host.find(":");
  if (pos == std::string::npos) {
    controller->SetFailed(method_path + " address is invalid!");
    close(client_fd);
    return;
  }
  std::string ip = host.substr(0, pos);
  uint16_t port = atoi(host.substr(pos + 1).c_str());

  // 初始化服务端sockaddr_in
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

  // 连接到server
  if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
      -1) {
    controller->SetFailed("connect error, errno:" +
                          std::string(strerror(errno)));
    close(client_fd);
    return;
  }
  // 发送rpc请求
  if (send(client_fd, send_str.c_str(), send_str.size(), 0) == 1) {
    controller->SetFailed("send error, errno:" + std::string(strerror(errno)));
    close(client_fd);
    return;
  }

  // 接收rpc响应
  char recv_buffer[1024];
  int recv_size = recv(client_fd, &recv_buffer, sizeof(recv_buffer), 0);
  if (recv_size <= 0) {
    controller->SetFailed("recv error, errno:" + std::string(strerror(errno)));
    close(client_fd);
    return;
  }

  // 数据反序列化
  if (!response->ParseFromArray(recv_buffer, recv_size)) {
    controller->SetFailed("parse error, response_str:" +
                          std::string(recv_buffer));
    close(client_fd);
    return;
  }

  close(client_fd);
}