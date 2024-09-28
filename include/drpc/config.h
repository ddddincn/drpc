#ifndef CONFIG_H
#define CONFIG_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

namespace drpc {
class Config {
 public:
  Config() = default;
  ~Config() = default;
  // 传入配置文件地址
  void parse(const std::string& configfile);
  // 获取参数值
  std::string get(const std::string& key);
  // 重载[]
  std::string operator[](const std::string& key);

 private:
  // 用于存储解析后的参数
  std::unordered_map<std::string, std::string> parameters_;
};
}  // namespace drpc

#endif  // CONFIG_H