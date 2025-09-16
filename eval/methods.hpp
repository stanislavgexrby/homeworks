#include <iostream>
#include <print>
#include <functional>
#include <numeric>
#include <utility>
#include <cmath>
#include <cassert>
#include <format>
#include <cstdint>
#include <iostream>

using ComputedFunc = std::function<double(double)>;

struct Range {
  double start = 0, end = 0;

  static Range parse() {
    Range range;
    std::cout << "input range start: ";
    std::cin >> range.start;
    std::cout << "input range end: ";
    std::cin >> range.end;
    if (range.start > range.end) {
      throw std::runtime_error(std::format("range start ({}) must be leq when range end ({})", range.start, range.end));
    }
    return range;
  }

  Range(double start = 0, double end = 0) : start(start), end(end) {
    if (start > end) {
      throw std::runtime_error(std::format("range start ({}) must be leq when range end ({})", start, end));
    }
  }

  std::string to_str() const {
    return std::format("[{}, {}]", start, end);
  }
  double length() const { return end - start; }
  std::tuple<double, double> bounds() const { return {start, end}; }
};

enum SolveMethod : uint32_t {
  Bissection,
  Newton,
  Newton2,
  Secants,
  MethodsNum,
};

constexpr std::string_view method_name(SolveMethod method) {
  switch (method) {
  case SolveMethod::Bissection:
    return "Bissection";
 case SolveMethod::Newton:
    return "Newton";
  case SolveMethod::Newton2:
    return "Newton2";
  case SolveMethod::Secants:
    return "Secants";
  default:
    assert(false);
  }
};

std::vector<Range> split_ranges(const Range &range, ComputedFunc f, uint32_t step_numbers = 40);

double solve(const Range &range, float eps,
             ComputedFunc f, ComputedFunc df,
             SolveMethod method, uint32_t iteration_max_number = 20000);