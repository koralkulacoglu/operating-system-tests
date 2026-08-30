#include <cassert>
#include <iostream>
#include <vector>

template <typename T>
struct BIT {
   public:
    BIT(int n) : n_(n), data_(n, 0) {}

    void add(int idx, T val) {
        while (idx <= n_) {
            data_[idx] += val;
            idx += idx & -idx;
        }
    }

    T sum(int idx) {
        T res = 0;
        while (idx >= 1) {
            res += data_[idx];
            idx -= idx & -idx;
        }
        return res;
    }

    T sum(int left, int right) { return sum(right) - sum(left - 1); }

   private:
    int n_;
    std::vector<T> data_;
};

void testSimple() {
    std::vector<long long> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int n = arr.size();
    BIT<long long> bit(n + 1);
    for (int i = 1; i <= n; i++) bit.add(i, arr[i - 1]);

    for (int i = 1; i <= n; i++) assert(bit.sum(i, i) == i);

    assert(bit.sum(1, n) == 55);
    assert(bit.sum(2, n) == 54);
    assert(bit.sum(3, n - 1) == 42);
}

int main() {
    testSimple();

    return 0;
}
