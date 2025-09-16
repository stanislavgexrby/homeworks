#include "numeric_methods.hpp"
#include <array>
#include <iomanip>

namespace PhysicsConstants {
    constexpr double PI = M_PI;
    constexpr double WATER_DENSITY = 1000.0; // кг/м³
    constexpr double SPHERE_VOLUME_COEFFICIENT = (4.0 / 3.0) * PI;
}

struct Material {
    std::string name;
    double density; // кг/м³
};

double calculate_sphere_volume(double radius) {
    return PhysicsConstants::SPHERE_VOLUME_COEFFICIENT * std::pow(radius, 3);
}

double calculate_immersion_depth(double radius, double sphere_density) {
    // Объем погруженной части шара
    double submerged_volume = (sphere_density * calculate_sphere_volume(radius)) / PhysicsConstants::WATER_DENSITY;
    
    // Функция для решения: π*h²*(3r - h)/3 - V_погруженная = 0
    MathFunction depth_equation = [radius, submerged_volume](double h) {
        return (PhysicsConstants::PI * h * h * (3.0 * radius - h)) / 3.0 - submerged_volume;
    };
    
    // Ищем интервалы смены знака от 0 до 2*radius (шар не может быть погружен глубже своего диаметра)
    Interval search_range(0.0, 2.0 * radius);
    auto intervals = find_sign_change_intervals(search_range, depth_equation, 1000);
    
    if (intervals.empty()) {
        throw std::runtime_error("No solution found for immersion depth");
    }
    
    // Используем метод бисекции как самый надежный
    double epsilon = 1e-9;
    return solve_equation(intervals[0], epsilon, depth_equation, nullptr, NumericalMethod::BISECTION);
}

void print_materials_table(double radius = 0.62) {
    std::array<Material, 11> materials = {{
        {"Пробка        ", 250},
        {"Бамбук        ", 400},
        {"Сосна         ", 500},
        {"Кедр          ", 550},
        {"Дуб           ", 700},
        {"Бук           ", 750},
        {"Красное дерево", 800},
        {"Тиковое дерево", 850},
        {"Парафин       ", 900},
        {"Лед/Полиэтилен", 920},
        {"Пчелиный воск ", 950}
    }};

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "\n=== IMMERSION DEPTH CALCULATION ===\n";
    std::cout << "Sphere radius: " << radius << " m\n";
    std::cout << "Water density: " << PhysicsConstants::WATER_DENSITY << " kg/m³\n\n";
    
    std::cout << std::left << std::setw(18) << "Material" 
              << std::setw(15) << "Density (kg/m³)" 
              << std::setw(15) << "Depth (m)" 
              << std::setw(20) << "Depth/Diameter (%)" << "\n";
    std::cout << std::string(68, '-') << "\n";

    for (const auto& material : materials) {
        try {
            double depth = calculate_immersion_depth(radius, material.density);
            double depth_percentage = (depth / (2.0 * radius)) * 100.0;
            
            std::cout << std::left << std::setw(18) << material.name
                      << std::setw(15) << material.density
                      << std::setw(15) << depth
                      << std::setw(20) << depth_percentage << "\n";
        } catch (const std::exception& e) {
            std::cout << std::left << std::setw(18) << material.name
                      << std::setw(15) << material.density
                      << "Error: " << e.what() << "\n";
        }
    }
}

void solve_custom_sphere() {
    std::cout << "\n=== CUSTOM SPHERE CALCULATION ===\n";
    
    double radius, density;
    std::cout << "Enter sphere radius (m): ";
    std::cin >> radius;
    
    if (radius <= 0) {
        throw std::runtime_error("Radius must be positive");
    }
    
    std::cout << "Enter sphere density (kg/m³): ";
    std::cin >> density;
    
    if (density <= 0) {
        throw std::runtime_error("Density must be positive");
    }
    
    if (density >= PhysicsConstants::WATER_DENSITY) {
        std::cout << "Warning: Sphere density is greater than or equal to water density.\n";
        std::cout << "The sphere will sink completely.\n";
        std::cout << "Immersion depth: " << 2.0 * radius << " m (full diameter)\n";
        return;
    }
    
    try {
        double depth = calculate_immersion_depth(radius, density);
        double percentage = (depth / (2.0 * radius)) * 100.0;
        
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "\nResults:\n";
        std::cout << "Sphere radius: " << radius << " m\n";
        std::cout << "Sphere density: " << density << " kg/m³\n";
        std::cout << "Immersion depth: " << depth << " m\n";
        std::cout << "Immersion percentage: " << percentage << "%\n";
        
        // Проверка результата
        double sphere_volume = calculate_sphere_volume(radius);
        double submerged_volume = (PhysicsConstants::PI * depth * depth * (3.0 * radius - depth)) / 3.0;
        double buoyant_force = PhysicsConstants::WATER_DENSITY * submerged_volume;
        double weight = density * sphere_volume;
        
        std::cout << "\nVerification:\n";
        std::cout << "Sphere weight: " << weight << " kg\n";
        std::cout << "Buoyant force: " << buoyant_force << " kg\n";
        std::cout << "Difference: " << std::abs(weight - buoyant_force) << " kg\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error calculating immersion depth: " << e.what() << "\n";
    }
}

int main() {
    try {
        // Показываем таблицу для стандартного радиуса
        print_materials_table();
        
        // Позволяем пользователю ввести свои параметры
        solve_custom_sphere();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}