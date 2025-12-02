#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <string>
#include <functional>

using namespace std;

// Структура для хранения результатов
struct Solution {
    vector<double> x;
    vector<double> y;
    string method_name;
};

// Базовый класс для задачи Коши
class CauchyProblem {
protected:
    function<double(double, double)> f;
    function<double(double)> exact_solution;
    double x0, y0;
    string equation_name;
    
public:
    CauchyProblem(function<double(double, double)> func, 
                  function<double(double)> exact, 
                  double x0_val, double y0_val,
                  string eq_name)
        : f(func), exact_solution(exact), x0(x0_val), y0(y0_val), equation_name(eq_name) {}
    
    double getX0() const { return x0; }
    double getY0() const { return y0; }
    string getName() const { return equation_name; }
    
    double evaluateF(double x, double y) const { return f(x, y); }
    double getExactValue(double x) const { return exact_solution(x); }
};

// Класс для форматирования вывода
class OutputFormatter {
private:
    int precision;
    bool scientific_notation;
    int column_width;
    
    void calculateColumnWidth() {
        if (scientific_notation) {
            column_width = precision + 12;
        } else {
            column_width = precision + 8;
        }
        if (column_width < 15) column_width = 15;
    }

public:
    OutputFormatter() : precision(6), scientific_notation(false) {
        calculateColumnWidth();
    }

    void configure() {
        cout << "\n========== НАСТРОЙКА ФОРМАТИРОВАНИЯ ВЫВОДА ==========\n";
        cout << "Выберите формат вывода чисел:\n";
        cout << "1. Обычный формат - например: 123.456789\n";
        cout << "2. Экспоненциальный формат - например: 1.234567e+02\n";
        cout << "Ваш выбор: ";

        int choice;
        cin >> choice;
        scientific_notation = (choice == 2);

        if (scientific_notation) {
            cout << "\nВведите точность (количество цифр после запятой):\n";
            cout << "Например: 6 -> 1.234567e+02, 3 -> 1.235e+02\n";
        } else {
            cout << "\nВведите количество знаков после запятой:\n";
            cout << "Например: 6 -> 123.456789, 3 -> 123.457\n";
        }
        cout << "Ваш выбор: ";
        cin >> precision;

        calculateColumnWidth();

        cout << "\nФормат установлен:\n";
        if (scientific_notation) {
            cout << "  Формат: экспоненциальный\n";
            cout << "  Точность: " << precision << " цифр\n";
        } else {
            cout << "  Формат: обычный (fixed)\n";
            cout << "  Знаков после запятой: " << precision << "\n";
        }
        cout << "  Ширина колонок: " << column_width << " символов\n";
    }

    void apply(ostream& os) const {
        if (scientific_notation) {
            os << scientific;
        } else {
            os << fixed;
        }
        os << setprecision(precision);
    }

    int getPrecision() const { return precision; }
    int getColumnWidth() const { return column_width; }
    bool isScientific() const { return scientific_notation; }
};

// Решатель ОДУ
class ODESolver {
private:
    const CauchyProblem& problem;

public:
    ODESolver(const CauchyProblem& prob) : problem(prob) {}

    Solution solveEuler(double h, int N) {
        Solution sol;
        sol.method_name = "Метод Эйлера";
        sol.x.resize(N + 1);
        sol.y.resize(N + 1);

        sol.x[0] = problem.getX0();
        sol.y[0] = problem.getY0();

        for (int k = 0; k < N; k++) {
            sol.x[k + 1] = sol.x[k] + h;
            sol.y[k + 1] = sol.y[k] + h * problem.evaluateF(sol.x[k], sol.y[k]);
        }

        return sol;
    }

    Solution solveEulerI(double h, int N) {
        Solution sol;
        sol.method_name = "Метод Эйлера I (средних прямоугольников)";
        sol.x.resize(N + 1);
        sol.y.resize(N + 1);

        sol.x[0] = problem.getX0();
        sol.y[0] = problem.getY0();

        for (int k = 0; k < N; k++) {
            sol.x[k + 1] = sol.x[k] + h;
            double y_half = sol.y[k] + (h / 2.0) * problem.evaluateF(sol.x[k], sol.y[k]);
            sol.y[k + 1] = sol.y[k] + h * problem.evaluateF(sol.x[k] + h / 2.0, y_half);
        }

        return sol;
    }

