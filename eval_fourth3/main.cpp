#include <iostream>
#include <iomanip>
#include <cmath>
#include <functional>
#include <string>

using namespace std;

double f1(double x) { return 1.0; } // константа
double F1(double x) { return x; }

double f2(double x) { return x; } // многочлен 1-й степени
double F2(double x) { return x*x / 2.0; }

double f3(double x) { return x*x; } // многочлен 2-й степени
double F3(double x) { return x*x*x / 3.0; }

double f4(double x) { return x*x*x; } // многочлен 3-й степени
double F4(double x) { return x*x*x*x / 4.0; }

double f5(double x) { return 1.27*x*x*x*x*x + 2.04*x; } // функция из задания
double F5(double x) { return 1.27*x*x*x*x*x*x/6.0 + 2.04*x*x/2.0; }

double f6(double x) { return sin(x); } // легко интегрируемая
double F6(double x) { return -cos(x); }

double f7(double x) { return exp(x); } // легко интегрируемая
double F7(double x) { return exp(x); }

double f8(double x) { return cos(x); } // тригонометрическая
double F8(double x) { return sin(x); }

double f9(double x) { return 1.0 / (1.0 + x*x); } // рациональная (arctg)
double F9(double x) { return atan(x); }

double f10(double x) { return x * exp(x); } // произведение
double F10(double x) { return (x - 1.0) * exp(x); }

double f11(double x) { return x * sin(x); } // произведение тригонометрической
double F11(double x) { return sin(x) - x * cos(x); }

// СКФ левых прямоугольников
double leftRectangle(function<double(double)> f, double a, double b, int m) {
    double h = (b - a) / m;
    double sum = 0.0;
    for (int i = 0; i < m; i++) {
        sum += f(a + i * h);
    }
    return h * sum;
}

// СКФ правых прямоугольников
double rightRectangle(function<double(double)> f, double a, double b, int m) {
    double h = (b - a) / m;
    double sum = 0.0;
    for (int i = 1; i <= m; i++) {
        sum += f(a + i * h);
    }
    return h * sum;
}

// СКФ средних прямоугольников
double middleRectangle(function<double(double)> f, double a, double b, int m) {
    double h = (b - a) / m;
    double sum = 0.0;
    for (int i = 0; i < m; i++) {
        sum += f(a + (i + 0.5) * h);
    }
    return h * sum;
}

