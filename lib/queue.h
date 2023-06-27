#ifndef RISC_V_SIMULATOR_QUEUE_H
#define RISC_V_SIMULATOR_QUEUE_H

#include <cstddef>

template<class T, int size = 64>
class Queue { //my circular queue, with capacity of size - 1
public:
    Queue() = default;

    Queue(const Queue &) = default;

    Queue(Queue &&) noexcept = default;

    Queue &operator=(const Queue &) = default;

    Queue &operator=(Queue &&) noexcept = default;

    inline void push_back(const T &val) {
        q[tail] = val;
        tail = (tail + 1) % size;
    }

    inline void pop_front() { head = (head + 1) % size; }

    inline void pop_back() { tail = (tail - 1 + size) % size; }

    inline void clear() { head = tail = 0; }

    inline T &front() { return q[head]; }

    inline T &back() { return q[(tail - 1 + size) % size]; }

    inline bool empty() { return head == tail; }

    inline bool full() { return (tail + 1) % size == head; }

    inline int cnt() {
        if (head <= tail) return tail - head;
        else return size - head + tail; //head > tail
    }

    inline T &operator[](int i) { return q[(head + i) % size]; } //dangerous

    class iterator {
    public:
        iterator() = default;

        iterator(const iterator &) = default;

        iterator(iterator &&) noexcept = default;

        iterator &operator=(const iterator &) noexcept = default;

        iterator &operator=(iterator &&) noexcept = default;

        T &operator*() { return queue[index]; }

        T *operator->() { return &(queue[index]); }

        iterator &operator++() {
            ++index;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++index;
            return tmp;
        }

        iterator &operator--() {
            --index;
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            --index;
            return tmp;
        }

        bool operator==(const iterator &rhs) const { return queue == rhs.queue && index == rhs.index; }

        bool operator!=(const iterator &rhs) const { return queue != rhs.queue || index != rhs.index; }

    private:
        explicit iterator(Queue<T, size> *que, int ind = 0) : queue(que), index(ind) {}

        Queue<T, size> *queue = nullptr;
        int index = 0;
    };

    inline iterator end() { return iterator(this, cnt()); }

    inline iterator begin() { return iterator(this, 0); }

private:
    T q[size]{};
    int head = 0, tail = 0;
};

#endif //RISC_V_SIMULATOR_QUEUE_H
