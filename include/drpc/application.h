#ifndef APPLICATION_H
#define APPLICATION_H

#include <getopt.h>
#include <spdlog/async.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <iostream>

#include "drpc/config.h"

namespace drpc {
class Application {
 public:
  static void init(int argc, char* const argv[]);
  static Application& getInstance();
  Config& getConfig();
  // 设置日志格式
  void setLogPattern(std::string pattern, spdlog::pattern_time_type time_type =
                                              spdlog::pattern_time_type::local);
  // 设置日志最低等级
  void setLogLevel(spdlog::level::level_enum log_level);
  std::shared_ptr<spdlog::logger> getLogger();

 private:
  Application() = default;
  Application(const Application&) = delete;
  Application(const Application&&) = delete;
  Application operator=(const Application&) = delete;
  ~Application() = default;
  static Config config_;
  static std::shared_ptr<spdlog::logger> logger_;
};
}  // namespace drpc

#endif  // APPLICATION_H