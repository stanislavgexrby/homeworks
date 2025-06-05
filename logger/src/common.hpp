#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <print>
#include <fstream>
#include <chrono>
#include <atomic>

using namespace std::chrono_literals;

class Logger {
protected:
  std::ostream &out;
  std::jthread log_thread;
public:
  Logger(std::ostream &out) : out(out) {}

  virtual void add_msg(std::string_view msg) = 0;
  virtual ~Logger() = default;
};