// СКФ трапеций
double trapezoid(function<double(double)> f, double a, double b, int m) {
    double h = (b - a) / m;
    double sum = (f(a) + f(b)) / 2.0;
    for (int i = 1; i < m; i++) {
        sum += f(a + i * h);
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

// Уточнение по Рунге-Ромбергу
double rungeRomberg(double Jh, double Jhl, int l, int r) {
    return Jhl + (Jhl - Jh) / (pow(l, r) - 1);
}

void printResults(const string& name, double Jh, double J, double absErr, double relErr, int precision) {
    cout << name << ":\n";
    cout << "  J(h) = " << fixed << setprecision(precision) << Jh << "\n";
    cout << "  |J - J(h)| = " << fixed << setprecision(precision) << absErr << "\n";
    cout << "  |J - J(h)|/|J| = " << fixed << setprecision(precision) << relErr << "\n\n";
}

void calculateIntegrals(function<double(double)> f, function<double(double)> F,
                        double a, double b, int m, int precision, bool rungeMode = false, int l = 2,
                        double Jh_left = 0, double Jh_right = 0, double Jh_middle = 0,
                        double Jh_trap = 0, double Jh_simp = 0) {

    double h = (b - a) / m;
    double J = F(b) - F(a);

    cout << "\n========================================\n";
    if (!rungeMode) {
        cout << "Параметры задачи:\n";
        cout << "A = " << fixed << setprecision(precision) << a << ", B = " << b << ", m = " << m << "\n";
        cout << "h = " << fixed << setprecision(precision) << h << "\n";
        cout << "Точное значение интеграла J = " << fixed << setprecision(precision) << J << "\n";
        cout << "========================================\n\n";
    } else {
        cout << "Новые параметры:\n";
        cout << "A = " << fixed << setprecision(precision) << a << ", B = " << b << ", m = " << m << " (увеличено в " << l << " раз)\n";
        cout << "h/" << l << " = " << fixed << setprecision(precision) << h << "\n";
        cout << "========================================\n\n";
    }

    // Вычисление приближенных значений
    double J_left = leftRectangle(f, a, b, m);
    double J_right = rightRectangle(f, a, b, m);
    double J_middle = middleRectangle(f, a, b, m);
    double J_trap = trapezoid(f, a, b, m);
    double J_simp = simpson(f, a, b, m);

    if (!rungeMode) {
        // Основной режим
        printResults("СКФ левых прямоугольников", J_left, J,
                    abs(J - J_left), abs(J - J_left) / abs(J), precision);
        printResults("СКФ правых прямоугольников", J_right, J,
                    abs(J - J_right), abs(J - J_right) / abs(J), precision);
        printResults("СКФ средних прямоугольников", J_middle, J,
                    abs(J - J_middle), abs(J - J_middle) / abs(J), precision);
        printResults("СКФ трапеций", J_trap, J,
                    abs(J - J_trap), abs(J - J_trap) / abs(J), precision);
        printResults("СКФ Симпсона", J_simp, J,
                    abs(J - J_simp), abs(J - J_simp) / abs(J), precision);
    } else {
        // Режим уточнения по Рунге
        cout << "Результаты с новым шагом h/" << l << ":\n\n";
        printResults("СКФ левых прямоугольников", J_left, J,
                    abs(J - J_left), abs(J - J_left) / abs(J), precision);
        printResults("СКФ правых прямоугольников", J_right, J,
                    abs(J - J_right), abs(J - J_right) / abs(J), precision);
        printResults("СКФ средних прямоугольников", J_middle, J,
                    abs(J - J_middle), abs(J - J_middle) / abs(J), precision);
        printResults("СКФ трапеций", J_trap, J,
                    abs(J - J_trap), abs(J - J_trap) / abs(J), precision);
        printResults("СКФ Симпсона", J_simp, J,
                    abs(J - J_simp), abs(J - J_simp) / abs(J), precision);

        // Уточнение по Рунге-Ромбергу
        cout << "\n========================================\n";
        cout << "УТОЧНЕНИЕ ПО РУНГЕ-РОМБЕРГУ\n";
        cout << "========================================\n\n";

        int nameWidth = 20;
        int valueWidth = max(precision + 12, 16);
        int errorWidth = max(precision + 12, 16);

        cout << left << setw(nameWidth) << "Название СКФ" << " | "
             << right << setw(valueWidth) << "J(h)" << " | "
             << setw(errorWidth) << "|J - J(h)|" << " | "
             << setw(valueWidth) << "J(h/l)" << " | "
             << setw(errorWidth) << "|J - J(h/l)|" << " | "
             << setw(valueWidth) << "J_R" << " | "
             << setw(errorWidth) << "|J - J_R|" << "\n";

        int totalWidth = nameWidth + 3 + valueWidth + 3 + errorWidth + 3 +
                         valueWidth + 3 + errorWidth + 3 + valueWidth + 3 + errorWidth;
        cout << string(totalWidth, '-') << "\n";

        double JR_left = rungeRomberg(Jh_left, J_left, l, 1);
        cout << left << setw(nameWidth) << "Левые прямоуг." << " | "
             << right << fixed << setprecision(precision) << setw(valueWidth) << Jh_left << " | "
             << setw(errorWidth) << abs(J - Jh_left) << " | "
             << setw(valueWidth) << J_left << " | "
             << setw(errorWidth) << abs(J - J_left) << " | "
             << setw(valueWidth) << JR_left << " | "
             << setw(errorWidth) << abs(J - JR_left) << "\n";

        double JR_right = rungeRomberg(Jh_right, J_right, l, 1);
        cout << left << setw(nameWidth) << "Правые прямоуг." << " | "
             << right << fixed << setprecision(precision) << setw(valueWidth) << Jh_right << " | "
             << setw(errorWidth) << abs(J - Jh_right) << " | "
             << setw(valueWidth) << J_right << " | "
             << setw(errorWidth) << abs(J - J_right) << " | "
             << setw(valueWidth) << JR_right << " | "
             << setw(errorWidth) << abs(J - JR_right) << "\n";

        double JR_middle = rungeRomberg(Jh_middle, J_middle, l, 2);
        cout << left << setw(nameWidth) << "Средние прямоуг." << " | "
             << right << fixed << setprecision(precision) << setw(valueWidth) << Jh_middle << " | "
             << setw(errorWidth) << abs(J - Jh_middle) << " | "
             << setw(valueWidth) << J_middle << " | "
             << setw(errorWidth) << abs(J - J_middle) << " | "
             << setw(valueWidth) << JR_middle << " | "
             << setw(errorWidth) << abs(J - JR_middle) << "\n";

        double JR_trap = rungeRomberg(Jh_trap, J_trap, l, 2);
        cout << left << setw(nameWidth) << "Трапеции" << " | "
             << right << fixed << setprecision(precision) << setw(valueWidth) << Jh_trap << " | "
             << setw(errorWidth) << abs(J - Jh_trap) << " | "
             << setw(valueWidth) << J_trap << " | "
             << setw(errorWidth) << abs(J - J_trap) << " | "
             << setw(valueWidth) << JR_trap << " | "
             << setw(errorWidth) << abs(J - JR_trap) << "\n";

        double JR_simp = rungeRomberg(Jh_simp, J_simp, l, 4);
        cout << left << setw(nameWidth) << "Симпсона" << " | "
             << right << fixed << setprecision(precision) << setw(valueWidth) << Jh_simp << " | "
             << setw(errorWidth) << abs(J - Jh_simp) << " | "
             << setw(valueWidth) << J_simp << " | "
             << setw(errorWidth) << abs(J - J_simp) << " | "
             << setw(valueWidth) << JR_simp << " | "
             << setw(errorWidth) << abs(J - JR_simp) << "\n";
    }
}

int main() {
    cout << "========================================\n";
    cout << "Приближенное вычисление интеграла\n";
    cout << "по составным квадратурным формулам\n";
    cout << "Уточнение по Рунге-Ромбергу\n";
    cout << "========================================\n";

    while (true) {
        cout << "\nВыберите функцию для интегрирования:\n";
        cout << "Многочлены:\n";
        cout << "  1. f(x) = 1 (константа)\n";
        cout << "  2. f(x) = x (многочлен 1-й степени)\n";
        cout << "  3. f(x) = x^2 (многочлен 2-й степени)\n";
        cout << "  4. f(x) = x^3 (многочлен 3-й степени)\n";
        cout << "  5. f(x) = 1.27*x^5 + 2.04*x\n";
        cout << "Тригонометрические и экспоненциальные:\n";
        cout << "  6. f(x) = sin(x)\n";
        cout << "  7. f(x) = e^x\n";
        cout << "  8. f(x) = cos(x)\n";
        cout << "Другие функции:\n";
        cout << "  9. f(x) = 1/(1+x^2) [arctan]\n";
        cout << " 10. f(x) = x*e^x\n";
        cout << " 11. f(x) = x*sin(x)\n";
        cout << "  0. Выход\n";
        cout << "Ваш выбор: ";

        int choice;
        cin >> choice;

        if (choice == 0) break;

        function<double(double)> f, F;
        string funcName;

        switch (choice) {
            case 1: f = f1; F = F1; funcName = "f(x) = 1"; break;
            case 2: f = f2; F = F2; funcName = "f(x) = x"; break;
            case 3: f = f3; F = F3; funcName = "f(x) = x^2"; break;
            case 4: f = f4; F = F4; funcName = "f(x) = x^3"; break;
            case 5: f = f5; F = F5; funcName = "f(x) = 1.27*x^5 + 2.04*x"; break;
            case 6: f = f6; F = F6; funcName = "f(x) = sin(x)"; break;
            case 7: f = f7; F = F7; funcName = "f(x) = e^x"; break;
            case 8: f = f8; F = F8; funcName = "f(x) = cos(x)"; break;
            case 9: f = f9; F = F9; funcName = "f(x) = 1/(1+x^2)"; break;
            case 10: f = f10; F = F10; funcName = "f(x) = x*e^x"; break;
            case 11: f = f11; F = F11; funcName = "f(x) = x*sin(x)"; break;
            default: cout << "Неверный выбор!\n"; continue;
        }

        cout << "\nВыбрана функция: " << funcName << "\n";

        double a, b;
        int m;

        cout << "Введите нижний предел интегрирования A: ";
        cin >> a;
        cout << "Введите верхний предел интегрирования B: ";
        cin >> b;
        cout << "Введите число промежутков деления m: ";
        cin >> m;

        int precision;
        cout << "Введите количество знаков после запятой для вывода: ";
        cin >> precision;

        calculateIntegrals(f, F, a, b, m, precision);

        cout << "\nВыберите действие:\n";
        cout << "1. Уточнение по Рунге-Ромбергу\n";
        cout << "2. Ввести новые параметры\n";
        cout << "3. Вернуться к выбору функции\n";
        cout << "Ваш выбор: ";

        int action;
        cin >> action;

        if (action == 1) {
            cout << "\nВведите параметр l (во сколько раз увеличить число разбиений): ";
            int l;
            cin >> l;

            double Jh_left = leftRectangle(f, a, b, m);
            double Jh_right = rightRectangle(f, a, b, m);
            double Jh_middle = middleRectangle(f, a, b, m);
            double Jh_trap = trapezoid(f, a, b, m);
            double Jh_simp = simpson(f, a, b, m);

            calculateIntegrals(f, F, a, b, m * l, precision, true, l,
                             Jh_left, Jh_right, Jh_middle, Jh_trap, Jh_simp);
        } else if (action == 2) {
            continue;
        }
    }

    cout << "\nСпасибо за использование программы!\n";
    return 0;
}