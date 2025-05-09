#include "../source/hashtable.hpp"
#include <print>

static bool test1() {
    return 0;
}

bool test() {
    using TestCaseT = std::pair<std::function<bool()>, std::string_view>;

    std::vector<TestCaseT> cases {
      {test1, "test1"},
    };

    bool test_passed = true;
    for (auto &&[test_case, name] : cases) {
      auto result = test_case();
      std::printf("case: {}; result: {}", name, result ? "passed" : "failed");

      if (!result) {
        test_passed = false;
      }
    }

    return test_passed;
  }