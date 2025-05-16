#include <atomic>
#include <random>
#include <print>
#include <thread>
#include <vector>
#include <ranges>
#include <stdexcept>

static std::tuple<std::size_t, std::size_t, std::size_t> get_random_indexes(std::size_t bound) {
  static std::random_device rd;
  static std::mt19937 gen(rd());

  if (bound < 3) {
    throw std::runtime_error("Need at least 3 unique elements");
  }

  std::uniform_int_distribution<std::size_t> dist(0, bound - 1);

  std::size_t i = dist(gen);

  std::size_t j;
  do {
    j = dist(gen);
  } while (j == i);

  std::size_t k;
  do {
    k = dist(gen);
  } while (k == i || k == j);

  return {i, j, k};
}

static std::vector<size_t> get_rand_array(std::size_t size) {
  std::vector<size_t> res(size, 1);
  for (auto &i : res) {
    i = rand();
  }
  return res;
}

static void incorrect() {
  constexpr size_t n = 40;
  auto nums = get_rand_array(n);

  size_t thread_num = 5;
  std::vector<std::jthread> threads;
  threads.reserve(thread_num);

  for (size_t th = 0; th < thread_num; th++) {
    threads.emplace_back(std::jthread([&](std::stop_token stop_token) {
      while (!stop_token.stop_requested()) {
        auto [i, j, k] = get_random_indexes(n);
        auto sum = nums[i] + nums[j] + nums[k];
        nums[i] = nums[j] = nums[k] = sum;
      }
    }));
  }
}

static void correct_a() {
  constexpr size_t n = 40;
  auto nums = get_rand_array(n);

  std::mutex mutex;

  size_t thread_num = 5;
  std::vector<std::jthread> threads;
  threads.reserve(thread_num);

  for (size_t th = 0; th < thread_num; th++) {
    threads.emplace_back(std::jthread([&](std::stop_token stop_token) {
      while (!stop_token.stop_requested()) {
        auto [i, j, k] = get_random_indexes(n);

        std::lock_guard lock(mutex);
        auto sum = nums[i] + nums[j] + nums[k];
        nums[i] = nums[j] = nums[k] = sum;
      }
    }));
  }
}

static void correct_b() {
  constexpr size_t n = 40;
  auto nums = get_rand_array(n);

  std::vector<std::mutex> mutexes(n);

  size_t thread_num = 5;
  std::vector<std::jthread> threads;
  threads.reserve(thread_num);

  for (size_t th = 0; th < thread_num; th++) {
    threads.emplace_back(std::jthread([&](std::stop_token stop_token) {
      while (!stop_token.stop_requested()) {
        auto [i, j, k] = get_random_indexes(n);
        std::array idx {i, j, k};

        size_t locked_n = 0;
        for (auto l : idx) {
          if (!mutexes[l].try_lock()) {
            break;
          }
          locked_n++;
        }

        if (locked_n == idx.size()) {
          auto sum = nums[i] + nums[j] + nums[k];
          nums[i] = nums[j] = nums[k] = sum;
        }

        for (std::size_t l = 0; l < locked_n; ++l) {
          mutexes[l].unlock();
        }
      }
    }));
  }
}

static void correct_d() {
  size_t n = 40;
  auto nums = get_rand_array(n);

  std::vector<std::mutex> mutexes(n);

  size_t thread_num = 5;
  std::vector<std::jthread> threads;
  threads.reserve(thread_num);

  std::atomic_bool done = false;
  for (size_t th = 0; th < thread_num; th++) {
    threads.emplace_back(std::jthread([&](std::stop_token stop_token) {
      while (!stop_token.stop_requested()) {
        if (done) {
          return;
        }
        auto [i, j, k] = get_random_indexes(n);
        std::array idx {i, j, k};
        std::sort(idx.begin(), idx.end());

        for (auto l : idx) {
          mutexes[l].lock();
        }

        auto sum = nums[i] + nums[j] + nums[k];
        nums[i] = nums[j] = nums[k] = sum;

        for (auto l : std::ranges::reverse_view(idx)) {
          mutexes[l].unlock();
        }
      }
    }));
  }

  done = true;
}

int main() {
  incorrect();
  std::println("incorrect done");

  // correct_a();
  // std::println("A done");
  // correct_b();
  // std::println("B done");
  // correct_d();
  // std::println("D done");
}