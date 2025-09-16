#include "methods.hpp"

static double bissection_method(const Range &range, double eps, ComputedFunc input_func,
                                uint32_t iteration_max_number) {
  auto [start, end] = range.bounds();
  auto func = input_func;
  if (std::abs(func(start)) < eps) {
    return start;
  }
  if (std::abs(func(end)) < eps) {
    return end;
  }

  // We always work with increased function - this change is not make sense for root
  if (func(end) < 0) {
    func = [input_func](double x) { return -input_func(x); };
  }

  for (uint32_t iter = 0; iter < iteration_max_number; iter++) {
    double mid = std::midpoint(start, end);
    double value = func(mid);
    if (std::abs(value) <= eps) {
      return mid;
    }
    if (value > 0) {
      end = mid;
    } else {
      start = mid;
    }
  }
  return std::numeric_limits<double>::max();
}

static double newton_method(const Range &range, double eps, ComputedFunc func, ComputedFunc df,
                            uint32_t iteration_max_number) {
  if (not df) {
    throw std::runtime_error("df in newton method must not be null");
  }

  double x = std::midpoint(range.start, range.end);
  uint32_t iter = 0;
  while (std::abs(func(x)) >= eps && iter++ < iteration_max_number) {
    x = x - (func(x) / df(x));
  }
  return x;
}

static double updated_newton_method(const Range &range, double eps, ComputedFunc func,
                                    ComputedFunc df, uint32_t iteration_max_number) {
  if (not df) {
    throw std::runtime_error("df in newton method must not be null");
  }

  double x = std::midpoint(range.start, range.end);
  double dfx0 = df(x);
  uint32_t iter = 0;
  while (std::abs(func(x)) >= eps && iter++ < iteration_max_number) {
    x = x - (func(x) / dfx0);
  }
  return x;
}

static double method_secants(const Range &range, double eps, ComputedFunc func,
                             uint32_t iteration_max_number) {
  double x0 = range.start;
  double x1 = range.end;
  uint32_t iter = 0;
  while (std::abs(func(x1)) >= eps && iter++ < iteration_max_number) {
    auto save = x1;
    double fx1 = func(x1);
    double inv_df = (x1 - x0) / (fx1 - func(x0));
    x1 = x1 - (fx1 * inv_df);
    x0 = save;
  }
  return x1;
}

std::vector<Range> split_ranges(const Range &range, ComputedFunc f, uint32_t step_numbers) {
  if (not f) {
    throw std::runtime_error("invalid function reference");
  }

  std::vector<Range> change_sign_ranges;
  double step = range.length() / step_numbers;
  double start = range.start;
  for (uint32_t i = 0; i < step_numbers; i++) {
    double end = start + step;
    if (f(start) * f(end) <= 0) {
      change_sign_ranges.emplace_back(start, end);
    }
    start = end;
  }
  return change_sign_ranges;
}

double solve(const Range &range, float eps,
             ComputedFunc f, ComputedFunc df,
             SolveMethod method, uint32_t iteration_max_number) {
  if (not f) {
    throw std::runtime_error("invalid function reference");
  }

  switch (method) {
  case SolveMethod::Bissection:
    return bissection_method(range, eps, f, iteration_max_number);
  case SolveMethod::Newton:
    return newton_method(range, eps, f, df, iteration_max_number);
  case SolveMethod::Newton2:
    return updated_newton_method(range, eps, f, df, iteration_max_number);
  case SolveMethod::Secants:
    return method_secants(range, eps, f, iteration_max_number);
  default:
    throw std::runtime_error(std::format("invalid method number: {}", std::to_underlying(method)));
  }
}