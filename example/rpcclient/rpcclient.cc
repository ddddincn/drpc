#include <iostream>

#include "../user.pb.h"
#include "drpc/application.h"
#include "drpc/channel.h"

int main(int argc, char *const argv[]) {
  // 初始化
  drpc::Application::init(argc, argv);

  // 生成stub，远程调用必须操作
  example::UserServiceRpc_Stub stub(new drpc::Channel());

  // =======================================================================
  std::cout << "调用UserService.Login() example:" << std::endl;
  std::cout << "UserService.Login()成功样例:" << std::endl;
  // 设置请求
  example::LoginRequest login_request;
  login_request.set_name("dddd");
  login_request.set_passwd("123");
  example::LoginResponse login_response;
  // 开始调用
  stub.Login(nullptr, &login_request, &login_response, nullptr);
  // 查看响应
  std::cout << "\terrcode: " << login_response.result().errcode() << std::endl;
  std::cout << "\terrmsg : " << login_response.result().errmsg() << "\n"
            << std::endl;

  // =======================================================================
  std::cout << "UserService.Login()失败样例:" << std::endl;
  // 设置请求
  login_request.set_name("aaaa");
  login_request.set_passwd("456");
  // 开始调用
  stub.Login(nullptr, &login_request, &login_response, nullptr);
  // 查看响应
  std::cout << "\terrcode: " << login_response.result().errcode() << std::endl;
  std::cout << "\terrmsg : " << login_response.result().errmsg() << "\n"
            << std::endl;

  // =======================================================================
  std::cout << "调用UserService.Register() example:" << std::endl;
  std::cout << "UserService.Register()成功样例:" << std::endl;
  // 设置请求
  example::RegisterRequest register_request;
  register_request.set_name("aaaa");
  register_request.set_passwd("456");
  example::RegisterResponse register_response;
  // 开始调用
  stub.Register(nullptr, &register_request, &register_response, nullptr);
  // 查看响应
  std::cout << "\terrcode: " << register_response.result().errcode()
            << std::endl;
  std::cout << "\terrmsg : " << register_response.result().errmsg() << "\n"
            << std::endl;

  // =======================================================================
  std::cout << "调用UserService.Register() example:" << std::endl;
  std::cout << "UserService.Register()失败样例:" << std::endl;
  // 设置请求
  register_request.set_name("dddd");
  register_request.set_passwd("123");
  // 开始调用
  stub.Register(nullptr, &register_request, &register_response, nullptr);
  // 查看响应
  std::cout << "\terrcode: " << register_response.result().errcode()
            << std::endl;
  std::cout << "\terrmsg : " << register_response.result().errmsg() << "\n"
            << std::endl;
}