    Solution solveEulerII(double h, int N) {
        Solution sol;
        sol.method_name = "Метод Эйлера II (трапеций)";
        sol.x.resize(N + 1);
        sol.y.resize(N + 1);

        sol.x[0] = problem.getX0();
        sol.y[0] = problem.getY0();

        for (int k = 0; k < N; k++) {
            sol.x[k + 1] = sol.x[k] + h;
            double Y_kp1 = sol.y[k] + h * problem.evaluateF(sol.x[k], sol.y[k]);
            sol.y[k + 1] = sol.y[k] + (h / 2.0) *
                (problem.evaluateF(sol.x[k], sol.y[k]) +
                 problem.evaluateF(sol.x[k + 1], Y_kp1));
        }

        return sol;
    }

    Solution solveRK4(double h, int N) {
        Solution sol;
        sol.method_name = "Метод Рунге-Кутты 4-го порядка";
        sol.x.resize(N + 1);
        sol.y.resize(N + 1);

        sol.x[0] = problem.getX0();
        sol.y[0] = problem.getY0();

        for (int k = 0; k < N; k++) {
            sol.x[k + 1] = sol.x[k] + h;

            double k1 = h * problem.evaluateF(sol.x[k], sol.y[k]);
            double k2 = h * problem.evaluateF(sol.x[k] + h / 2.0, sol.y[k] + k1 / 2.0);
            double k3 = h * problem.evaluateF(sol.x[k] + h / 2.0, sol.y[k] + k2 / 2.0);
            double k4 = h * problem.evaluateF(sol.x[k] + h, sol.y[k] + k3);

            sol.y[k + 1] = sol.y[k] + (k1 + 2.0 * k2 + 2.0 * k3 + k4) / 6.0;
        }

        return sol;
    }

    Solution getExactSolution(double h, int N) {
        Solution sol;
        sol.method_name = "Точное решение";
        sol.x.resize(N + 1);
        sol.y.resize(N + 1);

        for (int k = 0; k <= N; k++) {
            sol.x[k] = problem.getX0() + k * h;
            sol.y[k] = problem.getExactValue(sol.x[k]);
        }

        return sol;
    }
};

// Библиотека тестовых задач
class TestProblems {
public:
    static vector<CauchyProblem> getAllProblems() {
        vector<CauchyProblem> problems;

        problems.push_back(CauchyProblem(
            [](double x, double y) { return -y*y + 1; },
            [](double x) { return tanh(x); },
            0.0, 0.0,
            "y' = -y^2 + 1, y(0) = 0"
        ));

        problems.push_back(CauchyProblem(
            [](double x, double y) { return -y + x; },
            [](double x) { return x - 1 + 2*exp(-x); },
            0.0, 1.0,
            "y' = -y + x, y(0) = 1"
        ));

        problems.push_back(CauchyProblem(
            [](double x, double y) { return -2*y + y*y; },
            [](double x) { return 2.0/(1.0 + exp(2*x)); },
            0.0, 1.0,
            "y' = -2y + y^2, y(0) = 1"
        ));

        problems.push_back(CauchyProblem(
            [](double x, double y) { return -y + y*y; },
            [](double x) { return 1.0/(1.0 + exp(x)); },
            0.0, 0.5,
            "y' = -y + y^2, y(0) = 0.5"
        ));

        problems.push_back(CauchyProblem(
            [](double x, double y) { return -y + sin(x); },
            [](double x) { return (sin(x) - cos(x) + 3*exp(-x))/2.0; },
            0.0, 1.0,
            "y' = -y + sin(x), y(0) = 1"
        ));

        problems.push_back(CauchyProblem(
            [](double x, double y) { return -y + exp(-x); },
            [](double x) { return (1.0 + x)*exp(-x); },
            0.0, 1.0,
            "y' = -y + e^(-x), y(0) = 1"
        ));

        problems.push_back(CauchyProblem(
            [](double x, double y) { return -y + cos(x); },
            [](double x) { return (sin(x) + cos(x) + exp(-x))/2.0; },
            0.0, 1.0,
            "y' = -y + cos(x), y(0) = 1"
        ));

        problems.push_back(CauchyProblem(
            [](double x, double y) { return -y*(1.0 + x); },
            [](double x) { return exp(-x - x*x/2.0); },
            0.0, 1.0,
            "y' = -y(1 + x), y(0) = 1"
        ));

        problems.push_back(CauchyProblem(
            [](double x, double y) { return -y*(2.0 - cos(x)); },
            [](double x) { return exp(-2*x + sin(x)); },
            0.0, 1.0,
            "y' = -y(2 - cos(x)), y(0) = 1"
        ));

        problems.push_back(CauchyProblem(
            [](double x, double y) { return -3*y + y*y; },
            [](double x) { return 3.0/(2.0 + exp(3*x)); },
            0.0, 1.0,
            "y' = -3y + y^2, y(0) = 1"
        ));

        problems.push_back(CauchyProblem(
            [](double x, double y) { return -y*y; },
            [](double x) { return 1.0/(1.0 + x); },
            0.0, 1.0,
            "y' = -y^2, y(0) = 1"
        ));

        problems.push_back(CauchyProblem(
            [](double x, double y) { return -y + y*y; },
            [](double x) { return 1.0/(1.0 + exp(x)); },
            0.0, 0.5,
            "y' = -y + y^2, y(0) = 0.5"
        ));

        problems.push_back(CauchyProblem(
            [](double x, double y) { return -y + 2*y*y + 1; },
            [](double x) { return (1.0 + tanh(x))/2.0; },
            0.0, 0.25,
            "y' = -y + 2y^2 + 1, y(0) = 0.25"
        ));

        problems.push_back(CauchyProblem(
            [](double x, double y) { return -y + x/2.0; },
            [](double x) { return x/2.0 - 0.25 + 1.25*exp(-x); },
            0.0, 1.0,
            "y' = -y + x/2, y(0) = 1"
        ));

        return problems;
    }
};

