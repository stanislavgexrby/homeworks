#include "../source/hashtable.hpp"
#include <print>
#include <thread>
#include <iostream>
#include <random>

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

constexpr int NUM_THREADS = 20;
constexpr int NUM_OPERATIONS = 10'000;
constexpr int KEY_RANGE = 100;

void stress_test(HashTable& table) {
    thread_local std::mt19937 gen(std::random_device{}());
    thread_local std::uniform_int_distribution<int> key_dist(1, KEY_RANGE);
    thread_local std::uniform_int_distribution<int> op_dist(0, 2);

    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        int key = key_dist(gen);
        Value val{"test", key};

        switch (op_dist(gen)) {
            case 0:
                table.put(key, val);
                break;
            case 1:
                table.remove(key);
                break;
            case 2:
                table.check(key);
                break;
        }
    }
}

static bool test_stress() {
    HashTable table;

    std::vector<std::jthread> threads;
    threads.reserve(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(stress_test, std::ref(table));
    }

    threads.clear();
    return true;
}

bool test() {
    using TestCaseT = std::pair<std::function<bool()>, std::string_view>;

    std::vector<TestCaseT> cases {
        {test_add, "test_add"},
        {test_add_parallel, "test_add_parallel"},
        {test_add_1000_parallel, "test_add_1000_parallel"},
        {test_stress, "test_stress"}
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