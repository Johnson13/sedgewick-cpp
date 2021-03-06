// ex.4.39

// en: Modify the linked-list–based FIFO queue implementation in the
// text (Program 4.14) to call a function error() if the client
// attempts to get when the queue is empty or if there is no memory
// available from new for a put.

// ru: Измените приведенную в тексте реализацию очереди FIFO на базе
// связного списка (программа 4.14) так, чтобы в ней вызывалась
// функция error(), если клиент пытается выполнить операцию извлечь,
// когда очередь пуста, или если при выполнении new для операции
// занести отсутствует доступная память.

#include <memory>
#include <stdexcept>

#include <catch.hpp>

bool fail_alloc = false;

template <typename T>
class Queue {
   private:
    struct Node {
        explicit Node(T v) : item{v}, next{nullptr} {}

        void* operator new(std::size_t size) {
            if (fail_alloc) {
                throw std::bad_alloc();
            }

            return ::malloc(size);
        }

        void operator delete(void* p) { ::free(p); }

        T item;
        Node* next;
    };
    typedef Node* Link;
    Link head_;
    Link tail_;

   public:
    explicit Queue(int) : head_{nullptr}, tail_{nullptr} {}

    ~Queue() {
        auto cur = head_;
        while (cur) {
            auto next = cur->next;
            delete cur;
            cur = next;
        }
    }

    bool Empty() const { return head_ == nullptr; }

    void Put(T x) {
        Link t = tail_;
        try {
            tail_ = new Node(x);
        } catch (const std::bad_alloc& e) {
            throw std::length_error("queue is full (out of memory)");
        }

        if (head_) {
            t->next = tail_;
        } else {
            head_ = tail_;
        }
    }

    T Get() {
        if (Empty()) Error("queue is empty");
        T v = head_->item;
        Link t = head_->next;
        delete head_;
        head_ = t;
        return v;
    }

   private:
    void Error(const char* msg) const { throw std::length_error(msg); }
};

TEST_CASE("normal usage") {
    Queue<int> q{3};
    q.Put(1);
    REQUIRE(q.Get() == 1);
    q.Put(2);
    q.Put(3);
    REQUIRE(q.Get() == 2);
    q.Put(4);
    REQUIRE(q.Get() == 3);
    q.Put(5);
    q.Put(6);
    REQUIRE(q.Get() == 4);
    REQUIRE(q.Get() == 5);
    REQUIRE(q.Get() == 6);
}

TEST_CASE("empty queue") {
    Queue<int> q{3};
    REQUIRE_THROWS_AS(q.Get(), std::length_error);
}

TEST_CASE("full queue") {
    Queue<int> q{3};
    q.Put(1);
    q.Put(2);
    q.Put(3);
    fail_alloc = true;
    REQUIRE_THROWS_AS(q.Put(4), std::length_error);
    fail_alloc = false;
}
