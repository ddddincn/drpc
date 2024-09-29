#include "drpc/controller.h"

drpc::Controller::Controller() {
  failed_ = false;
  err_text_ = "";
}

void drpc::Controller::Reset() {
  failed_ = false;
  err_text_ = "";
}

bool drpc::Controller::Failed() const { return failed_; }

std::string drpc::Controller::ErrorText() const { return err_text_; }

void drpc::Controller::SetFailed(const std::string& reason) {
  failed_ = true;
  err_text_ = reason;
}

// 未实现

void drpc::Controller::StartCancel() {}

bool drpc::Controller::IsCanceled() const { return false; }

void drpc::Controller::NotifyOnCancel(google::protobuf::Closure* callback) {}
