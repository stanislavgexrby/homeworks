#include "../source/hashtable.hpp"
#include <print>
#include <thread>
#include <iostream>

static bool test_add() {
    HashTable table(1000);

    uint32_t n = 1000;
    for (int i = 0; i < n; i++) {
        table.Put(i, Value {"hi", i});
    }

    //std::vector<bool> found(n, false);

    for (int j = 0; j < n; j++) {
        if (table.Check(j) == std::nullopt) {
            return false;
            //found[j] = true;
        }
    }

    return true;
    //return std::all_of(found.begin(), found.end(), [](bool a){ return a; });
}

static bool test_add_parallel() {
    HashTable table(1000);

    uint32_t n_per_thread = 347;
    uint32_t threads_num = 5;
    std::vector<std::jthread> threads;
    threads.reserve(threads_num);
    std::atomic<int> count = 0;

    for (int i = 0; i < threads_num; i++) {
        threads.emplace_back([&table, n_per_thread, i, &count]() {
            for (int j = n_per_thread * i; j < n_per_thread * (i + 1); j++) {
                table.Put(count.load(), {"", j});
                count.fetch_add(1);
            }
        });
    }
    std::cout << count.load() << std::endl;

    for (auto &th : threads) {
        th.join();
    }

    //auto n = n_per_thread * threads_num;
    //std::vector<bool> found(n, false);

    //auto node = list._first.load()->next.load();
    /*while (node != nullptr) {
        found[node->value.d] = true;
        // std::println("{}", node->value.d);
        node = node->next.load();
    }

    if (list.size() != n) {
        return false;
    }*/
    //std::cout << count.load() << std::endl;
    for (int i = 0; i < count.load(); i++) {
        if (table.Check(i) == std::nullopt) {
            return false;
        }
    }

    return true;
}

bool test() {
    using TestCaseT = std::pair<std::function<bool()>, std::string_view>;

    std::vector<TestCaseT> cases {
        {test_add, "test_add"},
        {test_add_parallel, "test_add_parallel"}
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