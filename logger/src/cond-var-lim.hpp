#pragma once

#include "common.hpp"
#include "message.hpp"

class CondVarLimLogger : public Logger {
private:
  std::queue<Message> queue;
  std::size_t size = 0;

  std::mutex mutex;
  std::condition_variable cv_del;
  std::condition_variable cv_add;

  std::atomic<bool> end = false;
public:
  CondVarLimLogger(std::ostream &out, std::size_t _size = 10) : Logger(out), size(_size) {
    log_thread = std::jthread([&](std::stop_token stop_token) {
      while (!end && !stop_token.stop_requested()) {
        Message msg;
        bool was_poped = false;

        std::unique_lock lock(mutex);
        cv_del.wait(lock, [this]() {
          return !queue.empty() || end;
        });

        if (!queue.empty()) {
          msg = std::move(queue.front());
          queue.pop();
          cv_add.notify_one();
          was_poped = true;
        }

        if (was_poped) {
          out << msg.get();
        } else {
          std::this_thread::sleep_for(100ns);
        }
      }
    });
  }

  void add_msg(std::string msg) override {
    if (end) {
      return;
    }

    std::unique_lock lock(mutex);
    cv_add.wait(lock, [this]() {
      return queue.size() < size || end;
    });
    if (end) {
      return;
    }

    queue.push(Message(std::move(msg)));
    cv_del.notify_one();
  }

  ~CondVarLimLogger() {
    end = true;
    cv_del.notify_one();
    cv_add.notify_all();
    log_thread.join();
  }
};