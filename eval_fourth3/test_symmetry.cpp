#include <iostream>
#include <iomanip>
#include <cmath>
#include <functional>

using namespace std;

// f(x) = 1.27*x^5 + 2.04*x
double f(double x) { return 1.27*x*x*x*x*x + 2.04*x; }
double F(double x) { return 1.27*x*x*x*x*x*x/6.0 + 2.04*x*x/2.0; }

// СКФ средних прямоугольников
double middleRectangle(function<double(double)> f, double a, double b, int m) {
    double h = (b - a) / m;
    double sum = 0.0;
    for (int i = 0; i < m; i++) {
        sum += f(a + (i + 0.5) * h);
    }
    return h * sum;
}

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

void test_interval(double a, double b, int m) {
    double J_exact = F(b) - F(a);
    double J_middle = middleRectangle(f, a, b, m);
    double J_simpson = simpson(f, a, b, m);

    cout << "\n========================================\n";
    cout << "Интервал: [" << a << ", " << b << "], m = " << m << "\n";
    cout << "========================================\n";

    cout << "Точное значение J = " << fixed << setprecision(15) << J_exact << "\n\n";

    cout << "Средние прямоугольники:\n";
    cout << "  J(h) = " << setprecision(15) << J_middle << "\n";
    cout << "  |J - J(h)| = " << scientific << setprecision(16) << abs(J_exact - J_middle) << "\n";

    cout << fixed;
    cout << "Симпсон:\n";
    cout << "  J(h) = " << setprecision(15) << J_simpson << "\n";
    cout << "  |J - J(h)| = " << scientific << setprecision(16) << abs(J_exact - J_simpson) << "\n";
}

int main() {
    cout << "========================================\n";
    cout << "АНАЛИЗ СИММЕТРИИ\n";
    cout << "Функция: f(x) = 1.27*x^5 + 2.04*x\n";
    cout << "========================================\n";

    int m = 10000;

    // Тест 1: Симметричный интервал [-5, 5]
    test_interval(-5.0, 5.0, m);

    // Тест 2: Симметричный интервал [-90, 90]
    test_interval(-90.0, 90.0, m);

    // Тест 3: Нeсимметричный интервал [-1, 5]
    test_interval(-1.0, 5.0, m);

    return 0;
}
