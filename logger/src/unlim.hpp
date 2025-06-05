#pragma once

#include "common.hpp"
#include "message.hpp"

class UnlimLogger : public Logger {
  std::queue<Message> queue;

  std::mutex mutex;

  std::atomic<bool> end = false;
public:
  UnlimLogger(std::ostream &out) : Logger(out) {
    log_thread = std::jthread([&](std::stop_token stop_token) {
      while (!end && !stop_token.stop_requested()) {
        Message msg;
        while (!end) {
          std::unique_lock lock(mutex);
          if (queue.empty()) {
            lock.unlock();
            std::this_thread::sleep_for(10ns);
          } else {
            msg = std::move(queue.front());
            queue.pop();
            break;
          }
        }
        if (end) {
          break;
        }

        out << msg.data;
      }
    });
  }

  void add_msg(std::string_view msg) override {
    if (end) {
      return;
    }
    std::lock_guard lock(mutex);
    queue.push(Message(msg));
  }

  ~UnlimLogger() {
    end = true;
    log_thread.join();
  }
};