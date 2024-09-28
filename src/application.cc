#include "drpc/application.h"

void printHelp() {
  std::cout << "Usage: program [options]\n";
  std::cout << "Options:\n";
  std::cout << "  -h                      show help menu\n";
  std::cout << "  -f  <configfile>        configuration file" << std::endl;
}

drpc::Config drpc::Application::config_;
std::shared_ptr<spdlog::logger> drpc::Application::logger_ = nullptr;
void drpc::Application::init(int argc, char* const argv[]) {
  // 参数不足
  if (argc < 2) {
    printHelp();  // 打印参数的配置说明
    exit(EXIT_FAILURE);
  }
  int opt = 0;
  std::string config_file;
  while ((opt = getopt(argc, argv, "hf:")) != -1) {
    switch (opt) {
      case 'h':
        printHelp();
        exit(EXIT_SUCCESS);
      case 'f':
        config_file = optarg;  // 获取 -f 后的参数
        break;
      default:
        printHelp();
        exit(EXIT_FAILURE);
    }
  }
  config_.parse(config_file);

  // 日志初始化
  spdlog::init_thread_pool(8192, 1);
  std::string log_file =
      drpc::Application::getInstance().getConfig()["log_file"];
  if (log_file == "") log_file = "./logs/.log";
  try {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto daily_sink =
        std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_file, 0, 0);
    std::vector<spdlog::sink_ptr> sinks{console_sink, daily_sink};
    logger_ = std::make_shared<spdlog::async_logger>(
        "rpcServer", sinks.begin(), sinks.end(), spdlog::thread_pool(),
        spdlog::async_overflow_policy::block);
    logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [thread %t] [%^%l%$] %v");
  } catch (const spdlog::spdlog_ex& ex) {
    std::cout << "Log init failed: " << ex.what() << std::endl;
  }
}

drpc::Application& drpc::Application::getInstance() {
  static Application app;
  return app;
}

drpc::Config& drpc::Application::getConfig() { return config_; }

void drpc::Application::setLogPattern(std::string pattern,
                                      spdlog::pattern_time_type time_type) {
  logger_->set_pattern(pattern, time_type);
}
void drpc::Application::setLogLevel(spdlog::level::level_enum log_level) {
  logger_->set_level(log_level);
}

std::shared_ptr<spdlog::logger> drpc::Application::getLogger() {
  return logger_;
}
