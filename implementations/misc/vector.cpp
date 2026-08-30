#include <cstddef>
#include <memory>
#include <new>
#include <utility>

template <typename T>
class Vector {
   public:
    Vector() = default;

    Vector(const Vector& other) {
        reserve(other.size_);
        for (size_t i = 0; i < other.size_; i++) {
            std::construct_at(data_ + i, other.data_[i]);
        }
        size_ = other.size_;
    }

    Vector(Vector&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    Vector& operator=(const Vector& other) {
        if (this == &other) return *this;
        Vector tmp(other);
        swap(tmp);
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        if (this == &other) return *this;
        destroy_and_free();
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        return *this;
    }

    ~Vector() { destroy_and_free(); }

    void swap(Vector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) return;

        T* new_data = static_cast<T*>(::operator new(new_capacity * sizeof(T)));

        for (size_t i = 0; i < size_; i++) {
            std::construct_at(new_data + i, std::move(data_[i]));
            data_[i].~T();
        }

        ::operator delete(data_);
        data_ = new_data;
        capacity_ = new_capacity;
    }

    void push_back(const T& value) {
        if (size_ == capacity_) reserve(capacity_ ? capacity_ * 2 : 1);
        std::construct_at(data_ + size_, value);
        size_++;
    }

    void push_back(T&& value) {
        if (size_ == capacity_) reserve(capacity_ ? capacity_ * 2 : 1);
        std::construct_at(data_ + size_, std::move(value));
        size_++;
    }

    void pop_back() { data_[--size_].~T(); }

    T& operator[](size_t index) { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    T* begin() { return data_; }
    T* end() { return data_ + size_; }
    const T* begin() const { return data_; }
    const T* end() const { return data_ + size_; }

   private:
    void destroy_and_free() {
        for (size_t i = 0; i < size_; i++) data_[i].~T();
        ::operator delete(data_);
    }

    T* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};

void test() {
    Vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    Vector<int> copy = v;
    copy.push_back(4);

    Vector<int> moved = std::move(copy);
    moved.pop_back();
}

int main() {
    test();

    return 0;
}
