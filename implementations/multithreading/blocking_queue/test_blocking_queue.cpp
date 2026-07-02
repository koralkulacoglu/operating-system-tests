#include <atomic>
#include <cassert>
#include <cstdio>
#include <thread>
#include <vector>

#include "blocking_queue.hpp"

void test_basic() {
    BlockingQueue<int> q;

    q.push(1);
    q.push(2);
    q.push(3);

    int val;
    q.pop(val);
    assert(val == 1);

    q.pop(val);
    assert(val == 2);

    q.pop(val);
    assert(val == 3);
}

void test_mpmc_stress() {
    constexpr int kProducers = 4;
    constexpr int kConsumers = 4;
    constexpr int kItemsPerProducer = 10000;
    constexpr int kTotal = kProducers * kItemsPerProducer;
    static_assert(kTotal % kConsumers == 0);

    BlockingQueue<int> q;
    std::vector<std::atomic<int>> seen(kTotal);
    for (auto& s : seen) s = 0;

    std::vector<std::thread> producers;
    for (int p = 0; p < kProducers; ++p) {
        producers.emplace_back([&q, p] {
            for (int i = 0; i < kItemsPerProducer; ++i) {
                q.push(p * kItemsPerProducer + i);
            }
        });
    }

    std::vector<std::thread> consumers;
    for (int c = 0; c < kConsumers; ++c) {
        consumers.emplace_back([&q, &seen] {
            for (int i = 0; i < kTotal / kConsumers; ++i) {
                int val;
                q.pop(val);
                seen[val].fetch_add(1);
            }
        });
    }

    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();

    for (int i = 0; i < kTotal; ++i) {
        assert(seen[i].load() == 1);
    }
}

int main() {
    test_basic();
    test_mpmc_stress();
    std::puts("all tests passed!");
}
