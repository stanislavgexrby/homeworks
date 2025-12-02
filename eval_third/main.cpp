#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <string>
#include <functional>
#include <array>

using Function = std::function<double(double)>;

struct FuncDescr {
  Function f;
  Function deriv1;
  Function deriv2;
  std::string name;
};

const std::array<FuncDescr, 4> FUNCTIONS = {{
  {
    // f1: Polynomial degree 2
    [](double x) { return x * x + 3.0 * x - 2.0; },
    [](double x) { return 2.0 * x + 3.0; },
    [](double x) { return 2.0; },
    "f1(x) = x^2 + 3x - 2 (polynomial degree 2)"
  },
  {
    // f2: Polynomial degree 3
    [](double x) { return 2.0 * x * x * x - x * x + 3.0 * x + 1.0; },
    [](double x) { return 6.0 * x * x - 2.0 * x + 3.0; },
    [](double x) { return 12.0 * x - 2.0; },
    "f2(x) = 2x^3 - x^2 + 3x + 1 (polynomial degree 3)"
  },
  {
    // f3: Fast growing function
    [](double x) { return std::exp(3.0 * x); },
    [](double x) { return 3.0 * std::exp(3.0 * x); },
    [](double x) { return 9.0 * std::exp(3.0 * x); },
    "f3(x) = e^(3x) (fast growing)"
  },
  {
    // f4: Smooth function
    [](double x) { return std::cos(x) - 0.5 * x * x + 2.0; },
    [](double x) { return -std::sin(x) - x; },
    [](double x) { return -std::cos(x) - 1.0; },
    "f4(x) = cos(x) - 0.5x^2 + 2 (smooth)"
  }
}};

// (4) - initial point, O(h^2)
double formula4(const std::vector<double> &y, double h, int i) {
  return (-3.0 * y[i] + 4.0 * y[i+1] - y[i+2]) / (2.0 * h);
}

// (3) - internal point, O(h^2)
double formula3(const std::vector<double> &y, double h, int i) {
  return (y[i+1] - y[i-1]) / (2.0 * h);
}

// (5) - final point, O(h^2)
double formula5(const std::vector<double> &y, double h, int i) {
  return (3.0 * y[i] - 4.0 * y[i-1] + y[i-2]) / (2.0 * h);
}

// (7) - initial point, O(h^4)
double formula7(const std::vector<double> &y, double h, int i) {
  return (-25.0 * y[i] + 48.0 * y[i+1] - 36.0 * y[i+2] + 16.0 * y[i+3] - 3.0 * y[i+4]) / (12.0 * h);
}

// (8) - second point, O(h^4)
double formula8(const std::vector<double> &y, double h, int i) {
  return (-3.0 * y[i-1] - 10.0 * y[i] + 18.0 * y[i+1] - 6.0 * y[i+2] + y[i+3]) / (12.0 * h);
}

// (9) - internal points, O(h^4)
double formula9(const std::vector<double> &y, double h, int i) {
  return (y[i-2] - 8.0 * y[i-1] + 8.0 * y[i+1] - y[i+2]) / (12.0 * h);
}

// (10) - prev final point, O(h^4)
double formula10(const std::vector<double> &y, double h, int i) {
  return (3.0 * y[i+1] + 10.0 * y[i] - 18.0 * y[i-1] + 6.0 * y[i-2] - y[i-3]) / (12.0 * h);
}

// (11) - final point, O(h^4)
double formula11(const std::vector<double> &y, double h, int i) {
  return (25.0 * y[i] - 48.0 * y[i-1] + 36.0 * y[i-2] - 16.0 * y[i-3] + 3.0 * y[i-4]) / (12.0 * h);
}

// (6) - second derivative for internal points, O(h^2)
double formula6(const std::vector<double> &y, double h, int i) {
  return (y[i+1] - 2.0 * y[i] + y[i-1]) / (h * h);
}

// (12) - second derivative at initial point, O(h^2)
double formula12(const std::vector<double> &y, double h, int i) {
  return (2.0 * y[i] - 5.0 * y[i+1] + 4.0 * y[i+2] - y[i+3]) / (h * h);
}

// (13) - second derivative at final point, O(h^2)
double formula13(const std::vector<double> &y, double h, int i) {
  return (2.0 * y[i] - 5.0 * y[i-1] + 4.0 * y[i-2] - y[i-3]) / (h * h);
}

