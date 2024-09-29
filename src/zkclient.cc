#include "drpc/zkclient.h"

#include <zookeeper/zookeeper.h>

drpc::ZKClient::ZKClient() : zhandle_(nullptr) {
  // 日志初始化
  logger_ = drpc::Application::getInstance().getLogger();
}

drpc::ZKClient::~ZKClient() {
  if (zhandle_ != nullptr) {
    zookeeper_close(zhandle_);  // 关闭连接
  }
}

void watcher(zhandle_t* zh, int type, int state, const char* path,
             void* watcherCtx) {
  if (type == ZOO_SESSION_EVENT)  // 回调的消息类型是和会话相关的消息类型
  {
    if (state == ZOO_CONNECTED_STATE)  // zkclient和zkserver链接成功
    {
      sem_t* sem = (sem_t*)zoo_get_context(zh);
      sem_post(sem);
    }
  }
}

void drpc::ZKClient::init() {
  zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);
  // 获取ip和端口
  std::string ip = drpc::Application::getInstance().getConfig()["zookeeper_ip"];
  std::string port =
      drpc::Application::getInstance().getConfig()["zookeeper_port"];
  std::string host = ip + ":" + port;

  // 初始化zookeeper连接
  // zookeeper_init() 是一个异步调用
  zhandle_ = zookeeper_init(host.c_str(), watcher, 30000, nullptr, nullptr, 0);
  // 如果zhandle_为nullptr说明zookeeper没有成功调用IO发送链接
  if (zhandle_ == nullptr) {
    // logger_->error("zookeeper init error");
    std::cout << "zookeeper init error" << std::endl;
    exit(EXIT_FAILURE);
  }

  // 等待完成init完成
  sem_t sem;
  sem_init(&sem, 0, 0);
  zoo_set_context(zhandle_, &sem);
  sem_wait(&sem);
}

bool drpc::ZKClient::exist(const char* path) {
  // 检查该节点是否存在
  if (zoo_exists(zhandle_, path, 0, nullptr) == ZNONODE) return false;
  return true;
}

void drpc::ZKClient::create(const char* path, const char* data, int datalen,
                            int flags) {
  // 如果不存在就创建新的节点
  if (!exist(path)) {
    char buff[128];
    int create = zoo_create(zhandle_, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE,
                            flags, buff, sizeof(buff));
    if (create == ZOK) {
      logger_->info("zookeeper node create success, path: {}", path);
    } else {
      logger_->error("zookeeper node create error, path: {}, errno: {}", path,
                     create);
      exit(EXIT_FAILURE);
    }
  }
}

std::string drpc::ZKClient::getData(const char* path) {
  char buff[64];
  int buff_len = sizeof(buffer);
  int flag = zoo_get(zhandle_, path, 0, buff, &buff_len,
                     nullptr);  // 获取信息与返回值
  if (flag == ZOK)              // 如果获取失败
    return buff;
  return "";
}
