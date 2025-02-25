#include <array>

template <typename T, std::size_t Capacity>
class static_vector {
public:
    static_vector() = default;

    static constexpr std::size_t capacity() { return Capacity; }
    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    bool full() const { return size_ == Capacity; }

    void push_back(const T& value) {
        std::size_t pos = (front_ + size_) % Capacity;
        data_[pos] = value;
        ++size_;
    }

    void pop_back() {
        --size_;
    }

    void push_front(const T& value) {
        front_ = (front_ == 0) ? (Capacity - 1) : (front_ - 1);
        data_[front_] = value;
        ++size_;
    }

    void pop_front() {
        front_ = (front_ + 1) % Capacity;
        --size_;
    }

    T& front() {
        return data_[front_];
    }

    const T& front() const {
        return data_[front_];
    }

    T& back() {
        std::size_t pos = (front_ + size_ - 1) % Capacity;
        return data_[pos];
    }

    const T& back() const {
        std::size_t pos = (front_ + size_ - 1) % Capacity;
        return data_[pos];
    }

    T& operator[](std::size_t index) {
        std::size_t pos = (front_ + index) % Capacity;
        return data_[pos];
    }

    const T& operator[](std::size_t index) const {
        std::size_t pos = (front_ + index) % Capacity;
        return data_[pos];
    }

private:
    std::array<T, Capacity> data_{};
    std::size_t front_ = 0;
    std::size_t size_ = 0;
};