class RungeRombergRefinement {
public:
    static double refine(double J_h, double J_h2, int order) {
        return J_h2 + (J_h2 - J_h) / (pow(2, order) - 1);
    }
};

// Главный класс программы
class Application {
private:
    OutputFormatter formatter;
    vector<CauchyProblem> problems;

    void printHeader() {
        cout << "\n";
        cout << "╔════════════════════════════════════════════════════════════════╗\n";
        cout << "║   ЧИСЛЕННОЕ РЕШЕНИЕ ЗАДАЧИ КОШИ ДЛЯ ОДУ ПЕРВОГО ПОРЯДКА      ║\n";
        cout << "║              Методы Эйлера и Рунге-Кутты                      ║\n";
        cout << "╚════════════════════════════════════════════════════════════════╝\n";
    }

    int selectProblem() {
        cout << "\n========== ВЫБОР ТЕСТОВОЙ ЗАДАЧИ ==========\n";
        for (size_t i = 0; i < problems.size(); i++) {
            cout << setw(2) << (i + 1) << ". " << problems[i].getName() << "\n";
        }
        cout << " 0. Выход из программы\n";
        cout << "\nВыберите номер варианта (0-" << problems.size() << "): ";

        int choice;
        cin >> choice;
        return choice;
    }

    void printTable(const Solution& exact, const vector<Solution>& numerical, 
                   const OutputFormatter& fmt) {
        cout << "\n========== ТАБЛИЦА РЕШЕНИЙ ==========\n";

        int cw = fmt.getColumnWidth();

        cout << right;
        cout << setw(cw) << "x";
        cout << setw(cw) << "Точное";
        cout << setw(cw) << "Эйлер";
        cout << setw(cw) << "Эйлер I";
        cout << setw(cw) << "Эйлер II";
        cout << setw(cw) << "РК4";
        cout << "\n";

        int total_width = cw * 6;
        cout << string(total_width, '-') << "\n";

        fmt.apply(cout);
        cout << right;

        for (size_t i = 0; i < exact.x.size(); i++) {
            cout << setw(cw) << exact.x[i];
            cout << setw(cw) << exact.y[i];
            cout << setw(cw) << numerical[0].y[i];
            cout << setw(cw) << numerical[1].y[i];
            cout << setw(cw) << numerical[2].y[i];
            cout << setw(cw) << numerical[3].y[i];
            cout << "\n";
        }

        cout << defaultfloat << left;
    }

    void printErrorSummary(const Solution& exact, const vector<Solution>& numerical,
                          const OutputFormatter& fmt) {
        cout << "\n========== СВОДКА ПОГРЕШНОСТЕЙ (для yN) ==========\n";

        int cw = fmt.getColumnWidth();
        int method_width = 45;

        cout << left << setw(method_width) << "Метод";
        cout << right << setw(cw) << "Значение yN";
        cout << right << setw(cw) << "|yT - yN|";
        cout << "\n";

        int total_width = method_width + cw * 2;
        cout << string(total_width, '-') << "\n";

        double yT = exact.y.back();

        for (const auto& sol : numerical) {
            double yN = sol.y.back();
            double error = abs(yT - yN);

            cout << left << setw(method_width) << sol.method_name;

            fmt.apply(cout);
            cout << right;
            cout << setw(cw) << yN;
            cout << setw(cw) << error;
            cout << "\n";
        }

        cout << defaultfloat << left;
    }

