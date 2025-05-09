#include <iostream>
#include <ranges>
#include <print>
#include <charconv>
#include <thread>
#include <functional>

#include "window.hpp"

class SplittedString {
private:
  std::string my_string;
  std::vector<std::string_view> parts;

public:
// should be a single functino split(string_view) ->  vector<string_view>
// del -> sep
  SplittedString(std::string &&str, char del = ' ') : my_string(std::move(str)) {
    char *current = nullptr;
    int cur_len = 0;

    for (int i = 0; i < my_string.length(); i++) {
      if (my_string[i] != del) {
        if (current == nullptr) {
          current = &my_string[i];
        }
        cur_len++;
      } else {
        if (current != nullptr) {
          parts.push_back(std::string_view(current, cur_len));
          cur_len = 0;
          current = nullptr;
        }
      }
    }

    if (current != nullptr) {
      parts.push_back(std::string_view(current, cur_len));
    }
  }

  const std::vector<std::string_view> & get_parts() {
    return parts;
  }

  int get_size() {
    return parts.size();
  }

  const std::string_view & operator[](int index) const {
		if (index < 0 || index >= parts.size()) {
			throw std::runtime_error("index out of range");
		}
		return parts[index];
	}
};

static int parse_int(std::string_view s) {
  int value;
  auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.length(), value);

  if (ec == std::errc::invalid_argument) {
    throw std::runtime_error("This is not a number");
  } else if (ec == std::errc::result_out_of_range) {
    throw std::runtime_error("This number is larger than an T");
  }

  return value;
}

// return errors instead of exceptions
// std::expected etc...
static void parse_command(WindowSystem &windows) {
  std::string line;
  std::getline(std::cin, line);
  SplittedString splitted(std::move(line), ' ');

  if (splitted.get_size() < 1) {
    throw std::runtime_error("invalid string");
  }
  std::string_view command = splitted[0];

  //handle_*
  if (command == "add") {
    // handle_add()
    if (splitted.get_size() < 5) {
      throw std::runtime_error("invalid string");
    }
    auto x = parse_int(splitted[1]);
    auto y = parse_int(splitted[2]);
    auto w = parse_int(splitted[3]);
    auto h = parse_int(splitted[4]);
    int id = windows.create_window(x, y, w, h);
    std::println("id: {}", id);
  } else if (command == "list") {
    const auto &list = windows.get_windows_ids();
    for (int id : list) {
      std::cout << id << ' ';
    }
    std::cout << std::endl;
  } else if (command == "delete") {
    if (splitted.get_size() < 2) {
      throw std::runtime_error("invalid string");
    }
    auto id = parse_int(splitted[1]);
    windows.delete_window(id);
  } else if (command == "click") {
    if (splitted.get_size() < 3) {
      throw std::runtime_error("invalid string");
    }
    auto x = parse_int(splitted[1]);
    auto y = parse_int(splitted[2]);
    windows.click(x, y);
  } else if (command == "close") {
    if (splitted.get_size() < 2) {
      throw std::runtime_error("invalid string");
    }
    auto id = parse_int(splitted[1]);
    windows.close_window(id);
  } else if (command == "open") {
    if (splitted.get_size() < 2) {
      throw std::runtime_error("invalid string");
    }
    auto id = parse_int(splitted[1]);
    windows.open_window(id);
  } else if (command == "move") {
    if (splitted.get_size() < 4) {
      throw std::runtime_error("invalid string");
    }
    auto id = parse_int(splitted[1]);
    auto dx = parse_int(splitted[2]);
    auto dy = parse_int(splitted[3]);
    windows.move_window(id, dx, dy);
  } else if (command == "resize") {
    if (splitted.get_size() < 4) {
      throw std::runtime_error("invalid string");
    }
    auto id = parse_int(splitted[1]);
    auto w = parse_int(splitted[2]);
    auto h = parse_int(splitted[3]);
    windows.resize_window(id, w, h);
  } else if (command == "setcolor") {
    if (splitted.get_size() < 3) {
      throw std::runtime_error("invalid string");
    }
    auto id = parse_int(splitted[1]);
    auto color = std::string(splitted[2].data());
    windows.set_window_color(id, color);
  } else if (command == "setbound") {
    if (splitted.get_size() < 5) {
      throw std::runtime_error("invalid string");
    }
    auto id = parse_int(splitted[1]);
    auto color = std::string(splitted[2].data());
    // if not windows.window_exists(id) -> return error else
    windows.set_window_bound_color(id, color);
  } else if (command == "exit") {
    exit(0);
  } else {
    std::println("this command is not exist");
  }
}

static void update(WindowSystem &windows) {
  try {
    parse_command(windows);
  } catch (std::runtime_error& e) {
    std::cout << e.what() << std::endl;
  }
}

int main(int argc, char **argv) {
  auto &windows = WindowSystem::get();
  while (true) {
    update(windows);
    windows.draw();
  }
}