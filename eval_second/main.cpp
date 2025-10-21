#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <random>
#include <string>

using namespace std;

// f(x) = 1 - exp(-x) + x^2
double f(double x) {
    return 1.0 - exp(-x) + x * x;
}

struct TableEntry {
    double z;  // аргумент
    double fz; // значение функции
    double dist_to_x; // расстояние до точки интерполирования
};

class Interpolator {
private:
    vector<TableEntry> table;
    int m = 0; // число значений в таблице
    double a = 0.0, b = 0.0; // границы отрезка
    int precision = 4; // точность вывода по умолчанию

    // Вспомогательная функция для вычисления ширины столбца
    int getColumnWidth(const string& header, bool isNumeric = true) {
        if (isNumeric) {
            return max((int)header.length() + 2, precision + 12);
        } else {
            return (int)header.length() + 2;
        }
    }

public:
    Interpolator() {}

    void setPrecision() {
        cout << "Введите количество знаков после запятой для вывода: ";
        cin >> precision;
        if (precision < 1) precision = 1;
        if (precision > 15) precision = 15;
    }

    void createTable() {
        cout << "=== Задача алгебраического интерполирования ===" << endl;
        cout << "Вариант 9" << endl;
        cout << "f(x) = 1 - exp(-x) + x^2" << endl;

        // Запрос параметров у пользователя
        cout << "\nВведите число значений в таблице: ";
        int table_size;
        cin >> table_size;
        m = table_size - 1;

        cout << "Введите левую границу отрезка a: ";
        cin >> a;
        cout << "Введите правую границу отрезка b: ";
        cin >> b;

        cout << "\nПараметры задачи:" << endl;
        cout << "Отрезок [" << a << ", " << b << "]" << endl;
        cout << "Число значений в таблице: " << (m + 1) << endl;

        // Создаем равноотстоящие узлы
        double h = (b - a) / m;
        table.clear();
        table.reserve(m + 1);

        for (int k = 0; k <= m; k++) {
            double z = a + k * h;
            TableEntry entry;
            entry.z = z;
            entry.fz = f(z);
            entry.dist_to_x = 0.0;
            table.push_back(entry);
        }

        cout << "\nИсходная таблица значений функции:" << endl;

        int w1 = 4;
        int w2 = getColumnWidth("z_k");
        int w3 = getColumnWidth("f(z_k)");

        cout << left << setw(w1) << "k"
             << right << setw(w2) << "z_k"
             << right << setw(w3) << "f(z_k)" << endl;
        cout << string(w1 + w2 + w3, '-') << endl;

        for (int k = 0; k <= m; k++) {
            cout << left << setw(w1) << k
                 << right << setw(w2) << fixed << setprecision(precision) << table[k].z
                 << right << setw(w3) << fixed << setprecision(precision) << table[k].fz << endl;
        }
    }

    void sortTableByDistance(double x) {
        // Вычисляем расстояния до точки x
        for (auto& entry : table) {
            entry.dist_to_x = abs(entry.z - x);
        }

        // Сортируем по возрастанию расстояния
        sort(table.begin(), table.end(), 
             [](const TableEntry& a, const TableEntry& b) {
                 return a.dist_to_x < b.dist_to_x;
             });

        cout << "\nОтсортированная таблица (по близости к x = " << fixed << setprecision(precision) << x << "):" << endl;

        int w1 = 4;
        int w2 = getColumnWidth("x_k");
        int w3 = getColumnWidth("f(x_k)");
        int w4 = getColumnWidth("|x_k - x|");

        cout << left << setw(w1) << "k"
             << right << setw(w2) << "x_k"
             << right << setw(w3) << "f(x_k)"
             << right << setw(w4) << "|x_k - x|" << endl;
        cout << string(w1 + w2 + w3 + w4, '-') << endl;

        for (int k = 0; k <= m; k++) {
            cout << left << setw(w1) << k
                 << right << setw(w2) << fixed << setprecision(precision) << table[k].z
                 << right << setw(w3) << fixed << setprecision(precision) << table[k].fz
                 << right << setw(w4) << fixed << setprecision(precision) << table[k].dist_to_x << endl;
        }
    }

    // Интерполяция Лагранжа
    double lagrangeInterpolation(double x, int n) {
        cout << "\nПостроение многочлена по узлам: ";
        for (int i = 0; i <= n; i++) {
            cout << "x_" << i << "=" << table[i].z;
            if (i < n) cout << ", ";
        }
        cout << endl;

        double result = 0.0;
        double sum_coefficients = 0.0;

        for (int i = 0; i <= n; i++) {
            double li = 1.0; // базисная функция Лагранжа

            for (int j = 0; j <= n; j++) {
                if (i != j) {
                    li *= (x - table[j].z) / (table[i].z - table[j].z);
                }
            }

            sum_coefficients += li;
            result += table[i].fz * li;
        }

        cout << left << setw(45) << "Контроль: сумма лагранжевых коэффициентов ="
             << right << setw(precision + 10) << fixed << setprecision(precision + 2) << sum_coefficients << endl;

        return result;
    }

