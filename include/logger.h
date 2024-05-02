#include <iostream>
#include <mutex>

#include "utils.h"

class Logger {
private:
  static std::mutex mtx;

public:
  enum class Level { INFO, ERROR, DEBUG };

  template <typename... Args>
  static void log(std::ostream &os, Level level, Args &&...args);

  template <typename... Args> static void info(Args &&...args);

  template <typename... Args> static void error(Args &&...args);

  template <typename... Args> static void debug(Args &&...args);
};

template <typename... Args>
void Logger::log(std::ostream &os, Level level, Args &&...args) {
  opWithMutex(
      mtx,
      [&os, level](Args &&...args) {
        switch (level) {
        case Level::INFO:
          os << "[INFO] ";
          break;
        case Level::ERROR:
          os << "[ERROR] ";
          break;
        case Level::DEBUG:
          os << "[DEBUG] ";
          break;
        }
        (os << ... << args) << std::endl;
      },
      std::forward<Args>(args)...);
}

template <typename... Args> void Logger::info(Args &&...args) {
  log(std::cout, Level::INFO, std::forward<Args>(args)...);
}

template <typename... Args> void Logger::error(Args &&...args) {
  log(std::cout, Level::ERROR, std::forward<Args>(args)...);
}

template <typename... Args> void Logger::debug(Args &&...args) {
  log(std::cerr, Level::DEBUG, std::forward<Args>(args)...);
}