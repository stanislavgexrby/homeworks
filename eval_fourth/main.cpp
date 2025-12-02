#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <limits>

using namespace std;

double rho(double x) {
    return 1.0 / (x + 0.1);
}

double f0(double x) { return 3.5; }
double f1(double x) { return 2.0 * x + 1.5; }
double f2(double x) { return x * x - 3.0 * x + 2.0; }
double f3(double x) { return x * x * x - 2.0 * x * x + 4.0 * x - 1.0; }
double f4(double x) { return x * x * x * x + 0.5 * x * x * x - x * x + 2.0 * x + 3.0; }
double f5(double x) { return sin(x); }

double polynomialDegreeN(double x, int degree) {
    double result = 1.0;
    for (int i = 1; i <= degree; i++) {
        result += pow(x, i);
    }
    return result;
}

double computeMoment(int k, double a, double b, int steps = 50000) {
    double h = (b - a) / steps;
    double sum = rho(a) * pow(a, k) + rho(b) * pow(b, k);

    for (int i = 1; i < steps; i++) {
        double x = a + i * h;
        double coef = (i % 2 == 0) ? 2.0 : 4.0;
        sum += coef * rho(x) * pow(x, k);
    }

    return sum * h / 3.0;
}

double exactIntegralPolynomial(const vector<double>& coeffs, double a, double b) {
    int n = 1000000;
    double h = (b - a) / n;
    double sum = 0.0;

    for (int i = 0; i <= n; i++) {
        double x = a + i * h;
        double poly_value = 0.0;
        for (size_t j = 0; j < coeffs.size(); j++) {
            poly_value += coeffs[j] * pow(x, j);
        }
        double weight = (i == 0 || i == n) ? 0.5 : 1.0;
        sum += weight * poly_value / (x + 0.1);
    }

    return sum * h;
}

double exactIntegralF0(double a, double b) {
    return 3.5 * (log(b + 0.1) - log(a + 0.1));
}

double exactIntegralF1(double a, double b) {
    return (2.0 * b + 1.3 * log(b + 0.1)) - (2.0 * a + 1.3 * log(a + 0.1));
}

double exactIntegralF2(double a, double b) {
    auto primitive = [](double x) {
        return x*x/2.0 - 3.1*x + 2.31*log(x + 0.1);
    };
    return primitive(b) - primitive(a);
}

double exactIntegralF3(double a, double b) {
    auto primitive = [](double x) {
        return x*x*x/3.0 - 2.1*x*x/2.0 + 4.21*x - 1.421*log(x + 0.1);
    };
    return primitive(b) - primitive(a);
}

double computeExactIntegral(double (*f)(double), double a, double b, int steps = 1000000) {
    double h = (b - a) / steps;
    double sum = 0.0;

    for (int i = 0; i <= steps; i++) {
        double x = a + i * h;
        double weight = (i == 0 || i == steps) ? 0.5 : 1.0;
        sum += weight * rho(x) * f(x);
    }

    return sum * h;
}

vector<double> solveSystem(vector<vector<double>> A, vector<double> b) {
    int n = A.size();

    for (int i = 0; i < n; i++) {
        int maxRow = i;
        for (int k = i + 1; k < n; k++) {
            if (abs(A[k][i]) > abs(A[maxRow][i])) {
                maxRow = k;
            }
        }
        swap(A[i], A[maxRow]);
        swap(b[i], b[maxRow]);

        for (int k = i + 1; k < n; k++) {
            double c = A[k][i] / A[i][i];
            for (int j = i; j < n; j++) {
                A[k][j] -= c * A[i][j];
            }
            b[k] -= c * b[i];
        }
    }

    vector<double> x(n);
    for (int i = n - 1; i >= 0; i--) {
        x[i] = b[i];
        for (int j = i + 1; j < n; j++) {
            x[i] -= A[i][j] * x[j];
        }
        x[i] /= A[i][i];
    }

    return x;
}

bool checkSingularity(double a, double b) {
    const double singularity = -0.1;
    const double epsilon = 1e-10;

    if (a <= singularity && singularity <= b) {
        return false;
    }

    if (abs(a - singularity) < epsilon || abs(b - singularity) < epsilon) {
        return false;
    }

    return true;
}

