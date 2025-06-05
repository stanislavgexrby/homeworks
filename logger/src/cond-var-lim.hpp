#pragma once

#include "common.hpp"
#include "message.hpp"

class CondVarLimLogger : public Logger {
private:
  std::queue<Message> queue;
  std::size_t size = 0;

  std::mutex mutex;
  std::condition_variable cv_pop;
  std::condition_variable cv_push;

  std::atomic<bool> end = false;
public:
  CondVarLimLogger(std::ostream &out, std::size_t _size = 10) : Logger(out), size(_size) {
    log_thread = std::jthread([&](std::stop_token stop_token) {
      while (!end && !stop_token.stop_requested()) {
        Message msg;
        bool was_poped = false;

        std::unique_lock lock(mutex);
        cv_pop.wait(lock, [this]() {
          return !queue.empty() || end;
        });

        if (!queue.empty()) {
          msg = std::move(queue.front());
          queue.pop();
          cv_push.notify_one();
          was_poped = true;
        }

        if (was_poped) {
          out << msg.data;
        } else {
          std::this_thread::sleep_for(100ns);
        }
      }
    });
  }

  void add_msg(std::string_view msg) override {
    if (end) {
      return;
    }

    std::unique_lock lock(mutex);
    cv_push.wait(lock, [this]() {
      return queue.size() < size || end;
    });
    if (end) {
      return;
    }

    queue.push(std::move(msg));
    cv_pop.notify_one();
  }

  ~CondVarLimLogger() {
    end = true;
    cv_pop.notify_one();
    cv_push.notify_all();
    log_thread.join();
  }
};