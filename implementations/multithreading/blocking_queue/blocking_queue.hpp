#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T, class Container = std::queue<T>>
class BlockingQueue {
    std::mutex mtx;
    std::condition_variable cv;
    Container queue;

   public:
    void push(T item) {
        std::lock_guard lk(mtx);
        queue.push(item);
        cv.notify_one();
    }

    void pop(T& item) {
        std::unique_lock lk(mtx);
        cv.wait(lk, [this] { return !queue.empty(); });
        item = queue.front();
        queue.pop();
    }
};
