#include "numeric_methods.hpp"

std::string get_method_name(NumericalMethod method) {
    switch (method) {
        case NumericalMethod::BISECTION:
            return "Bisection";
        case NumericalMethod::NEWTON:
            return "Newton";
        case NumericalMethod::MODIFIED_NEWTON:
            return "Modified Newton";
        case NumericalMethod::SECANT:
            return "Secant";
        default:
            return "Unknown";
    }
}

std::vector<Interval> find_sign_change_intervals(const Interval& range, MathFunction f, uint32_t num_steps) {
    if (!f) {
        throw std::runtime_error("Invalid function reference");
    }

    std::vector<Interval> sign_change_intervals;
    double step = range.length() / num_steps;
    double current = range.start;

    for (uint32_t i = 0; i < num_steps; i++) {
        double next = current + step;
        // Проверяем смену знака
        if (f(current) * f(next) <= 0) {
            sign_change_intervals.emplace_back(current, next);
        }
        current = next;
    }

    return sign_change_intervals;
}

double bisection_method(const Interval& range, double eps, MathFunction f, uint32_t max_iter) {
    double a = range.start;
    double b = range.end;

    if (f(a) * f(b) > 0) {
        throw std::runtime_error("Function values have same sign at interval endpoints");
    }

    if (std::abs(f(a)) < eps) return a;
    if (std::abs(f(b)) < eps) return b;

    for (uint32_t iter = 0; iter < max_iter; iter++) {
        double c = (a + b) / 2.0;
        double fc = f(c);

        if (std::abs(fc) <= eps || (b - a) / 2.0 < eps) {
            return c;
        }

        if (f(a) * fc < 0) {
            b = c;
        } else {
            a = c;
        }
    }

    return (a + b) / 2.0;
}

double newton_method(const Interval& range, double eps, MathFunction f, MathFunction df, uint32_t max_iter) {
    if (!df) {
        throw std::runtime_error("Derivative function is required for Newton method");
    }

    double x = range.midpoint();

    for (uint32_t iter = 0; iter < max_iter; iter++) {
        double fx = f(x);
        double dfx = df(x);

        if (std::abs(fx) <= eps) {
            return x;
        }

        if (std::abs(dfx) < 1e-15) {
            throw std::runtime_error("Derivative is too close to zero");
        }

        x = x - fx / dfx;
    }

    return x;
}

double modified_newton_method(const Interval& range, double eps, MathFunction f, MathFunction df, uint32_t max_iter) {
    if (!df) {
        throw std::runtime_error("Derivative function is required for modified Newton method");
    }

    double x = range.midpoint();
    double dfx0 = df(x);

    if (std::abs(dfx0) < 1e-15) {
        throw std::runtime_error("Initial derivative is too close to zero");
    }

    for (uint32_t iter = 0; iter < max_iter; iter++) {
        double fx = f(x);

        if (std::abs(fx) <= eps) {
            return x;
        }

        x = x - fx / dfx0;
    }

    return x;
}

double secant_method(const Interval& range, double eps, MathFunction f, uint32_t max_iter) {
    double x0 = range.start;
    double x1 = range.end;

    for (uint32_t iter = 0; iter < max_iter; iter++) {
        double fx0 = f(x0);
        double fx1 = f(x1);

        if (std::abs(fx1) <= eps) {
            return x1;
        }

        if (std::abs(fx1 - fx0) < 1e-15) {
            throw std::runtime_error("Function values are too close");
        }

        double x2 = x1 - fx1 * (x1 - x0) / (fx1 - fx0);
        x0 = x1;
        x1 = x2;
    }

    return x1;
}

double solve_equation(const Interval& range, double epsilon,
                     MathFunction f, MathFunction df,
                     NumericalMethod method, uint32_t max_iterations) {
    if (!f) {
        throw std::runtime_error("Invalid function reference");
    }

    switch (method) {
        case NumericalMethod::BISECTION:
            return bisection_method(range, epsilon, f, max_iterations);
        case NumericalMethod::NEWTON:
            return newton_method(range, epsilon, f, df, max_iterations);
        case NumericalMethod::MODIFIED_NEWTON:
            return modified_newton_method(range, epsilon, f, df, max_iterations);
        case NumericalMethod::SECANT:
            return secant_method(range, epsilon, f, max_iterations);
        default:
            throw std::runtime_error("Unknown numerical method");
    }
}