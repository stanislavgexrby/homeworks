#pragma once

#include "common.hpp"
#include "message.hpp"

class CondVarUnlimLogger : public Logger {
private:
  std::queue<Message> queue;

  std::mutex mutex;
  std::condition_variable cv;

  std::atomic<bool> end = false;
public:
  CondVarUnlimLogger(std::ostream &out) : Logger(out) {
    log_thread = std::jthread([&](std::stop_token stop_token) {
      while (!end && !stop_token.stop_requested()) {
        Message msg;
        bool was_poped = false;

        std::unique_lock lock(mutex);
        cv.wait(lock, [this, &lock]() {
          return !queue.empty() || end;
        });

        if (!queue.empty()) {
          msg = std::move(queue.front());
          queue.pop();
          was_poped = true;
        }
        lock.unlock();

        if (was_poped) {
          out << msg.data;
        } else {
          std::this_thread::sleep_for(10ns);
        }
      }
    });
  }

  void add_msg(std::string_view msg) override {
    if (end) {
      return;
    }
    std::lock_guard lock(mutex);
    queue.push(Message(msg));
    cv.notify_one();
  }

  ~CondVarUnlimLogger() {
    end = true;
    cv.notify_one();
    log_thread.join();
  }
};