int main() {
  char continue_choice;

  do {
    std::cout << "Select function:\n";
    for (size_t i = 0; i < FUNCTIONS.size(); ++i) {
      std::cout << (i + 1) << ". " << FUNCTIONS[i].name << "\n";
    }
    std::cout << "Your choice: ";
    int func_choice;
    std::cin >> func_choice;

    if (func_choice < 1 || func_choice > static_cast<int>(FUNCTIONS.size())) {
      std::cout << "Invalid choice!\n";
      continue;
    }

    const FuncDescr &selected_func = FUNCTIONS[func_choice - 1];

    std::cout << "\nEnter number of points m+1 in table (minimum 5): ";
    int m;
    std::cin >> m;
    m--;

    if (m < 4) {
      std::cout << "Error: need at least 5 points!\n";
      continue;
    }

    std::cout << "Enter initial point x0: ";
    double x0;
    std::cin >> x0;

    std::cout << "Enter step h > 0: ";
    double h;
    std::cin >> h;

    if (h <= 0) {
      std::cout << "Error: step must be positive!\n";
      continue;
    }

    std::cout << "Enter number of decimal places for output (e.g., 4, 6, 8): ";
    int precision;
    std::cin >> precision;

    if (precision < 1 || precision > 15) {
      std::cout << "Warning: precision adjusted to range [1, 15]\n";
      precision = std::max(1, std::min(15, precision));
    }

    // Build function value table
    std::vector<double> x(m + 1);
    std::vector<double> y(m + 1);

    int col_width = precision + 8;

    std::cout << "\n=== Function Values Table ===\n";
    std::cout << std::setw(5) << "k" << std::setw(col_width) << "x_k" << std::setw(col_width) << "y_k" << "\n";
    std::cout << std::string(5 + 2 * col_width, '-') << "\n";

    for (int k = 0; k <= m; k++) {
      x[k] = x0 + k * h;
      y[k] = selected_func.f(x[k]);
      std::cout << std::setw(5) << k << std::setw(col_width) << std::fixed << std::setprecision(precision) << x[k] 
                << std::setw(col_width) << y[k] << "\n";
    }

    // Compute derivatives
    std::vector<double> f_prime_exact(m + 1);
    std::vector<double> f_prime_O_h2(m + 1);
    std::vector<double> f_prime_O_h4(m + 1);
    std::vector<double> f_double_prime_exact(m + 1);
    std::vector<double> f_double_prime_O_h2(m + 1);

    // Exact values
    for (int k = 0; k <= m; k++) {
      f_prime_exact[k] = selected_func.deriv1(x[k]);
      f_double_prime_exact[k] = selected_func.deriv2(x[k]);
    }

    // First derivative O(h^2)
    f_prime_O_h2[0] = formula4(y, h, 0);
    for (int k = 1; k < m; k++) {
      f_prime_O_h2[k] = formula3(y, h, k);
    }
    f_prime_O_h2[m] = formula5(y, h, m);

    // First derivative O(h^4)
    f_prime_O_h4[0] = formula7(y, h, 0);
    f_prime_O_h4[1] = formula8(y, h, 1);
    for (int k = 2; k <= m - 2; k++) {
      f_prime_O_h4[k] = formula9(y, h, k);
    }
    f_prime_O_h4[m - 1] = formula10(y, h, m - 1);
    f_prime_O_h4[m] = formula11(y, h, m);

    // Second derivative O(h^2)
    f_double_prime_O_h2[0] = formula12(y, h, 0);
    for (int k = 1; k < m; k++) {
      f_double_prime_O_h2[k] = formula6(y, h, k);
    }
    f_double_prime_O_h2[m] = formula13(y, h, m);

    // Output results
    std::cout << "\n=== Numerical Differentiation Results ===\n\n";
    std::cout << std::setw(col_width) << "x_k" << std::setw(col_width) << "y_k" << std::setw(col_width) << "f'_exact"
              << std::setw(col_width) << "~f'" << std::setw(col_width) << "error"
              << std::setw(col_width) << "~~f'" << std::setw(col_width) << "error"
              << std::setw(col_width) << "f''_exact" << std::setw(col_width) << "~f''" << std::setw(col_width) << "error" << "\n";
    std::cout << std::setw(col_width) << "" << std::setw(col_width) << "" << std::setw(col_width) << ""
              << std::setw(col_width) << "O(h^2)" << std::setw(col_width) << "O(h^2)"
              << std::setw(col_width) << "O(h^4)" << std::setw(col_width) << "O(h^4)"
              << std::setw(col_width) << "" << std::setw(col_width) << "O(h^2)" << std::setw(col_width) << "O(h^2)" << "\n";
    std::cout << std::string(10 * col_width, '-') << "\n";

    for (int k = 0; k <= m; k++) {
      std::cout << std::setw(col_width) << std::setprecision(precision) << x[k]
                << std::setw(col_width) << std::setprecision(precision) << y[k]
                << std::setw(col_width) << std::setprecision(precision) << f_prime_exact[k]
                << std::setw(col_width) << std::setprecision(precision) << f_prime_O_h2[k]
                << std::setw(col_width) << std::setprecision(precision) << std::abs(f_prime_exact[k] - f_prime_O_h2[k])
                << std::setw(col_width) << std::setprecision(precision) << f_prime_O_h4[k]
                << std::setw(col_width) << std::setprecision(precision) << std::abs(f_prime_exact[k] - f_prime_O_h4[k])
                << std::setw(col_width) << std::setprecision(precision) << f_double_prime_exact[k]
                << std::setw(col_width) << std::setprecision(precision) << f_double_prime_O_h2[k]
                << std::setw(col_width) << std::setprecision(precision) << std::abs(f_double_prime_exact[k] - f_double_prime_O_h2[k]) << "\n";
    }

    std::cout << "\nContinue? (y/n): ";
    std::cin >> continue_choice;

  } while (continue_choice == 'y' || continue_choice == 'Y');

  std::cout << "\nProgram finished.\n";

  return 0;
}