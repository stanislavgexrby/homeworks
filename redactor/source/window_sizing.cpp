#include "window.h"

static void ChangeColor() {
    printf("\033[30;107m");
}

static void RestoreColor() {
    printf("\033[0;0m");
}

static void Highlight() {
    printf("\033[93;40m");
}

void Window::PrintWindow() {
    // system("clear");
    std::cout << '0';
    for (int l = 0; l < width; ++l)
    {
        std::cout << '-';
    }
    std::cout << '0' << '\n';
    for (int i = 0; i < heigh; ++i)
    {
        std::cout << '|';
        for (int j = 0; j < width; ++j)
        {
            if (i * width + j == _pointer.GetPos()) {
                ChangeColor();
                std::cout << window[i * width + j];
                RestoreColor();
                continue;
            }
            if (_highlighted.IncludeHighlited(i * width + j)) {
                Highlight();
                std::cout << window[i * width + j];
                RestoreColor();
                continue;
            }
            std::cout << window[i * width + j];
        }
        std::cout << '|';
        std::cout << '\n';
    }
    std::cout << '0';
    for (int l = 0; l < width; ++l)
    {
        std::cout << '-';
    }
    std::cout << '0' << '\n';
    _highlighted.ClearHighlited();
    //_data.PrintData();
}
