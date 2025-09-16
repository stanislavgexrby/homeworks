#include "numeric_methods.hpp"
#include <iomanip>

void solve_interactive(const Interval& range, double eps, MathFunction f, MathFunction df) {
    std::cout << "Function: f(x) = 5*sin(2x) - sqrt(1-x)\n";
    std::cout << "Interval: " << range.to_string() << "\n";
    std::cout << "Epsilon: " << eps << "\n\n";

    auto sign_change_intervals = find_sign_change_intervals(range, f);

    std::cout << "Sign change intervals found:\n";
    for (size_t i = 0; i < sign_change_intervals.size(); i++) {
        std::cout << "  " << i << ": " << sign_change_intervals[i].to_string() << "\n";
    }

    if (sign_change_intervals.empty()) {
        std::cout << "No sign change intervals found!\n";
        return;
    }

    std::cout << "\nChoose interval (0-" << (sign_change_intervals.size() - 1) << "): ";
    size_t interval_idx;
    std::cin >> interval_idx;

    if (interval_idx >= sign_change_intervals.size()) {
        throw std::runtime_error("Invalid interval index");
    }

    const auto& chosen_interval = sign_change_intervals[interval_idx];
    std::cout << "\nSolving on interval: " << chosen_interval.to_string() << "\n";
    std::cout << std::fixed << std::setprecision(10);

    std::array<NumericalMethod, 4> methods = {
        NumericalMethod::BISECTION,
        NumericalMethod::NEWTON,
        NumericalMethod::MODIFIED_NEWTON,
        NumericalMethod::SECANT
    };

    for (auto method : methods) {
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "Method: " << get_method_name(method) << "\n";

        try {
            double root = solve_equation(chosen_interval, eps, f, df, method);
            std::cout << "Root found: " << root << "\n";
            std::cout << "Function value at root: " << f(root) << "\n";
            std::cout << "Absolute residual: " << std::abs(f(root)) << "\n";
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
}

double test_function(double x) {
    return 5.0 * std::sin(2.0 * x) - std::sqrt(1.0 - x);
}

double test_function_derivative(double x) {
    return 10.0 * std::cos(2.0 * x) + 1.0 / (2.0 * std::sqrt(1.0 - x));
}

int main() {
    try {
        Interval test_range(-15.0, -10.0);
        double epsilon = 1e-6;

        solve_interactive(test_range, epsilon, test_function, test_function_derivative);

        std::cout << "\n\nFor custom function, enter parameters:\n";
        auto custom_range = Interval::read_from_console();
        std::cout << "Enter epsilon: ";
        double custom_eps;
        std::cin >> custom_eps;

        solve_interactive(custom_range, custom_eps, test_function, test_function_derivative);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}