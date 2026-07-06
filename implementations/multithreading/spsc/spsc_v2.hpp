
#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <new>

template <typename T, std::size_t N> class SPSCv2 {
#ifdef __cpp_lib_hardware_interference_size
  static constexpr std::size_t cache_line =
      std::hardware_destructive_interference_size;
#else
  static constexpr std::size_t cache_line = 64;
#endif

  alignas(cache_line) std::atomic<size_t> head{0};
  alignas(cache_line) std::atomic<size_t> tail{0};
  alignas(cache_line) std::array<T, N> data;

public:
  size_t size() {
    const std::size_t t = tail.load(std::memory_order_acquire);
    const std::size_t h = head.load(std::memory_order_acquire);
    return h - t;
  }

  bool full() { return size() == N; }

  bool empty() { return size() == 0; }

  bool push(T item) {
    if (full())
      return false;

    const std::size_t h = head.load(std::memory_order_relaxed);

    data[h % N] = item;
    head.store(h + 1, std::memory_order_release);

    return true;
  }

  bool pop(T &item) {
    if (empty())
      return false;

    const std::size_t t = tail.load(std::memory_order_relaxed);

    item = data[t % N];

    tail.store(t + 1, std::memory_order_release);

    return true;
  }
};
