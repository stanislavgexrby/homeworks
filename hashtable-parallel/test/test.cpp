#include "../source/hashtable.hpp"
#include <print>
#include <thread>
#include <iostream>

static bool test_add() {
    HashTable table;

    uint32_t n = 10000;
    for (int i = 0; i < n; i++) {
        table.put(i, Value {"hi", i});
    }

    for (int j = 0; j < n; j++) {
        if (table.check(j) == std::nullopt) {
            return false;
        }
    }

    return true;
}

static bool test_add_1000_parallel() {
    HashTable table;
    uint32_t n_per_thread = 100;
    uint32_t threads_num = 10;
    std::vector<std::jthread> threads;
    threads.reserve(threads_num);

    for (int i = 0; i < threads_num; i++) {
        threads.emplace_back([&table, n_per_thread, i]() {
            for (int j = n_per_thread * i; j < n_per_thread * (i + 1); j++) {
                table.put(j, {"yeah", j});
            }
        });
    }
    threads.clear();

    for (int i = 0; i < 1000; i++) {
        if (table.check(i) == std::nullopt) {
            return false;
        }
    }

    return true;
}

static bool test_add_parallel() {
    HashTable table;

    uint32_t n_per_thread = 347;
    uint32_t threads_num = 5;
    std::vector<std::jthread> threads;
    threads.reserve(threads_num);
    std::atomic<int> count = 0;

    for (int i = 0; i < threads_num; i++) {
        threads.emplace_back([&table, n_per_thread, i, &count]() {
            for (int j = n_per_thread * i; j < n_per_thread * (i + 1); j++) {
                int current = count.fetch_add(1);
                table.put(current, {"yeah", j});
            }
        });
    }
    threads.clear();

    for (int i = 0; i < count.load(); i++) {
        if (table.check(i) == std::nullopt) {
            return false;
        }
    }

    return true;
}

bool test() {
    using TestCaseT = std::pair<std::function<bool()>, std::string_view>;

    std::vector<TestCaseT> cases {
        {test_add, "test_add"},
        {test_add_parallel, "test_add_parallel"},
        {test_add_1000_parallel, "test_add_1000_parallel"}
    };

    bool test_passed = true;
    for (auto &&[test_case, name] : cases) {
        auto result = test_case();
        std::println("case: {}; result: {}", name, result ? "passed" : "failed");

        if (!result) {
          test_passed = false;
        }
    }

    return test_passed;
  }