    // Интерполяция Ньютона с разделенными разностями
    double newtonInterpolation(double x, int n) {
        // Создаем таблицу разделенных разностей
        vector<vector<double>> dd(n + 1, vector<double>(n + 1));

        // Заполняем нулевой столбец (значения функции)
        for (int i = 0; i <= n; i++) {
            dd[i][0] = table[i].fz;
        }

        // Вычисляем разделенные разности
        for (int j = 1; j <= n; j++) {
            for (int i = 0; i <= n - j; i++) {
                dd[i][j] = (dd[i + 1][j - 1] - dd[i][j - 1]) / 
                          (table[i + j].z - table[i].z);
            }
        }

        cout << "\nТаблица разделенных разностей:" << endl;

        int w1 = 4;
        int w2 = getColumnWidth("x_i");
        int w3 = getColumnWidth("f[x_i]");
        int w4 = getColumnWidth("f[x_i,x_{i+1}]");

        cout << left << setw(w1) << "i"
             << right << setw(w2) << "x_i"
             << right << setw(w3) << "f[x_i]";

        if (n >= 1) cout << right << setw(w4) << "f[x_i,x_{i+1}]";
        if (n >= 2) cout << right << setw(w4) << "f[x_i,...,x_{i+2}]";
        if (n >= 3) cout << right << setw(w4) << "...";
        cout << endl;

        int total_width = w1 + w2 + w3;
        if (n >= 1) total_width += w4;
        if (n >= 2) total_width += w4;
        if (n >= 3) total_width += w4;
        cout << string(total_width, '-') << endl;

        for (int i = 0; i <= n; i++) {
            cout << left << setw(w1) << i
                 << right << setw(w2) << fixed << setprecision(precision) << table[i].z
                 << right << setw(w3) << fixed << setprecision(precision + 1) << dd[i][0];

            for (int j = 1; j <= min(3, n - i); j++) {
                cout << right << setw(w4) << fixed << setprecision(precision + 1) << dd[i][j];
            }
            cout << endl;
        }

        // Вычисляем значение многочлена Ньютона
        double result = dd[0][0];
        for (int k = 1; k <= n; k++) {
            double term = dd[0][k];
            for (int j = 0; j < k; j++) {
                term *= (x - table[j].z);
            }
            result += term;
        }

        return result;
    }

    void solve() {
        setPrecision();
        createTable();

        while (true) {
            double x;
            int n;

            cout << "\n=== Решение задачи интерполирования ===" << endl;
            cout << "Введите точку интерполирования x: ";
            cin >> x;

            // Запрос степени многочлена с проверкой
            while (true) {
                cout << "Введите степень интерполяционного многочлена n (n ≤ " << m << "): ";
                cin >> n;

                if (n <= m && n >= 0) {
                    break;
                } else {
                    cout << "Введено недопустимое значение n. Введите n ≤ " << m << endl;
                }
            }
            
            // Сортируем таблицу по близости к x
            sortTableByDistance(x);

            // Вычисления по Лагранжу
            cout << "\n=== Метод Лагранжа ===" << endl;
            double pn_lagrange = lagrangeInterpolation(x, n);
            double exact_value = f(x);
            double error_lagrange = abs(exact_value - pn_lagrange);

            cout << left << setw(35) << ("P_" + to_string(n) + "^L(x) =")
                 << right << setw(precision + 12) << fixed << setprecision(precision + 2) << pn_lagrange << endl;
            cout << left << setw(35) << "Точное значение f(x) ="
                 << right << setw(precision + 12) << fixed << setprecision(precision + 2) << exact_value << endl;
            cout << left << setw(35) << ("Погрешность |f(x) - P_" + to_string(n) + "^L(x)| =")
                 << right << setw(precision + 12) << scientific << setprecision(precision) << error_lagrange << endl;

            // Вычисления по Ньютону
            cout << "\n=== Метод Ньютона ===" << endl;
            double pn_newton = newtonInterpolation(x, n);
            double error_newton = abs(exact_value - pn_newton);

            cout << left << setw(35) << ("P_" + to_string(n) + "^N(x) =")
                 << right << setw(precision + 12) << fixed << setprecision(precision + 2) << pn_newton << endl;
            cout << left << setw(35) << ("Погрешность |f(x) - P_" + to_string(n) + "^N(x)| =")
                 << right << setw(precision + 12) << scientific << setprecision(precision) << error_newton << endl;

            // Контроль результатов
            cout << "\n=== Контроль результатов ===" << endl;
            cout << left << setw(35) << "Разность |P_L(x) - P_N(x)| ="
                 << right << setw(precision + 12) << scientific << setprecision(precision) << abs(pn_lagrange - pn_newton) << endl;

            // Продолжение работы
            char choice;
            cout << "\nВвести новые значения x и n? (y/n): ";
            cin >> choice;
            if (choice != 'y' && choice != 'Y') {
                break;
            }
        }

        cout << "Программа завершена." << endl;
    }
};

int main() {
    Interpolator interpolator;
    interpolator.solve();
    return 0;
}