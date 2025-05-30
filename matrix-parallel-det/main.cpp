#include "determine.hpp"

Matrix load_matrix_from_file(const std::string &file_name) {
  std::ifstream file(file_name);

  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file");
  }

  int rows, cols;
  file >> rows >> cols;

  std::vector<std::vector<float>> matrix(rows, std::vector<float>(cols));

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      file >> matrix[i][j];
    }
  }
  file.close();

  return Matrix(matrix);
}

int main() {
  Matrix matrix1 = load_matrix_from_file("../data/matrix_4x4.txt");
  size_t n;
  std::cin >> n;
  max_number_of_threads = n;

  auto start1 = std::chrono::high_resolution_clock::now();
  auto res1 = det_low(matrix1);
  auto end1 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration1 = end1 - start1;

  std::cout << "low level parallel result :  " << res1 << '\n' << "time : " << duration1.count() << std::endl;
  float res2;

  std::cout << std::endl;

  auto start2 = std::chrono::high_resolution_clock::now();
  det_high(matrix1, res2);
  auto end2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration2 = end2 - start2;

  std::cout << "high level parallel result : " << res2 << '\n' << "time : " << duration2.count() << std::endl;

  return test() ? 0 : 1;
}
