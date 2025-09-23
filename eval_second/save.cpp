#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <random>

using namespace std;

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
    int m = 0;
    double a = 0.0, b = 0.0;
    int precision = 4;
    
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
        cout << "f(x) = 1 - exp(-x) + x^2" << endl;
        
        // Запрос параметров у пользователя
        cout << "\nВведите число значений в таблице: ";
        int table_size;
        cin >> table_size;
        m = table_size - 1;
        
        cout << "Введите левую границу отрезка: ";
        cin >> a;
        cout << "Введите правую границу отрезка: ";
        cin >> b;
        
        cout << "\nПараметры задачи:" << endl;
        cout << "Отрезок [" << a << ", " << b << "]" << endl;
        cout << "Число значений в таблице: " << (m + 1) << endl;
        
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
        cout << "k\tz_k\t\tf(z_k)" << endl;
        for (int k = 0; k <= m; k++) {
            cout << k << "\t" << fixed << setprecision(precision) << table[k].z 
                 << "\t\t" << table[k].fz << endl;
        }
    }
    
    void sortTableByDistance(double x) {
        for (auto& entry : table) {
            entry.dist_to_x = abs(entry.z - x);
        }
        
        sort(table.begin(), table.end(), 
             [](const TableEntry& a, const TableEntry& b) {
                 return a.dist_to_x < b.dist_to_x;
             });
        
        cout << "\nОтсортированная таблица (по близости к x = " << x << "):" << endl;
        cout << "k\tx_k\t\tf(x_k)\t\t|x_k - x|" << endl;
        for (int k = 0; k <= m; k++) {
            cout << k << "\t" << fixed << setprecision(precision) << table[k].z 
                 << "\t\t" << table[k].fz << "\t\t" << table[k].dist_to_x << endl;
        }
    }
    
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
            double li = 1.0;
            
            for (int j = 0; j <= n; j++) {
                if (i != j) {
                    li *= (x - table[j].z) / (table[i].z - table[j].z);
                }
            }
            
            sum_coefficients += li;
            result += table[i].fz * li;
        }
        
        cout << "Контроль: сумма лагранжевых коэффициентов = " 
             << fixed << setprecision(precision + 2) << sum_coefficients << endl;
        
        return result;
    }
    
    double newtonInterpolation(double x, int n) {
        vector<vector<double>> dd(n + 1, vector<double>(n + 1));
        
        for (int i = 0; i <= n; i++) {
            dd[i][0] = table[i].fz;
        }
        
        for (int j = 1; j <= n; j++) {
            for (int i = 0; i <= n - j; i++) {
                dd[i][j] = (dd[i + 1][j - 1] - dd[i][j - 1]) / 
                          (table[i + j].z - table[i].z);
            }
        }
        
        cout << "\nТаблица разделенных разностей:" << endl;
        cout << "i\tx_i\t\tf[x_i]\t\tf[x_i,x_{i+1}]\tf[x_i,...,x_{i+2}]\t..." << endl;
        for (int i = 0; i <= n; i++) {
            cout << i << "\t" << fixed << setprecision(precision) << table[i].z << "\t";
            for (int j = 0; j <= min(3, n - i); j++) {
                cout << "\t" << setprecision(precision + 1) << dd[i][j];
            }
            cout << endl;
        }
        
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
            
            while (true) {
                cout << "Введите степень интерполяционного многочлена n (n ≤ " << m << "): ";
                cin >> n;
                
                if (n <= m && n >= 0) {
                    break;
                } else {
                    cout << "Введено недопустимое значение n. Введите n ≤ " << m << endl;
                }
            }
            
            sortTableByDistance(x);
            
            // Вычисления по Лагранжу
            cout << "\n=== Метод Лагранжа ===" << endl;
            double pn_lagrange = lagrangeInterpolation(x, n);
            double exact_value = f(x);
            double error_lagrange = abs(exact_value - pn_lagrange);
            
            cout << "P_" << n << "^L(x) = " << fixed << setprecision(precision + 2) << pn_lagrange << endl;
            cout << "Точное значение f(x) = " << exact_value << endl;
            cout << "Абсолютная погрешность |f(x) - P_" << n << "^L(x)| = " 
                 << scientific << setprecision(precision) << error_lagrange << endl;
            
            // Вычисления по Ньютону
            cout << "\n=== Метод Ньютона ===" << endl;
            double pn_newton = newtonInterpolation(x, n);
            double error_newton = abs(exact_value - pn_newton);
            
            cout << "P_" << n << "^N(x) = " << fixed << setprecision(precision + 2) << pn_newton << endl;
            cout << "Абсолютная погрешность |f(x) - P_" << n << "^N(x)| = " 
                 << scientific << setprecision(precision) << error_newton << endl;
            
            // Контроль результатов
            cout << "\n=== Контроль результатов ===" << endl;
            cout << "Разность |P_L(x) - P_N(x)| = " 
                 << scientific << setprecision(precision) << abs(pn_lagrange - pn_newton) << endl;
            
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