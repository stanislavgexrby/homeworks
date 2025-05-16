#include <atomic>
#include <random>
#include <print>
#include <thread>
#include <vector>
#include <ranges>
#include <stdexcept>
#include <memory>
#include <mutex>
#include <iostream>

constexpr size_t size = 10000;

static std::tuple<std::size_t, std::size_t, std::size_t> get_random_indexes(std::size_t size) {
  thread_local std::mt19937 gen(std::random_device{}());
  std::uniform_int_distribution<std::size_t> dist(0, size - 1);

  size_t i, j, k;
  do {
    i = dist(gen);
    j = dist(gen);
    k = dist(gen);
  } while (i == j || i == k || j == k);

  return {i, j, k};
}

static std::vector<size_t> get_random_array(std::size_t size) {
  std::vector<size_t> res;
  res.reserve(size);

  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<std::size_t> value_dist(0, 1000);

  for (size_t i = 0; i < size; ++i) {
    res.push_back(value_dist(gen));
  }
  return res;
}

static void naive() {
  auto nums = std::make_shared<std::vector<size_t>>(get_random_array(size));

  size_t thread_num = 5;
  std::vector<std::jthread> threads;
  threads.reserve(thread_num);

  for (size_t th = 0; th < thread_num; th++) {
    threads.emplace_back(std::jthread([&](std::stop_token stop_token) {
      while (!stop_token.stop_requested()) {
        auto [i, j, k] = get_random_indexes(size);

        auto sum = (*nums)[i] + (*nums)[j] + (*nums)[k];
        (*nums)[i] = (*nums)[j] = (*nums)[k] = sum;

        // std::this_thread::sleep_for(std::chrono::microseconds(10));
      }
    }));
  }

  // std::this_thread::sleep_for(std::chrono::seconds(2));
}

static void way_a() {
  auto nums = std::make_shared<std::vector<size_t>>(get_random_array(size));
  auto mutex = std::make_shared<std::mutex>();

  size_t thread_num = 5;
  std::vector<std::jthread> threads;
  threads.reserve(thread_num);

  for (size_t th = 0; th < thread_num; th++) {
    threads.emplace_back(std::jthread([nums, mutex](std::stop_token stop_token) {
      while (!stop_token.stop_requested()) {
        auto [i, j, k] = get_random_indexes(size);
        {
          std::lock_guard<std::mutex> lock(*mutex);
          auto sum = (*nums)[i] + (*nums)[j] + (*nums)[k];
          (*nums)[i] = (*nums)[j] = (*nums)[k] = sum;
        }

        // std::this_thread::sleep_for(std::chrono::microseconds(10));
      }
    }));
  }

  // std::this_thread::sleep_for(std::chrono::seconds(2));
}

static void way_b() {
  auto nums = std::make_shared<std::vector<size_t>>(get_random_array(size));
  auto mutexes = std::make_shared<std::vector<std::mutex>>(size);

  size_t thread_num = 5;
  std::vector<std::jthread> threads;
  threads.reserve(thread_num);

  for (size_t th = 0; th < thread_num; th++) {
    threads.emplace_back(std::jthread([nums, mutexes](std::stop_token stop_token) {
      while (!stop_token.stop_requested()) {
        auto [i, j, k] = get_random_indexes(size);

        std::unique_lock lock_i((*mutexes)[i], std::try_to_lock);
        std::unique_lock lock_j((*mutexes)[j], std::try_to_lock);
        std::unique_lock lock_k((*mutexes)[k], std::try_to_lock);

        if (lock_i.owns_lock() && lock_j.owns_lock() && lock_k.owns_lock()) {
            auto sum = (*nums)[i] + (*nums)[j] + (*nums)[k];
            (*nums)[i] = (*nums)[j] = (*nums)[k] = sum;
        }

        // std::this_thread::sleep_for(std::chrono::microseconds(10));

      }
    }));
  }

  // std::this_thread::sleep_for(std::chrono::seconds(2));
}

static void way_d() {
  auto nums = std::make_shared<std::vector<size_t>>(get_random_array(size));
  auto mutexes = std::make_shared<std::vector<std::mutex>>(size);

  size_t thread_num = 5;
  std::vector<std::jthread> threads;
  threads.reserve(thread_num);

  for (size_t th = 0; th < thread_num; th++) {
    threads.emplace_back(std::jthread([nums, mutexes](std::stop_token stop_token) {
      while (!stop_token.stop_requested()) {
        auto [i, j, k] = get_random_indexes(size);
        std::array<size_t, 3> indexes = {i, j, k};
        std::sort(indexes.begin(), indexes.end());

        std::unique_lock lock1((*mutexes)[indexes[0]], std::try_to_lock);
        if (!lock1.owns_lock()) {
          continue;
        }
        std::unique_lock lock2((*mutexes)[indexes[1]], std::try_to_lock);
        if (!lock2.owns_lock()) {
          continue;
        }
        std::unique_lock lock3((*mutexes)[indexes[2]], std::try_to_lock);
        if (!lock3.owns_lock()) {
          continue;
        }
        auto sum = (*nums)[i] + (*nums)[j] + (*nums)[k];
        (*nums)[i] = (*nums)[j] = (*nums)[k] = sum;

        // std::this_thread::sleep_for(std::chrono::microseconds(20));

      }
    }));
  }

  // std::this_thread::sleep_for(std::chrono::seconds(2));
}

int main() {
  auto start = std::chrono::high_resolution_clock::now();
  naive();
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;
  std::cout << "naive time : " << duration.count() << std::endl;

  start = std::chrono::high_resolution_clock::now();
  way_a();
  end = std::chrono::high_resolution_clock::now();
  duration = end - start;
  std::cout << "way a time : " << duration.count() << std::endl;

  start = std::chrono::high_resolution_clock::now();
  way_b();
  end = std::chrono::high_resolution_clock::now();
  duration = end - start;
  std::cout << "way b time : " << duration.count() << std::endl;

  start = std::chrono::high_resolution_clock::now();
  way_d();
  end = std::chrono::high_resolution_clock::now();
  duration = end - start;
  std::cout << "way d time : " << duration.count() << std::endl;

  return 0;
}