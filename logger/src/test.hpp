#pragma once

#include "cond-var-unlim.hpp"
#include "cond-var-lim.hpp"
#include "lim.hpp"
#include "unlim.hpp"

template<typename LoggerT>
static void test_logger(bool random_delay = false) {
  std::ofstream out(std::format("logger:{},random_delay:{}.txt", typeid(LoggerT).name(), random_delay));
  LoggerT logger(out);

  size_t n = 18;
  std::vector<std::jthread> threads;
  threads.reserve(n);

  for (size_t th = 0; th < n; th++) {
    threads.emplace_back([&logger, th, random_delay](std::stop_token stop_token) {
      int stop = 1000;
      while (stop-- >= 0 && !stop_token.stop_requested()) {
        auto msg = std::format("{} thread\n", th);
        logger.add_msg(msg);
        auto delay = std::chrono::nanoseconds(random_delay ? (rand() % 100) : 10);
        std::this_thread::sleep_for(delay);
      }
    });
  }

  std::this_thread::sleep_for(10ms);

  std::println("logger:{},random_delay:{} - done", typeid(LoggerT).name(), random_delay ? "" : "out");
}

using Loggers = std::tuple<CondVarUnlimLogger, CondVarLimLogger, UnlimLogger, LimLogger>;

template<std::size_t index = 0>
void test() {
  if constexpr (index < std::tuple_size<Loggers>::value) {
    using T = std::tuple_element_t<index, Loggers>;
    test_logger<T>(false);
    test_logger<T>(true);
    test<index + 1>();
  }
}