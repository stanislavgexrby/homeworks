#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>

static bool check_brackets(const std::string &input) {
    int len = input.length();
    std::vector<char> set_of_brackets;
    for (int i = 0; i < len; ++i) {
        char c = input[i];
        if (c == '(' || c == '[') {
            set_of_brackets.push_back(c);
        } else if (c == ')') {
            char s = set_of_brackets.back();
            set_of_brackets.pop_back();
            if (s != '(') {
                return false;
            }
        } else if (c == ']') {
            char s = set_of_brackets.back();
            set_of_brackets.pop_back();
            if (s != '[') {
                return false;
            }
        }
    }
    return set_of_brackets.empty();
}

int main() {
    std::string input;
    std::cin >> input;
    if (check_brackets(input) == true) {
        std::cout << "Все верно";
    } else {
        std::cout << "Неверно";
    }
    return 0;
}