    void performRungeRombergRefinement(const CauchyProblem& problem, double h, int N) {
        cout << "\n========== УТОЧНЕНИЕ ПО РУНГЕ-РОМБЕРГУ ==========\n";

        ODESolver solver(problem);

        auto euler_h = solver.solveEuler(h, N);
        auto eulerI_h = solver.solveEulerI(h, N);
        auto eulerII_h = solver.solveEulerII(h, N);
        auto rk4_h = solver.solveRK4(h, N);

        auto euler_h2 = solver.solveEuler(h/2, 2*N);
        auto eulerI_h2 = solver.solveEulerI(h/2, 2*N);
        auto eulerII_h2 = solver.solveEulerII(h/2, 2*N);
        auto rk4_h2 = solver.solveRK4(h/2, 2*N);

        double yT = problem.getExactValue(problem.getX0() + N*h);

        cout << "\nПараметры: h = " << h << ", N = " << N
             << ", xN = " << (problem.getX0() + N*h) << "\n\n";

        int cw = formatter.getColumnWidth();
        int method_width = 30;

        // Заголовок
        cout << left << setw(method_width) << "Метод";
        cout << right << setw(cw) << "J(h)";
        cout << right << setw(cw) << "|yT - J(h)|";
        cout << right << setw(cw) << "J(h/2)";
        cout << right << setw(cw) << "|yT - J(h/2)|";
        cout << right << setw(cw) << "J_R";
        cout << right << setw(cw) << "|yT - J_R|";
        cout << "\n";

        int total_width = method_width + cw * 6;
        cout << string(total_width, '-') << "\n";

        auto printRow = [&](const string& name, const Solution& sol_h,
                           const Solution& sol_h2, int order) {
            double J_h = sol_h.y.back();
            double J_h2 = sol_h2.y.back();
            double J_R = RungeRombergRefinement::refine(J_h, J_h2, order);

            cout << left << setw(method_width) << name;

            formatter.apply(cout);
            cout << right;
            cout << setw(cw) << J_h;
            cout << setw(cw) << abs(yT - J_h);
            cout << setw(cw) << J_h2;
            cout << setw(cw) << abs(yT - J_h2);
            cout << setw(cw) << J_R;
            cout << setw(cw) << abs(yT - J_R);
            cout << "\n";
        };

        printRow("Эйлер", euler_h, euler_h2, 1);
        printRow("Эйлер I", eulerI_h, eulerI_h2, 2);
        printRow("Эйлер II", eulerII_h, eulerII_h2, 2);
        printRow("РК4", rk4_h, rk4_h2, 4);

        cout << "\nПримечание: порядок методов - Эйлер: r=1, Эйлер I и II: r=2, РК4: r=4\n";

        cout << defaultfloat << left;
    }

public:
    Application() {
        problems = TestProblems::getAllProblems();
    }

    void run() {
        printHeader();
        formatter.configure();

        while (true) {
            int problem_choice = selectProblem();

            if (problem_choice == 0) {
                cout << "\nСпасибо за использование программы!\n";
                break;
            }

            if (problem_choice < 1 || problem_choice > (int)problems.size()) {
                cout << "Неверный выбор! Попробуйте снова.\n";
                continue;
            }

            const CauchyProblem& problem = problems[problem_choice - 1];

            cout << "\n========== ПАРАМЕТРЫ РЕШЕНИЯ ==========\n";
            cout << "Задача: " << problem.getName() << "\n";
            cout << "Начальные условия: x0 = " << problem.getX0() 
                 << ", y0 = " << problem.getY0() << "\n\n";

            double h;
            int N;

            cout << "Введите шаг h: ";
            cin >> h;
            cout << "Введите число шагов N: ";
            cin >> N;

            ODESolver solver(problem);

            auto exact = solver.getExactSolution(h, N);
            vector<Solution> numerical;
            numerical.push_back(solver.solveEuler(h, N));
            numerical.push_back(solver.solveEulerI(h, N));
            numerical.push_back(solver.solveEulerII(h, N));
            numerical.push_back(solver.solveRK4(h, N));

            printTable(exact, numerical, formatter);
            printErrorSummary(exact, numerical, formatter);

            while (true) {
                cout << "\n========== ДАЛЬНЕЙШИЕ ДЕЙСТВИЯ ==========\n";
                cout << "1. Ввести новые параметры h и N\n";
                cout << "2. Выполнить уточнение по Рунге-Ромбергу\n";
                cout << "3. Выбрать другую задачу\n";
                cout << "0. Выход из программы\n";
                cout << "Ваш выбор: ";

                int action;
                cin >> action;

                if (action == 0) {
                    cout << "\nСпасибо за использование программы!\n";
                    return;
                } else if (action == 1) {
                    break;
                } else if (action == 2) {
                    performRungeRombergRefinement(problem, h, N);
                } else if (action == 3) {
                    goto next_problem;
                } else {
                    cout << "Неверный выбор!\n";
                }
            }

            problem_choice--;
            continue;

            next_problem:
            continue;
        }
    }
};

int main() {
    Application app;
    app.run();
    return 0;
}