#include <cassert>
#include <cstdint>
#include <cstdio>
#include <thread>

#include "spsc_v2.hpp"

void test_basic() {
    SPSCv2<int, 4> q;

    assert(q.empty());
    assert(q.push(1));
    assert(q.push(2));
    assert(q.push(3));
    assert(q.push(4));
    assert(!q.push(5));  // full: capacity is N == 4
    assert(q.full());

    int v;
    assert(q.pop(v) && v == 1);
    assert(q.pop(v) && v == 2);
    assert(q.push(5));  // room again after popping

    assert(q.pop(v) && v == 3);
    assert(q.pop(v) && v == 4);
    assert(q.pop(v) && v == 5);
    assert(q.empty());
    assert(!q.pop(v));  // empty
}

// One producer and one consumer hammer the queue. The consumer checks that
// every value arrives exactly once and in FIFO order. Run this under
// ThreadSanitizer (-fsanitize=thread) to validate the acquire/release pairing.
void test_spsc_threaded() {
    constexpr std::uint64_t count = 5'000'000;
    SPSCv2<std::uint64_t, 1024> q;
    std::uint64_t consumed_sum = 0;

    std::thread producer([&] {
        for (std::uint64_t i = 1; i <= count; ++i) {
            while (!q.push(i)) {
                // spin until there is room
            }
        }
    });

    std::thread consumer([&] {
        std::uint64_t received = 0;
        std::uint64_t expected = 1;
        std::uint64_t v;
        while (received < count) {
            if (q.pop(v)) {
                assert(v == expected);  // FIFO order must be preserved
                ++expected;
                consumed_sum += v;
                ++received;
            }
        }
    });

    producer.join();
    consumer.join();

    const std::uint64_t expected_sum = count * (count + 1) / 2;
    assert(consumed_sum == expected_sum);
}

int main() {
    test_basic();
    test_spsc_threaded();
    std::puts("all tests passed!");
}
