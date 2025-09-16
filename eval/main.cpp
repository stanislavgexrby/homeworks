#include "methods.hpp"

void interractive_solve(const Range &range, float eps, ComputedFunc f, ComputedFunc df) {
  auto change_sign_ranges = split_ranges(range, f);

  std::println("ranges with sign changing:");
  for (uint32_t i = 0; i < change_sign_ranges.size(); i++) {
    std::println("  {}: {}", i, change_sign_ranges[i].to_str());
  }
  std::println("");
  std::print("choose range: ");
  uint32_t range_idx = 0;
  std::cin >> range_idx;
  if (range_idx >= change_sign_ranges.size()) {
    throw std::runtime_error(std::format("invalid range_idx: {}", range_idx));
  }

  std::print("choose method ({} - Bissection, {} - Newton, {} - Newton2, {} - Secants): ",
    std::to_underlying(SolveMethod::Bissection), solve_method_name(SolveMethod::Bissection),
    std::to_underlying(SolveMethod::Newton), solve_method_name(SolveMethod::Newton),
    std::to_underlying(SolveMethod::Newton2), solve_method_name(SolveMethod::Newton2),
    std::to_underlying(SolveMethod::Secants), solve_method_name(SolveMethod::Secants));
  uint32_t solve_method;
  std::cin >> solve_method;

  double root = solve(change_sign_ranges[range_idx], eps, f, df, static_cast<SolveMethod>(solve_method));

  std::println("root = {}\n\n", root);
}

static double default_f(double x) { return std::pow(x - 2, 2) - 1; }
static double default_df(double x) { return 2 * (x - 2); }

int main() {
  auto range = Range::parse();
  std::print("input eps: ");
  double eps;
  std::cin >> eps;
  interractive_solve(range, eps, default_f, default_df);
}