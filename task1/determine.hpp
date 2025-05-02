#include "matrix.hpp"

static size_t number_of_threads = 0;
static size_t max_number_of_threads = std::thread::hardware_concurrency();
static std::mutex mutex;

void determinant(const Matrix &matrix, float &result);
float det(const Matrix &matrix, std::size_t thread_num = 10);

