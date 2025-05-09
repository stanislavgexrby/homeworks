#include "determine.hpp"

void det_high(const Matrix &matrix, float &result){
  size_t matrix_size = matrix.size();

  if (matrix_size == 1) {
    result = matrix.data()[0][0];
    return;
  }
  if (matrix_size == 2) {
    result = matrix.data()[0][0] * matrix.data()[1][1] - matrix.data()[1][0] * matrix.data()[0][1];
    return;
  }

  float det = 0;
  std::vector<float> threads_results(matrix_size, 0);
//{
  std::vector<std::thread> threads;

  for (int i = 0; i < matrix_size; i++) {
    auto minor = matrix.minor(std::size_t(0), i);

    if (number_of_threads.load() < max_number_of_threads) {
      threads.emplace_back(det_high, minor, std::ref(threads_results[i]));
      number_of_threads.fetch_add(1);
    } else {
      det_high(minor, threads_results[i]);
    }
  }
//}

  for (auto &thread : threads) {
    thread.join();
  }

  for (int j = 0; j < matrix_size; j++) {
    det += (j % 2 == 0 ? 1 : -1) * matrix.data()[0][j] * threads_results[j];
  }

  result = det;
}