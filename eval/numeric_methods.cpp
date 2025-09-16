#include "numeric_methods.hpp"

vector<Interval> find_sign_change_intervals(double A, double B, function<double(double)> f, int N) {
    vector<Interval> intervals;
    double h = (B - A) / N;

    for (int i = 0; i < N; i++) {
        double x1 = A + i * h;
        double x2 = A + (i + 1) * h;

        if (f(x1) * f(x2) <= 0) {
            intervals.push_back(Interval(x1, x2));
        }
    }

    return intervals;
}

SolutionResult bisection_method(const Interval& interval, function<double(double)> f, double eps) {
    double a = interval.a;
    double b = interval.b;
    int iterations = 0;
    double c;

    do {
        iterations++;
        c = (a + b) / 2.0;

        if (f(a) * f(c) < 0) {
            b = c;
        } else {
            a = c;
        }
    } while ((b - a) > eps && abs(f(c)) > eps);

    return {c, iterations, (b - a), abs(f(c))};
}

SolutionResult newton_method(const Interval& interval, function<double(double)> f,
                           function<double(double)> df, double x0, double eps) {
    double x = x0;
    double x_prev;
    int iterations = 0;

    do {
        iterations++;
        x_prev = x;
        double fx = f(x);
        double dfx = df(x);

        if (abs(dfx) < 1e-15) {
            cout << "Предупреждение: производная близка к нулю!" << endl;
            break;
        }

        x = x - fx / dfx;

    } while (abs(x - x_prev) > eps && abs(f(x)) > eps && iterations < 1000);

    return {x, iterations, abs(x - x_prev), abs(f(x))};
}

SolutionResult modified_newton_method(const Interval& interval, function<double(double)> f, 
                                    function<double(double)> df, double x0, double eps) {
    double x = x0;
    double x_prev;
    double df0 = df(x0);
    int iterations = 0;

    if (abs(df0) < 1e-15) {
        cout << "Ошибка: начальная производная близка к нулю!" << endl;
        return {x0, 0, 0, abs(f(x0))};
    }

    do {
        iterations++;
        x_prev = x;
        double fx = f(x);
        x = x - fx / df0;

    } while (abs(x - x_prev) > eps && abs(f(x)) > eps && iterations < 1000);

    return {x, iterations, abs(x - x_prev), abs(f(x))};
}

SolutionResult secant_method(const Interval& interval, function<double(double)> f, double eps) {
    double x0 = interval.a;
    double x1 = interval.b;
    double x2;
    int iterations = 0;

    do {
        iterations++;
        double f0 = f(x0);
        double f1 = f(x1);

        if (abs(f1 - f0) < 1e-15) {
            cout << "Предупреждение: значения функции слишком близки!" << endl;
            break;
        }

        x2 = x1 - f1 * (x1 - x0) / (f1 - f0);
        x0 = x1;
        x1 = x2;

    } while (abs(x1 - x0) > eps && abs(f(x1)) > eps && iterations < 1000);

    return {x1, iterations, abs(x1 - x0), abs(f(x1))};
}

double test_function(double x) {
    return 5.0 * sin(2.0 * x) - sqrt(1.0 - x);
}

double test_derivative(double x) {
    return 10.0 * cos(2.0 * x) + 1.0 / (2.0 * sqrt(1.0 - x));
}