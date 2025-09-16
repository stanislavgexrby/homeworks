#include "numeric_methods.hpp"

void print_header() {
    cout << "=========================================" << endl;
    cout << "ЧИСЛЕННЫЕ МЕТОДЫ РЕШЕНИЯ НЕЛИНЕЙНЫХ УРАВНЕНИЙ" << endl;
    cout << "=========================================" << endl;
    cout << endl;
}

void print_function() {
    cout << "Функция: f(x) = 5*sin(2x) - sqrt(1-x)" << endl;
    cout << endl;
}

void print_parameters(double A, double B) {
    cout << "Исходные параметры задачи:" << endl;
    cout << "Функция: f(x) = 5*sin(2x) - sqrt(1-x)" << endl;
    cout << "Интервал: [" << A << ", " << B << "]" << endl;
    cout << endl;
}

void solve_on_interval(const Interval& interval, function<double(double)> f, 
                      function<double(double)> df, double eps) {
    
    cout << "Решение на отрезке [" << interval.a << ", " << interval.b << "]" << endl;
    cout << "=========================================" << endl;
    
    // 1. Метод бисекции
    cout << "\n1. МЕТОД ПОЛОВИННОГО ДЕЛЕНИЯ (БИСЕКЦИИ)" << endl;
    cout << "Начальные приближения: a = " << interval.a << ", b = " << interval.b << endl;
    
    SolutionResult bisection = bisection_method(interval, f, eps);
    
    cout << "Количество шагов: " << bisection.iterations << endl;
    cout << "Приближенное решение: x = " << fixed << setprecision(10) << bisection.root << endl;
    cout << "Длина последнего отрезка: " << bisection.last_diff << endl;
    cout << "Абсолютная величина невязки: |f(x)| = " << bisection.residual << endl;
    
    // 2. Метод Ньютона
    cout << "\n2. МЕТОД НЬЮТОНА (КАСАТЕЛЬНЫХ)" << endl;
    cout << "Введите начальное приближение x0: ";
    double x0_newton;
    cin >> x0_newton;
    
    SolutionResult newton = newton_method(interval, f, df, x0_newton, eps);
    
    cout << "Начальное приближение: x0 = " << x0_newton << endl;
    cout << "Количество шагов: " << newton.iterations << endl;
    cout << "Приближенное решение: x = " << fixed << setprecision(10) << newton.root << endl;
    cout << "|x_m - x_{m-1}|: " << newton.last_diff << endl;
    cout << "Абсолютная величина невязки: |f(x)| = " << newton.residual << endl;
    
    // 3. Модифицированный метод Ньютона
    cout << "\n3. МОДИФИЦИРОВАННЫЙ МЕТОД НЬЮТОНА" << endl;
    cout << "Введите начальное приближение x0: ";
    double x0_modified;
    cin >> x0_modified;
    
    SolutionResult modified = modified_newton_method(interval, f, df, x0_modified, eps);
    
    cout << "Начальное приближение: x0 = " << x0_modified << endl;
    cout << "Количество шагов: " << modified.iterations << endl;
    cout << "Приближенное решение: x = " << fixed << setprecision(10) << modified.root << endl;
    cout << "|x_m - x_{m-1}|: " << modified.last_diff << endl;
    cout << "Абсолютная величина невязки: |f(x)| = " << modified.residual << endl;
    
    // 4. Метод секущих
    cout << "\n4. МЕТОД СЕКУЩИХ" << endl;
    cout << "Начальные приближения: x0 = " << interval.a << ", x1 = " << interval.b << endl;
    
    SolutionResult secant = secant_method(interval, f, eps);
    
    cout << "Количество шагов: " << secant.iterations << endl;
    cout << "Приближенное решение: x = " << fixed << setprecision(10) << secant.root << endl;
    cout << "|x_m - x_{m-1}|: " << secant.last_diff << endl;
    cout << "Абсолютная величина невязки: |f(x)| = " << secant.residual << endl;
}

void run_solver() {
    char c;
    print_header();
    print_function();
    int N;
    cout << "Введите количество разбиений(например 1000): ";
    cin >> N;
    double A, B, eps;
    cout << "Введите начало отрезка: ";
    cin >> A;
    cout << "Введите конец отрезка: ";
    cin >> B;
    while (A > B) {
        cout << "Введите заново промежуток:\n";
        cout << "Введите начало отрезка: ";
        cin >> A;
        cout << "Введите конец отрезка: ";
        cin >> B;
    }
    cout << "\n\n";
    print_parameters(A, B);
    cout << "ОТДЕЛЕНИЕ КОРНЕЙ" << endl;
    cout << "=========================================" << endl;
    auto intervals = find_sign_change_intervals(A, B, test_function, N);
    cout << "Найдено отрезков перемены знака: " << intervals.size() << endl;
    if (intervals.empty()) {
        cout << "Корни не найдены на заданном интервале!" << endl;
        return;
    }
    cout << "\nОтрезки перемены знака:" << endl;
    for (size_t i = 0; i < intervals.size(); i++) {
        cout << i + 1 << ". [" << intervals[i].a << ", " << intervals[i].b << "]" << endl;
    }
    cout << "\nУТОЧНЕНИЕ КОРНЕЙ" << endl;
    cout << "=========================================" << endl;

    while (true) {
        cout << "Выберите номер отрезка для уточнения (1-" << intervals.size() << ") или введите -1, чтобы выйти: ";
        int choice;
        cin >> choice;
        if (choice > (int)intervals.size()) {
            cout << "Неверный номер отрезка!" << endl;
        } else if (choice == -1) {
            return;
        } else {
            std::cout << "Введите приближение(например 0.000001): ";
            std::cin >> eps;
            solve_on_interval(intervals[choice - 1], test_function, test_derivative, eps);
            cout << "\n=========================================" << endl;
            cout << "РЕШЕНИЕ ЗАВЕРШЕНО" << endl;
            cout << "=========================================" << endl;
        }
    }
}

int main() {
    run_solver();
    return 0;
}