bool validateNodes(const vector<double>& nodes, double a, double b) {
    const double singularity = -0.1;
    const double epsilon = 1e-6;

    for (size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i] < a || nodes[i] > b) {
            cout << "Ошибка: узел x" << i+1 << " = " << nodes[i]
                 << " находится вне промежутка [" << a << ", " << b << "]\n";
            return false;
        }

        if (abs(nodes[i] - singularity) < epsilon) {
            cout << "Ошибка: узел x" << i+1 << " = " << nodes[i]
                 << " слишком близок к особенности x = " << singularity << "\n";
            return false;
        }

        for (size_t j = i + 1; j < nodes.size(); j++) {
            if (abs(nodes[i] - nodes[j]) < epsilon) {
                cout << "Ошибка: узлы x" << i+1 << " и x" << j+1
                     << " слишком близки или совпадают\n";
                return false;
            }
        }
    }

    return true;
}

template<typename T>
bool safeInput(T& value) {
    if (cin >> value) {
        return true;
    } else {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
}

int main() {
    double (*functions[])(double) = {f0, f1, f2, f3, f4, f5};
    string funcNames[] = {
        "f0(x) = 3.5",
        "f1(x) = 2x + 1.5",
        "f2(x) = x² - 3x + 2",
        "f3(x) = x³ - 2x² + 4x - 1",
        "f4(x) = x⁴ + 0.5x³ - x² + 2x + 3",
        "f5(x) = sin(x)"
    };

    char continue_choice;
    bool firstRun = true;
    int precision = 10;
    double a, b;
    int N;
    vector<double> nodes;
    int funcChoice;

    do {
        bool needFullInput = firstRun;

        if (!firstRun) {
            cout << "\n1. Изменить все данные\n";
            cout << "2. Изменить только узлы (количество и значения)\n";
            cout << "Ваш выбор: ";

            int changeChoice;
            if (!safeInput(changeChoice) || (changeChoice != 1 && changeChoice != 2)) {
                cout << "Неверный выбор!\n";
                continue;
            }

            needFullInput = (changeChoice == 1);
        }

        if (needFullInput) {
            cout << "\n=== Построение ИКФ ===\n";
            cout << "Вариант 9: rho(x) = 1/(x+0.1)\n\n";

            cout << "Введите количество знаков после запятой: ";
            if (!safeInput(precision) || precision < 0 || precision > 20) {
                cout << "Некорректный ввод. Используется значение по умолчанию: 10\n";
                precision = 10;
            }

            cout << fixed << setprecision(precision);

            cout << "\nВведите концы промежутка интегрирования [a, b]:\n";
            cout << "a = ";
            if (!safeInput(a)) {
                cout << "Ошибка: некорректный ввод!\n";
                continue;
            }

            cout << "b = ";
            if (!safeInput(b)) {
                cout << "Ошибка: некорректный ввод!\n";
                continue;
            }

            if (a >= b) {
                cout << "Ошибка: a должно быть меньше b!\n";
                continue;
            }

            if (!checkSingularity(a, b)) {
                cout << "Ошибка: промежуток [" << a << ", " << b
                     << "] содержит особенность весовой функции rho(x) = 1/(x+0.1) при x = -0.1\n";
                cout << "Пожалуйста, выберите другой промежуток.\n";
                continue;
            }
        } else {
            cout << fixed << setprecision(precision);
            cout << "\nИспользуются промежуток [" << a << ", " << b << "] и точность " << precision << " знаков\n";
        }

        cout << "\nВведите количество узлов N: ";
        if (!safeInput(N) || N <= 0) {
            cout << "N должно быть положительным целым числом!\n";
            continue;
        }

        nodes.resize(N);
        cout << "\nВведите " << N << " узлов:\n";
        bool inputError = false;
        for (int i = 0; i < N; i++) {
            cout << "x" << i+1 << " = ";
            if (!safeInput(nodes[i])) {
                cout << "Ошибка: некорректный ввод!\n";
                inputError = true;
                break;
            }
        }

        if (inputError) {
            continue;
        }

        if (!validateNodes(nodes, a, b)) {
            continue;
        }

        if (needFullInput) {
            cout << "\nВыберите функцию для интегрирования:\n";
            for (int i = 0; i < 4; i++) {
                cout << i << ". " << funcNames[i] << "\n";
            }
            cout << "4. f4(x) = многочлен степени " << N-1 << " (1 + x + x² + ... + x^" << N-1 << ")\n";
            cout << "5. " << funcNames[5] << "\n";

            cout << "Ваш выбор: ";
            if (!safeInput(funcChoice) || funcChoice < 0 || funcChoice > 5) {
                cout << "Неверный выбор!\n";
                continue;
            }
        } else {
            if (funcChoice == 4) {
                cout << "\nИспользуется функция: многочлен степени " << N-1 << "\n";
            } else {
                cout << "\nИспользуется функция: " << funcNames[funcChoice] << "\n";
            }
        }

        firstRun = false;

        cout << "\nВычисление моментов...\n";
        vector<double> moments(N);
        for (int k = 0; k < N; k++) {
            moments[k] = computeMoment(k, a, b);
            cout << "μ" << k << " = " << moments[k] << "\n";
        }

        vector<vector<double>> matrix(N, vector<double>(N));
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matrix[i][j] = pow(nodes[j], i);
            }
        }

        vector<double> coeffs = solveSystem(matrix, moments);

        cout << "\nКоэффициенты ИКФ:\n";
        for (int i = 0; i < N; i++) {
            cout << "A" << i+1 << " = " << coeffs[i] << " (узел x" << i+1 << " = " << nodes[i] << ")\n";
        }

        if (funcChoice != 4) {
            cout << "\nПроверка точности ИКФ на многочлене степени " << N-1 << ":\n";
            cout << "p(x) = 1 + x + x² + ... + x^" << N-1 << "\n";

            auto polyN = [N](double x) { return polynomialDegreeN(x, N-1); };

            vector<double> polyCoeffs(N, 1.0);
            double exact_poly = exactIntegralPolynomial(polyCoeffs, a, b);

            double approx_poly = 0.0;
            for (int i = 0; i < N; i++) {
                approx_poly += coeffs[i] * polyN(nodes[i]);
            }

            cout << "Точное значение: " << exact_poly << "\n";
            cout << "Приближенное значение (ИКФ): " << approx_poly << "\n";
            cout << "Абсолютная разность: " << abs(exact_poly - approx_poly) << "\n";
            if (abs(exact_poly) > 1e-10) {
                cout << "Относительная погрешность: " << abs(exact_poly - approx_poly) / abs(exact_poly) * 100 << "%\n";
            }
        }

        if (funcChoice == 4) {
            cout << "\n=== Вычисление интеграла для f4(x) = многочлен степени " << N-1 << " ===\n";
        } else {
            cout << "\n=== Вычисление интеграла для " << funcNames[funcChoice] << " ===\n";
        }

        double exactValue, approxValue = 0.0;

        if (funcChoice == 4) {
            auto f4Dynamic = [N](double x) { return polynomialDegreeN(x, N-1); };

            vector<double> polyCoeffs(N, 1.0);
            exactValue = exactIntegralPolynomial(polyCoeffs, a, b);

            for (int i = 0; i < N; i++) {
                approxValue += coeffs[i] * f4Dynamic(nodes[i]);
            }
        } else {
            switch(funcChoice) {
                case 0:
                    exactValue = exactIntegralF0(a, b);
                    break;
                case 1:
                    exactValue = exactIntegralF1(a, b);
                    break;
                case 2:
                    exactValue = exactIntegralF2(a, b);
                    break;
                case 3:
                    exactValue = exactIntegralF3(a, b);
                    break;
                case 5:
                    exactValue = computeExactIntegral(functions[funcChoice], a, b);
                    break;
                default:
                    exactValue = computeExactIntegral(functions[funcChoice], a, b);
            }

            for (int i = 0; i < N; i++) {
                approxValue += coeffs[i] * functions[funcChoice](nodes[i]);
            }
        }

        cout << "Точное значение интеграла: " << exactValue << "\n";
        cout << "Приближенное значение: " << approxValue << "\n";

        double absError = abs(exactValue - approxValue);
        double relError = (exactValue != 0) ? absError / abs(exactValue) : 0;

        cout << "Абсолютная погрешность: " << absError << "\n";
        cout << "Относительная погрешность: " << relError * 100 << "%\n";

        cout << "\nContinue? (y/n): ";
        cin >> continue_choice;

    } while (continue_choice == 'y' || continue_choice == 'Y');

    return 0;
}