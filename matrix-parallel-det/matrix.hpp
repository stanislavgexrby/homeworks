#pragma once

#include <string_view>
#include <atomic>
#include <functional>
#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>
#include <thread>
#include <assert.h>
#include <list>
#include <fstream>
#include <mutex>
#include <ostream>

class Matrix {
  std::vector<std::vector<float>> _matrix;
  Matrix() {}
public:
  Matrix(const int n) {
    _matrix = std::vector(n, std::vector(n, 0.0f));
    for (int i = 0; i < n; i++) {
      _matrix[i][i] = 1;
    }
  }

  Matrix(const std::vector<std::vector<float>> &data) : _matrix(data) {}
  Matrix(std::vector<std::vector<float>> &&data) : _matrix(std::move(data)) {}

  Matrix minor(std::size_t row, std::size_t col) const {
    std::size_t n = _matrix.size();
    std::vector matrix(n - 1, std::vector(n - 1, 0.0f));

    std::size_t i_idx = 0;
    for (std::size_t i = 0; i < n; i++) {
      if (i == row) {
        continue;
      }

      std::size_t j_idx = 0;
      for (std::size_t j = 0; j < n; j++) {
        if (j == col) {
          continue;
        }
        matrix[i_idx][j_idx] = _matrix[i][j];
        j_idx++;
      }
      i_idx++;
    }

    Matrix ret;
    ret._matrix = std::move(matrix);
    return ret;
  }

  const std::vector<std::vector<float>> & data() const {
    return _matrix;
  }

  std::size_t size() const {
    return _matrix.size();
  }
};