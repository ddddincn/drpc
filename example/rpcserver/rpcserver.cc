#include <signal.h>

#include "../user.pb.h"
#include "drpc/application.h"
#include "drpc/provider.h"

bool doLogin(const std::string& name, const std::string& passwd) {
  if (name == "dddd" && passwd == "123") return true;
  return false;
}

bool doRegister(const int id, const std::string& name,
                const std::string& passwd) {
  if (name == "dddd") return false;
  return true;
}

class UserService : public example::UserServiceRpc {
  // 重写Login()函数实现业务
  void Login(::google::protobuf::RpcController* controller,
             const ::example::LoginRequest* request,
             ::example::LoginResponse* response,
             ::google::protobuf::Closure* done) override {
    // ====================获取参数====================
    std::string name = request->name();
    std::string passwd = request->passwd();

    // ====================模拟业务====================
    bool flag = doLogin(name, passwd);

    // ====================完成响应====================
    if (flag == true) {  // 如果login成功的响应
      example::ResultCode* code = response->mutable_result();
      code->set_errcode(200);
      code->set_errmsg("login success");
      response->set_success(true);
    } else {  // 如果login失败的响应
      example::ResultCode* code = response->mutable_result();
      code->set_errcode(400);
      code->set_errmsg("login error");
      response->set_success(false);
    }

    // ====================执行回调====================
    done->Run();
  }

  virtual void Register(::google::protobuf::RpcController* controller,
                        const ::example::RegisterRequest* request,
                        ::example::RegisterResponse* response,
                        ::google::protobuf::Closure* done) override {
    // ====================获取参数====================
    int id = request->id();
    std::string name = request->name();
    std::string passwd = request->passwd();

    // ====================模拟业务====================
    bool flag = doRegister(id, name, passwd);

    // ====================完成响应====================
    if (flag == true) {  // 如果login成功的响应
      example::ResultCode* code = response->mutable_result();
      code->set_errcode(200);
      code->set_errmsg("register success");
      response->set_success(true);
    } else {  // 如果login失败的响应
      example::ResultCode* code = response->mutable_result();
      code->set_errcode(400);
      code->set_errmsg("register error");
      response->set_success(false);
    }

    // ====================执行回调====================
    done->Run();
  }
};

int main(int argc, char* const argv[]) {
  // 初始化框架程序
  drpc::Application::init(argc, argv);
  // 关闭控制台日志输出
  drpc::Application::getInstance().closeConsoleLog();
  // 服务提供者
  drpc::Provider provider;
  // 让provider将UserService和本机ip:port发布到zookeeper，客户端可以从zookeeper获取方法的访问方法
  provider.notifyService(new UserService());
  // provider提供服务
  provider.run();
  return 0;
}
