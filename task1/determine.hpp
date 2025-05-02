#pragma once

#include "matrix.hpp"

static size_t number_of_threads = 0;
static size_t max_number_of_threads = std::thread::hardware_concurrency();
static std::mutex mutex;

void det_high(const Matrix &matrix, float &result);
float det_low(const Matrix &matrix, std::size_t thread_num = 10);

