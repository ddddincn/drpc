#ifndef CHANNEL_H
#define CHANNEL_H

#include <arpa/inet.h>
#include <errno.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

#include "drpc/application.h"
#include "drpc/zkclient.h"
#include "header.pb.h"

namespace drpc {
class Channel : public google::protobuf::RpcChannel {
 public:
  void CallMethod(const google::protobuf::MethodDescriptor* method,
                  google::protobuf::RpcController* controller,
                  const google::protobuf::Message* request,
                  google::protobuf::Message* response,
                  google::protobuf::Closure* done) override;
};
}  // namespace drpc
#endif  // CHANNEL_H