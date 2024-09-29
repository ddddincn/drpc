#include <iostream>

#include "../user.pb.h"
#include "drpc/application.h"
#include "drpc/channel.h"
#include "drpc/controller.h"

int main(int argc, char *const argv[]) {
  // 初始化
  drpc::Application::init(argc, argv);

  // 生成stub，远程调用必须操作
  example::UserServiceRpc_Stub stub(new drpc::Channel());

  // =======================================================================
  std::cout << "调用UserService.Login() example:" << std::endl;
  std::cout << "UserService.Login()成功样例:" << std::endl;
  // 设置请求
  example::LoginRequest login_success_request;
  login_success_request.set_name("dddd");
  login_success_request.set_passwd("123");
  // 开始调用
  drpc::Controller login_success_controller;
  example::LoginResponse login_success_response;
  stub.Login(&login_success_controller, &login_success_request,
             &login_success_response, nullptr);
  // 查看调用
  if (login_success_controller.Failed()) {
    std::cout << "\tLogin error" << std::endl;
    std::cout << "\t errmsg : " << login_success_controller.ErrorText()
              << std::endl;
  } else {
    // 查看响应
    std::cout << "\terrcode: " << login_success_response.result().errcode()
              << std::endl;
    std::cout << "\terrmsg : " << login_success_response.result().errmsg()
              << "\n"
              << std::endl;
  }

  // =======================================================================
  std::cout << "UserService.Login()失败样例:" << std::endl;
  // 设置请求
  example::LoginRequest login_error_request;
  login_error_request.set_name("aaaa");
  login_error_request.set_passwd("456");
  // 开始调用
  drpc::Controller login_error_controller;
  example::LoginResponse login_error_response;
  stub.Login(&login_error_controller, &login_error_request,
             &login_error_response, nullptr);
  if (login_error_controller.Failed()) {
    std::cout << "\tLogin error" << std::endl;
    std::cout << "\t errmsg : " << login_error_controller.ErrorText()
              << std::endl;
  } else {
    // 查看响应
    std::cout << "\terrcode: " << login_error_response.result().errcode()
              << std::endl;
    std::cout << "\terrmsg : " << login_error_response.result().errmsg() << "\n"
              << std::endl;
  }

  // =======================================================================
  std::cout << "调用UserService.Register() example:" << std::endl;
  std::cout << "UserService.Register()成功样例:" << std::endl;
  // 设置请求
  example::RegisterRequest register_success_request;
  register_success_request.set_name("aaaa");
  register_success_request.set_passwd("456");
  // 开始调用
  drpc::Controller register_success_controller;
  example::RegisterResponse register_success_response;
  stub.Register(&register_success_controller, &register_success_request,
                &register_success_response, nullptr);
  // 查看响应
  if (register_success_controller.Failed()) {
    std::cout << "\tLogin error" << std::endl;
    std::cout << "\t errmsg : " << register_success_controller.ErrorText()
              << std::endl;
  } else {
    std::cout << "\terrcode: " << register_success_response.result().errcode()
              << std::endl;
    std::cout << "\terrmsg : " << register_success_response.result().errmsg()
              << "\n"
              << std::endl;
  }

  // =======================================================================
  std::cout << "UserService.Register()失败样例:" << std::endl;
  // 设置请求
  example::RegisterRequest register_error_request;
  register_error_request.set_name("dddd");
  register_error_request.set_passwd("123");
  // 开始调用
  drpc::Controller register_error_controller;
  example::RegisterResponse register_error_response;
  stub.Register(nullptr, &register_error_request, &register_error_response,
                nullptr);
  // 查看响应
  if (register_error_controller.Failed()) {
    std::cout << "\tLogin error" << std::endl;
    std::cout << "\t errmsg : " << register_error_controller.ErrorText()
              << std::endl;
  } else {
    std::cout << "\terrcode: " << register_error_response.result().errcode()
              << std::endl;
    std::cout << "\terrmsg : " << register_error_response.result().errmsg()
              << "\n"
              << std::endl;
  }
}
