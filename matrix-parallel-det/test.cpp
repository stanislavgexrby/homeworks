#include "determine.hpp"

#include <print>
#include <complex>

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

static bool is_float_equal(float value, float det, float scale = 1.0f) {
    const float abs_epsilon = std::numeric_limits<float>::epsilon();

    const float adaptive_epsilon = abs_epsilon * scale * 10;

    return std::fabs(value - det) < adaptive_epsilon;
}

static bool time_high_test() {
  std::vector<std::vector<float>> matrix = {
    {1, 6, 7, 43},
    {3, 6, 2, 1},
    {0, 3, 5, 6},
    {3, 33, 11, 9}
  };
  Matrix m(std::move(matrix));

  float det;
  std::cout << std::endl;
  for (int i = 1; i <= 20; i++) {
    max_number_of_threads = i;
    det = 0;

    auto start = std::chrono::high_resolution_clock::now();
    det_high(m, det);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "number of threads: " << i << "| time: " << duration.count() << std::endl;

    if (!is_float_equal(det, 11370)) {
      return false;
    }
  }
  std::cout << std::endl;

  return true;
}

static bool time_low_test() {
  // std::vector<std::vector<float>> matrix = {
  //   {0, 4, 1, 6, 9},
  //   {1, 4, 0, 3, 5},
  //   {0, 4, 2, 8, 7},
  //   {0, 4, 3, 5, 1},
  //   {0, 4, 4, 6, 5}
  // };
  std::vector<std::vector<float>> matrix = {
    {1, 6, 7, 43},
    {3, 6, 2, 1},
    {0, 3, 5, 6},
    {3, 33, 11, 9}
  };
  Matrix m(std::move(matrix));

  float det;
  std::cout << std::endl;
  for (int i = 1; i <= 20; i++) {
    auto start = std::chrono::high_resolution_clock::now();
    det = det_low(m, i);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "number of threads: " << i << "| time: " << duration.count() << std::endl;

    if (!is_float_equal(det, 11370)) {
      return false;
    }
  }
  std::cout << std::endl;

  return true;
}

bool test() {
  using TestCaseT = std::pair<std::function<bool()>, std::string_view>;

  std::vector<TestCaseT> cases {
    {test_low_det, "test_low_det"},
    {test_high_det, "test_high_det"},
    {test_low_swapped, "test_low_swapped"},
    {test_high_swapped, "test_high_swapped"},
    {time_high_test, "time_high_test"},
    {time_low_test, "time_low_test"}
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