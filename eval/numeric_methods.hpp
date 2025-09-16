#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <functional>

using namespace std;

struct Interval {
    double a, b;
    Interval(double start, double end) : a(start), b(end) {}
};

// Результат решения методом
struct SolutionResult {
    double root;
    int iterations;
    double last_diff;
    double residual;
};

// Функции для решения уравнений
vector<Interval> find_sign_change_intervals(double A, double B, function<double(double)> f, int N = 1000);

SolutionResult bisection_method(const Interval& interval, function<double(double)> f, double eps);

SolutionResult newton_method(const Interval& interval, function<double(double)> f, 
                           function<double(double)> df, double x0, double eps);

SolutionResult modified_newton_method(const Interval& interval, function<double(double)> f, 
                                    function<double(double)> df, double x0, double eps);

SolutionResult secant_method(const Interval& interval, function<double(double)> f, double eps);

// Тестовая функция из задания №9
double test_function(double x);
double test_derivative(double x);