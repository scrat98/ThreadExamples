#ifndef LAB4_THREADSEXAMPLES_THREADPOOL_HPP
#define LAB4_THREADSEXAMPLES_THREADPOOL_HPP
// https://www.youtube.com/watch?v=LL8wkskDlbs&list=PL5jc9xFGsL8E12so1wlMS0r0hTQoJL74M see the lessons

#include <future>
#include <functional>
#include <thread>
#include <chrono>
#include <mutex>
#include <future>
#include <condition_variable>
#include <vector>
#include <queue>

class ThreadPool {
public:
    ThreadPool(size_t);

    ~ThreadPool();

    // http://kevinushey.github.io/blog/2016/01/27/introduction-to-c++-variadic-templates/
    // https://eli.thegreenplace.net/2014/variadic-templates-in-c/
    // https://en.wikipedia.org/wiki/C%2B%2B11#Rvalue_references_and_move_constructors
    // https://stackoverflow.com/questions/26383517/how-to-deduce-the-return-type-of-a-function-object-from-parameters-list
    // https://www.safaribooksonline.com/library/view/effective-modern-c/9781491908419/ch01.html#understand_template_type_deduction
    template<class F, class... Args>
    auto addTask(F&& f, Args&& ... args)
    -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;

    std::mutex mtx;
    std::condition_variable cv;
//  bool stop;

//  thread can caches this variable. But mutable it's bad practice
//  http://en.cppreference.com/w/cpp/atomic/atomic
    std::atomic<bool> stop;
};

// It's bad practice
// 1) do useless checks in infinity loop
// 2) lock/unlock mutex for all threads. And n-1 threads will know, that mutex has locked yet
//ThreadPool::ThreadPool(size_t threadsCount) : stop(false) {
//    for (size_t i = 0; i < threadsCount; i++) {
//        threads.emplace_back([this]() {
//            while (true) {
//                std::function<void()> task;
//                if (!tasks.empty() || stop) {
//                    std::unique_lock<std::mutex> lock(mtx);
//                    if (stop && tasks.empty()) return;
//                    else if (tasks.empty()) continue;
//                    task = std::move(tasks.front());
//                    tasks.pop();
//                }
//                if (task) task();
//            }
//    }
//}

//        });
// To avoid logic above we should use condition_variable
// http://en.cppreference.com/w/cpp/thread/condition_variable
ThreadPool::ThreadPool(size_t threadsCount) : stop(false) {
    for (size_t i = 0; i < threadsCount; i++) {
        threads.emplace_back([this]() {
            while (true) {
                std::function<void()> task;
                /* if (!tasks.empty() || stop)*/ {
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this]() { return !tasks.empty() || stop == true; });
                    if (stop && tasks.empty()) return;
                    // else if (tasks.empty()) continue;
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                /*if (task)*/ task();
            }
        });
    }
}

template<class F, class... Args>
auto ThreadPool::addTask(F&& function, Args&& ... args)
-> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

//  it doesn't work. see below (Fucking c++)
//    auto task = std::packaged_task<return_type()>(
//            std::bind(std::forward<F>(function), std::forward<Args>(args)...)
//    );
//    auto result = task.get_future();

// make copy with std::make_shared, cause std::function in our queue requires copyable elements,
// but promise isn't(it's only movable - std::move)
    auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(function), std::forward<Args>(args)...)
    );
    auto result = task->get_future();

    {
        std::unique_lock<std::mutex> lock(mtx);
        if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");
        tasks.emplace([task]() { (*task)(); });
    }

    cv.notify_one();
    return result;
};

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop = true;
    }
    cv.notify_all();

    for (std::thread& thread : threads) {
        thread.join();
    }
}

#endif LAB4_THREADSEXAMPLES_THREADPOOL_HPP