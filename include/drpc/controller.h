#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <google/protobuf/service.h>

#include <string>

namespace drpc {
class Controller : public google::protobuf::RpcController {
 public:
  Controller();
  void Reset() override;
  bool Failed() const override;
  std::string ErrorText() const override;
  void SetFailed(const std::string& reason) override;

  // 未实现
  void StartCancel() override;
  bool IsCanceled() const override;
  void NotifyOnCancel(google::protobuf::Closure* callback) override;

 private:
  bool failed_;
  std::string err_text_;
};
}  // namespace drpc
#endif  // CONTROLLER_H