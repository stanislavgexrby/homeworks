#pragma once

#include <string_view>
#include <cstring>

struct Message {
    std::unique_ptr<char[]> data;
    size_t size = 0;

    Message() = default;

    explicit Message(const std::string& msg) : size(msg.size()) {
        data = std::make_unique<char[]>(size + 1);
        std::memcpy(data.get(), msg.c_str(), size);
        data[size] = '\0';
    }

    Message(const Message&) = delete;
    Message& operator=(const Message&) = delete;

    Message(Message&& other) noexcept = default;
    Message& operator=(Message&& other) noexcept = default;

    ~Message() = default;

    const char* get() const { return data.get(); }
};