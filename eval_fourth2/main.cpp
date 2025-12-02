#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

double f0(double x) {
    return 5.0;
}

double f1(double x) {
    return 2.0 * x + 3.0;
}

double f2(double x) {
    return x * x - 4.0 * x + 1.0;
}

double f3(double x) {
    return x * x * x - 2.0 * x * x + x - 1.0;
}

double f4(double x) {
    return sin(x);
}

double F0(double x) {
    return 5.0 * x;
}

double F1(double x) {
    return x * x + 3.0 * x;
}

double F2(double x) {
    return x * x * x / 3.0 - 2.0 * x * x + x;
}

double F3(double x) {
    return x * x * x * x / 4.0 - 2.0 * x * x * x / 3.0 + x * x / 2.0 - x;
}

double F4(double x) {
    return -cos(x);
}

// Точное значение интеграла
double exactIntegral(int funcNum, double a, double b) {
    switch(funcNum) {
        case 0: return F0(b) - F0(a);
        case 1: return F1(b) - F1(a);
        case 2: return F2(b) - F2(a);
        case 3: return F3(b) - F3(a);
        case 4: return F4(b) - F4(a);
        default: return 0.0;
    }
}

typedef double (*FuncPtr)(double);

FuncPtr getFunction(int funcNum) {
    switch(funcNum) {
        case 0: return f0;
        case 1: return f1;
        case 2: return f2;
        case 3: return f3;
        case 4: return f4;
        default: return nullptr;
    }
}

// КФ левого прямоугольника
double leftRectangle(FuncPtr f, double a, double b) {
    return (b - a) * f(a);
}

// КФ правого прямоугольника
double rightRectangle(FuncPtr f, double a, double b) {
    return (b - a) * f(b);
}

// КФ среднего прямоугольника
double middleRectangle(FuncPtr f, double a, double b) {
    return (b - a) * f((a + b) / 2.0);
}

// КФ трапеции
double trapezoid(FuncPtr f, double a, double b) {
    return (b - a) / 2.0 * (f(a) + f(b));
}

// КФ Симпсона
double simpson(FuncPtr f, double a, double b) {
    return (b - a) / 6.0 * (f(a) + 4.0 * f((a + b) / 2.0) + f(b));
}

int main() {
    cout << fixed << setprecision(10);

    while(true) {
        cout << "\n========================================\n";
        cout << "Выберите функцию для интегрирования:\n";
        cout << "0: f(x) = 5 (многочлен степени 0)\n";
        cout << "1: f(x) = 2x + 3 (многочлен степени 1)\n";
        cout << "2: f(x) = x^2 - 4x + 1 (многочлен степени 2)\n";
        cout << "3: f(x) = x^3 - 2x^2 + x - 1 (многочлен степени 3)\n";
        cout << "4: f(x) = sin(x)\n";
        cout << "5: Выход\n";
        cout << "Ваш выбор: ";

        int funcNum;
        cin >> funcNum;

        if(funcNum == 5) {
            cout << "Выход из программы.\n";
            break;
        }

        if(funcNum < 0 || funcNum > 4) {
            cout << "Неверный выбор. Попробуйте снова.\n";
            continue;
        }

        cout << "\nВведите концы промежутка интегрирования:\n";
        cout << "a = ";
        double a;
        cin >> a;
        cout << "b = ";
        double b;
        cin >> b;

        if(a >= b) {
            cout << "Ошибка: a должно быть меньше b!\n";
            continue;
        }
        int precision;
        cout << "Введите количество знаков после запятой: ";
        cin >> precision;
        cout << fixed << setprecision(precision);

        FuncPtr f = getFunction(funcNum);

        // Точное значение
        double exact = exactIntegral(funcNum, a, b);
        cout << "\n--- РЕЗУЛЬТАТЫ ---\n";
        cout << "Точное значение интеграла: " << exact << "\n\n";

        // КФ левого прямоугольника
        double leftRect = leftRectangle(f, a, b);
        cout << "КФ левого прямоугольника: " << leftRect << "\n";
        cout << "Абсолютная погрешность: " << fabs(exact - leftRect) << "\n\n";

        // КФ правого прямоугольника
        double rightRect = rightRectangle(f, a, b);
        cout << "КФ правого прямоугольника: " << rightRect << "\n";
        cout << "Абсолютная погрешность: " << fabs(exact - rightRect) << "\n\n";

        // КФ среднего прямоугольника
        double midRect = middleRectangle(f, a, b);
        cout << "КФ среднего прямоугольника: " << midRect << "\n";
        cout << "Абсолютная погрешность: " << fabs(exact - midRect) << "\n\n";

        // КФ трапеции
        double trap = trapezoid(f, a, b);
        cout << "КФ трапеции: " << trap << "\n";
        cout << "Абсолютная погрешность: " << fabs(exact - trap) << "\n\n";

        // КФ Симпсона
        double simp = simpson(f, a, b);
        cout << "КФ Симпсона: " << simp << "\n";
        cout << "Абсолютная погрешность: " << fabs(exact - simp) << "\n\n";

        cout << "========================================\n";
    }

    return 0;
}