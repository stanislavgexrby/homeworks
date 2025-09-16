#include "numeric_methods.hpp"

const double PI = 3.14159265359;
const double WATER_DENSITY = 1000.0; // кг/м³

struct Material {
    string name;
    double density;
};

Material materials[] = {
    {"Пробка", 250},
    {"Бамбук", 400},
    {"Сосна", 500},
    {"Кедр", 550},
    {"Дуб", 700},
    {"Бук", 750},
    {"Красное дерево", 800},
    {"Тиковое дерево", 850},
    {"Парафин", 900},
    {"Лёд/Полиэтилен", 920},
    {"Пчелиный воск", 950}
};

double sphere_volume(double r) {
    return (4.0 / 3.0) * PI * pow(r, 3);
}

// Функция для нахождения глубины погружения
// Уравнение: π*h²*(3r - h)/3 = ρ_шара * V_шара / ρ_воды
function<double(double)> create_depth_equation(double r, double sphere_density) {
    double target_volume = sphere_density * sphere_volume(r) / WATER_DENSITY;

    return [r, target_volume](double h) -> double {
        return PI * h * h * (3.0 * r - h) / 3.0 - target_volume;
    };
}

double solve_depth(double r, double sphere_density) {
    if (sphere_density >= WATER_DENSITY) {
        return 2.0 * r;
    }

    auto depth_eq = create_depth_equation(r, sphere_density);
    Interval search_interval(0.0, 2.0 * r);

    double eps = 1e-9;
    SolutionResult result = bisection_method(search_interval, depth_eq, eps);

    return result.root;
}

void print_table(double r) {
    cout << "\n=========================================" << endl;
    cout << "ЗАДАЧА О ПОГРУЖЕНИИ ШАРА" << endl;
    cout << "=========================================" << endl;
    cout << "Радиус шара: " << r << " м" << endl;
    cout << "Плотность воды: " << WATER_DENSITY << " кг/м³" << endl;
    cout << endl;

    cout << left << setw(20) << "Материал"
         << setw(15) << "Плотность"
         << setw(15) << "Глубина"
         << setw(15) << "%" << endl;
    cout << string(65, '-') << endl;

    for (int i = 0; i < 11; i++) {
        try {
            double depth = solve_depth(r, materials[i].density);
            double percentage = (depth / (2.0 * r)) * 100.0;

            cout << left << setw(20) << materials[i].name
                 << setw(15) << materials[i].density
                 << setw(15) << fixed << setprecision(6) << depth
                 << setw(15) << fixed << setprecision(2) << percentage << endl;

        } catch (const exception& e) {
            cout << left << setw(20) << materials[i].name
                 << "Ошибка вычисления" << endl;
        }
    }
}

void solve_custom() {
    cout << "\n=========================================" << endl;
    cout << "РАСЧЕТ ДЛЯ ПРОИЗВОЛЬНЫХ ПАРАМЕТРОВ" << endl;
    cout << "=========================================" << endl;

    double r, density;
    cout << "Введите радиус шара (м): ";
    cin >> r;

    cout << "Введите плотность материала (кг/м³): ";
    cin >> density;

    if (r <= 0 || density <= 0) {
        cout << "Ошибка: параметры должны быть положительными!" << endl;
        return;
    }

    try {
        double depth = solve_depth(r, density);
        double percentage = (depth / (2.0 * r)) * 100.0;

        cout << "\nРезультат:" << endl;
        cout << "Глубина погружения: " << fixed << setprecision(6) << depth << " м" << endl;
        cout << "Процент погружения: " << fixed << setprecision(2) << percentage << "%" << endl;

    } catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
    }
}

int main() {
    double standard_r = 0.62;
    print_table(standard_r);

    solve_custom();

    return 0;
}