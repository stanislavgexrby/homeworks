#pragma once

#include "common.hpp"
#include "message.hpp"

class LimLogger : public Logger {
private:
  std::queue<Message> queue;
  std::size_t size = 0;

  std::mutex mutex;

  std::atomic<bool> end = false;
public:
  LimLogger(std::ostream &out, std::size_t _size = 100) : Logger(out), size(_size) {
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

        out << msg.get();
      }
    });
  }

  void add_msg(std::string msg) override {
    while (!end) {
      std::unique_lock lock(mutex);
      if (end) {
        break;
      }

      if (queue.size() < size) {
        queue.push(Message(std::move(msg)));
        break;
      } else {
        lock.unlock();
        std::this_thread::sleep_for(10ns);
      }
    }
  }

  ~LimLogger() {
    end = true;
    log_thread.join();
  }
};