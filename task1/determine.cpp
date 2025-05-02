#include "determine.hpp"

static float det_sequence(const Matrix &matrix) {
  auto matrix_size = matrix.size();
  if (matrix_size == 1) {
    return matrix.data()[0][0];
  }
  if (matrix_size == 2) {
    return matrix.data()[0][0] * matrix.data()[1][1] - matrix.data()[1][0] * matrix.data()[0][1];
  }

  float result = 0;
  for (auto &&[i, num] : std::ranges::enumerate_view(matrix.data()[0])) {
    auto minor = matrix.minor(std::size_t(0), i);
    auto minor_det = det_sequence(minor);
    result += num * minor_det * (i % 2 == 0 ? 1 : -1);
  }

  return result;
}

float det(const Matrix &matrix, std::size_t thread_num) {
  auto matrix_size = matrix.size();
  if (matrix_size == 1) {
    return matrix.data()[0][0];
  }
  if (matrix_size == 2) {
    return matrix.data()[0][0] * matrix.data()[1][1] - matrix.data()[1][0] * matrix.data()[0][1];
  }

  std::vector<std::thread> threads;
  thread_num = std::min(thread_num, matrix_size);
  threads.reserve(thread_num);

  std::atomic<float> result = 0;
  for (std::size_t i = 0; i < thread_num; ++i) {
    threads.emplace_back([&, i]() {
      auto idx = i;
      while (idx < matrix_size) {
        auto num = matrix.data()[0][idx];
        auto minor = matrix.minor(std::size_t(0), idx);
        auto minor_det = det_sequence(minor);
        result += num * minor_det * (idx % 2 == 0 ? 1 : -1);
        idx += thread_num;
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  return result;
}