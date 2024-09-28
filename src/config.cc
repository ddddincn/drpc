#include "drpc/config.h"

// 辅助函数，用于去除字符串首尾空格
std::string trim(const std::string& s) {
  auto start = s.begin();
  while (start != s.end() && std::isspace(*start)) {
    start++;
  }
  auto end = s.end();
  do {
    end--;
  } while (std::distance(start, end) > 0 && std::isspace(*end));
  return std::string(start, end + 1);
}

void drpc::Config::parse(const std::string& configfile) {
  // 如果该文件不存在或者是目录
  if (!std::filesystem::exists(configfile) ||
      std::filesystem::is_directory(configfile)) {
    std::cout << configfile << " is not exist or is a directory" << std::endl;
    exit(EXIT_FAILURE);
  }
  // 只读方式打开文件
  std::ifstream file(configfile);
  // 判断是否能打开文件
  if (!file.is_open()) {
    std::cout << "can not open " << configfile << std::endl;
    exit(EXIT_FAILURE);
  }
  // 读取文件
  std::string line;
  int line_count = 0;  // 行计数
  while (std::getline(file, line)) {
    line_count++;
    // 去除首位空格
    trim(line);
    // 如果首字符为#视为注释
    if (line[0] == '#' || line == "") continue;
    // 处理参数
    int pos = line.find('=');
    if (pos == std::string::npos) {
      std::cout << "\"" << line << "\" is not be allowed in \"" << configfile
                << "\" at " << line_count << " line" << std::endl;
      exit(EXIT_FAILURE);
    }
    // 插入提取到的参数
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);
    parameters_[key] = value;
  }
}

std::string drpc::Config::get(const std::string& key) {
  return parameters_[key];
}

std::string drpc::Config::operator[](const std::string& str) {
  return parameters_[str];
}