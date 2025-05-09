#include <iostream>
#include <cstring>
#include <GL/glut.h>

#include "window.hpp"

#define RST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


void WindowSystem::clear_screen() {
  for (int i = 0; i < _screen_width; i++) {
    for (int j = 0; j < _screen_heigth; j++) {
      set_pixel(i, j, ' ', Color::WHITE);
    }
  }
}

WindowSystem::WindowSystem() {
  clear_screen();
}

void WindowSystem::draw() {
  clear_screen();

  for (const auto &wnd : _windows) {
    if (!wnd->_opened) {
      continue;
    }

    int lastx = wnd->_x + wnd->_width;
    int lasty = wnd->_y + wnd->_heigth;
    for (int y = wnd->_y; y < lasty; y++) {
      for (int x = wnd->_x; x < lastx; x++) {
        Color color =
          x == wnd->_x || y == wnd->_y || x == lastx - 1 || y == lasty - 1
          ? wnd->_bound_color : wnd->_color;
        set_pixel(x, y, '*', color);
      }
    }
  }

  draw_screen();
}


void WindowSystem::draw_screen() const {
  putchar_on_pixel(Pixel {Color::WHITE, '+'});
  for (int i = 0; i < _screen_width ; i++) {
    putchar_on_pixel(Pixel {Color::WHITE, '-'});
  }
  putchar_on_pixel(Pixel {Color::WHITE, '+'});
  std::cout << '\n';

  for (int y = 0; y < _screen_heigth; y++) {
    putchar_on_pixel(Pixel {Color::WHITE, '|'});
    for (int x = 0; x < _screen_width; x++) {
      putchar_on_pixel(_screen[y * _screen_width + x]);
    }
    putchar_on_pixel(Pixel {Color::WHITE, '|'});
    std::cout << '\n';
  }

  putchar_on_pixel(Pixel {Color::WHITE, '+'});
  for (int i = 0; i < _screen_width ; i++) {
    putchar_on_pixel(Pixel {Color::WHITE, '-'});
  }
  putchar_on_pixel(Pixel {Color::WHITE, '+'});
  std::cout << '\n';
}

void WindowSystem::set_pixel(int x, int y, char ch, Color color) {
  if (x < 0 || x >= _screen_width || y < 0 || y >= _screen_heigth) {
    return; // throw std::invalid_argument or assert()
  }
  // structured bindings
  auto &pixel = _screen[y * _screen_width + x];
  pixel.ch = ch;
  pixel.color = color;
}

void WindowSystem::putchar_on_pixel(const Pixel &pixel) const {
  switch (pixel.color) {
    // it's possible to otput enum values directly
  case Color::WHITE:
    // MASTERPIECE
    // std::cout << FWHT(<< pixel.ch <<);
    std::cout << KWHT;
    break;
  case Color::BLUE:
    std::cout << KBLU;
    break;
  case Color::RED:
    std::cout << KRED;
    break;
  case Color::YELLOW:
    std::cout << KYEL;
    break;
  default:
    break;
  }

  std::cout << pixel.ch << RST;
}