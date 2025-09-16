#pragma once
#include <iostream>
#include <functional>
#include <numeric>
#include <utility>
#include <cmath>
#include <cassert>
#include <format>
#include <cstdint>
#include <vector>
#include <string>

using MathFunction = std::function<double(double)>;

struct Interval {
    double start = 0.0;
    double end = 0.0;

    Interval(double a = 0.0, double b = 0.0) : start(a), end(b) {
        if (a > b) {
            throw std::runtime_error("Invalid interval: start > end");
        }
    }

    static Interval read_from_console() {
        double a, b;
        std::cout << "Enter interval start: ";
        std::cin >> a;
        std::cout << "Enter interval end: ";
        std::cin >> b;
        return Interval(a, b);
    }

    std::string to_string() const {
        return std::format("[{:.6f}, {:.6f}]", start, end);
    }

    double length() const {
        return end - start;
    }

    double midpoint() const {
        return (start + end) / 2.0;
    }
};

enum class NumericalMethod : uint32_t {
    BISECTION = 0,
    NEWTON = 1,
    MODIFIED_NEWTON = 2,
    SECANT = 3
};

std::string get_method_name(NumericalMethod method);

std::vector<Interval> find_sign_change_intervals(const Interval& range, MathFunction f, uint32_t num_steps = 100);

double solve_equation(const Interval& range, double epsilon,
                     MathFunction f, MathFunction df,
                     NumericalMethod method, uint32_t max_iterations = 10000);

double bisection_method(const Interval& range, double eps, MathFunction f, uint32_t max_iter);
double newton_method(const Interval& range, double eps, MathFunction f, MathFunction df, uint32_t max_iter);
double modified_newton_method(const Interval& range, double eps, MathFunction f, MathFunction df, uint32_t max_iter);
double secant_method(const Interval& range, double eps, MathFunction f, uint32_t max_iter);