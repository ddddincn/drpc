#ifndef ZKCLIENT_H
#define ZKCLIENT_H

#include <semaphore.h>
#include <zookeeper/zookeeper.h>

#include <mutex>
#include <string>

#include "drpc/application.h"

namespace drpc {
class ZKClient {
 public:
  ZKClient();
  ~ZKClient();
  // 连接zkserver
  void init();
  // zkserver上是否存在该节点
  bool exist(const char* path);
  // 在zkserver上根据path创建节点
  void create(const char* path, const char* data, int datalen, int flags = 0);
  std::string getData(const char* path);

 private:
  zhandle_t* zhandle_;  // zk客户端句柄
  std::shared_ptr<spdlog::logger> logger_;
};
}  // namespace drpc

#endif  // ZKCLIENT_H