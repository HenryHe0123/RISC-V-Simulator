#ifndef RISC_V_SIMULATOR_QUEUE_H
#define RISC_V_SIMULATOR_QUEUE_H

#include <cstddef>

template<class T, int size = 64>
class Queue { //my circular queue, with capacity of size - 1
public:
    inline void push_back(const T &val) {
        q[tail] = val;
        tail = next(tail);
    }

    inline void pop_front() { head = next(head); }

    inline void pop_back() { tail = pre(tail); }

    inline void clear() { head = tail = 0; }

    inline T &front() { return q[head]; }

    inline T &back() { return q[pre(tail)]; }

    inline int front_index() { return head; }

    inline int back_index() { return pre(tail); }

    inline bool empty() { return head == tail; }

    inline bool full() { return head == next(tail); }

    inline int cnt() {
        if (head <= tail) return tail - head;
        else return size - head + tail; //head > tail
    }

    inline T &operator[](int i) { return q[i]; } //direct but also dangerous

    inline const T &operator[](int i) const { return q[i]; }

    class iterator {
    public:
        iterator(Queue<T, size> *que, int ind) : queue(que), index(ind) {}

        T &operator*() { return queue->q[index]; }

        T *operator->() { return queue->q + index; }

        iterator &operator++() {
            index = next(index);
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            index = next(index);
            return tmp;
        }

        iterator &operator--() {
            index = pre(index);
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            index = pre(index);
            return tmp;
        }

        bool operator==(const iterator &rhs) const { return queue == rhs.queue && index == rhs.index; }

        bool operator!=(const iterator &rhs) const { return queue != rhs.queue || index != rhs.index; }

    private:
        Queue<T, size> *queue = nullptr;
        int index = 0;
    };

    inline iterator end() { return iterator(this, tail); }

    inline iterator begin() { return iterator(this, head); }

private:
    T q[size]{};
    int head = 0, tail = 0;

    inline static int pre(int i) { return (i - 1 + size) % size; }

    inline static int next(int i) { return (i + 1) % size; }

    friend class Queue::iterator;
};

#endif //RISC_V_SIMULATOR_QUEUE_H
