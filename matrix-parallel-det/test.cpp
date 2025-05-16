#include "determine.hpp"

#include <print>

static bool test_low_det() {
  size_t max_size = 10;
  for (int n = 1; n < max_size; n++) {
    auto data = std::vector(n, std::vector(n, 0.0f));
    for (int i = 0; i < n; i++) {
      data[i][i] = 1;
    }
    auto matrix = Matrix(std::move(data));
    if (det_low(matrix, 20) != 1) {
      return false;
    }
  }

  float det = 24;
  int n = 4;
  auto data = std::vector(n, std::vector(n, 0.0f));
  data = {
    {1, 5, 6, 7},
    {0, 2, 8, 9},
    {0, 0, 3, 4},
    {0, 0, 0, 4}
  };

  if (det_low(Matrix(std::move(data)), 20) != det) {
    return false;
  }

  det = 25;
  n = 3;
  data = {
    {1, 3, 5},
    {4, 0, 1},
    {2, 1, 0},
  };

  if (det_low(Matrix(std::move(data)), 20) != det) {
    return false;
  }

  det = -1;
  n = 4;
  data = {
    {0, 0, 0, 1},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {1, 0, 0, 0}
  };

  if (det_low(Matrix(std::move(data)), 20) != det) {
    return false;
  }

  return true;
}

static bool test_high_det() {
  size_t max_size = 10;
  for (int n = 1; n < max_size; n++) {
    auto data = std::vector(n, std::vector(n, 0.0f));
    for (int i = 0; i < n; i++) {
      data[i][i] = 1;
    }
    auto matrix = Matrix(std::move(data));
    float result;
    det_high(matrix, result);
    if (result != 1) {
      return false;
    }
  }

  float det = -53016;
  int n = 4;
  auto data = std::vector(n, std::vector(n, 0.0f));
  data = {
    {1, 11, 43, 87},
    {3, 0, 1, 4},
    {5, 47, 0, 1},
    {11, 12, 3, 4},
  };
  float result;
  det_high(Matrix(std::move(data)), result);

  if (result != det) {
    return false;
  }

  det = 25;
  n = 3;
  data = {
    {1, 3, 5},
    {4, 0, 1},
    {2, 1, 0},
  };
  det_high(Matrix(std::move(data)), result);

  if (result != det) {
    return false;
  }

  return true;
}

static bool test_low_swapped() {
  std::vector<std::vector<float>> original = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12},
    {13, 14, 15, 16}
  };
  std::vector<std::vector<float>> swapped = {
    {4, 3, 2, 1},
    {8, 7, 6, 5},
    {12, 11, 10, 9},
    {16, 15, 14, 13}
  };

  if (det_low(Matrix(std::move(original)), 20) != (-1) * det_low(Matrix(std::move(swapped)), 20)) {
    return false;
  }

  return true;
}

static bool test_high_swapped() {
  std::vector<std::vector<float>> original = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12},
    {13, 14, 15, 16}
  };
  std::vector<std::vector<float>> swapped = {
    {4, 3, 2, 1},
    {8, 7, 6, 5},
    {12, 11, 10, 9},
    {16, 15, 14, 13}
  };

  float result1;
  float result2;
  det_high(Matrix(std::move(original)), result1);
  det_high(Matrix(std::move(swapped)), result2);

  if (result1 != (-1) * result2) {
    return false;
  }

  return true;
}

bool test() {
  using TestCaseT = std::pair<std::function<bool()>, std::string_view>;

  std::vector<TestCaseT> cases {
    {test_low_det, "test_low_det"},
    {test_high_det, "test_high_det"},
    {test_low_swapped, "test_low_swapped"},
    {test_high_swapped, "test_high_swapped"}
  };

  bool test_passed = true;
  std::cout << std::endl << "Testing:" << std::endl;
  for (auto &&[test_case, name] : cases) {
    auto result = test_case();
    std::println("CASE: {}; RESULT: {}", name, result ? "passed" : "failed");

    if (!result) {
      test_passed = false;
    }
  }

  return test_passed;
}