#ifndef PROVIDER_H
#define PROVIDER_H

#include <google/protobuf/descriptor.h>
#include <google/protobuf/service.h>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>
#include <spdlog/async.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <functional>
#include <string>
#include <unordered_map>

#include "drpc/application.h"
#include "drpc/zkclient.h"
#include "header.pb.h"

namespace drpc {
class Provider {
 public:
  Provider();
  // 提供给外部使用，发布rpc方法的函数调用
  void notifyService(google::protobuf::Service* service);
  // 启动rpc节点，开始提供rpc远程服务调用
  void run();

 private:
  // 日志记录器 异步控制台文件同时记录
  std::shared_ptr<muduo::net::TcpServer> server_;
  std::shared_ptr<muduo::net::EventLoop> event_loop_;
  std::shared_ptr<spdlog::logger> logger_;

  // 服务类型信息
  struct ServiceInfo {
    google::protobuf::Service* service_;  // 服务对象
    std::unordered_map<std::string, const google::protobuf::MethodDescriptor*>
        method_map_;  // 服务名称和服务方法的映射
  };
  // 存储注册成功的服务对象和对象方法的信息
  std::unordered_map<std::string, struct ServiceInfo> service_map_;
  void onConnection_(const muduo::net::TcpConnectionPtr&);
  void onMessage_(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*,
                  muduo::Timestamp);

  // Closure的回调操作，用于序列化rpc的响应和发送
  void sendResponse_(const muduo::net::TcpConnectionPtr&,
                     google::protobuf::Message*);
};
}  // namespace drpc

#endif
