#include <iostream>
#include <iomanip>
#include <cmath>
#include <functional>

using namespace std;

double f(double x) { return exp(x); }
double F(double x) { return exp(x); }

// СКФ Симпсона
double simpson(function<double(double)> f, double a, double b, int m) {
    if (m % 2 != 0) m++;
    double h = (b - a) / m;
    double sum = f(a) + f(b);

    for (int i = 1; i < m; i++) {
        double coeff = (i % 2 == 0) ? 2.0 : 4.0;
        sum += coeff * f(a + i * h);
    }
    return h * sum / 3.0;
}

int main() {
    double a = 0.0;
    double b = 10.0;
    double J_exact = F(b) - F(a);

    cout << "========================================\n";
    cout << "ПРОВЕРКА НА ПРОЧНОСТЬ\n";
    cout << "Функция: f(x) = e^x\n";
    cout << "Пределы: [" << a << ", " << b << "]\n";
    cout << "Точное значение интеграла J = " << fixed << setprecision(10) << J_exact << "\n";
    cout << "========================================\n\n";

    cout << left << setw(15) << "m"
         << right << setw(25) << "J(Симпсон)"
         << setw(25) << "|J - J(Симпсон)|"
         << setw(25) << "Относительная погр." << "\n";
    cout << string(90, '-') << "\n";

    int m_values[] = {100, 1000, 10000, 100000, 1000000};

    for (int m : m_values) {
        double J_simpson = simpson(f, a, b, m);
        double abs_err = abs(J_exact - J_simpson);
        double rel_err = abs_err / abs(J_exact);

        cout << left << setw(15) << m
             << right << fixed << setprecision(10) << setw(25) << J_simpson
             << scientific << setprecision(4) << setw(25) << abs_err
             << setw(25) << rel_err << "\n";
    }

    return 0;
}
