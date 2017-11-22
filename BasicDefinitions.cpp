#include <iostream>
#include <future>
#include <functional>
#include <ctime>
#include <thread>
#include <chrono>
#include <mutex>
#include <future>
#include <condition_variable>

class SayHello {
public:
    void operator()(std::promise<int> p) const {
        this->do_smth(std::move(p));
    }

    void execute(std::promise<int> p) const {
        this->do_smth(std::move(p));
    }

private:
    void do_smth(std::promise<int> p) const {
        try {
            auto val = rand() % 10;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (val == 5) throw std::invalid_argument("some error");
            p.set_value(val);
        } catch (...) {
            try {
                p.set_exception(std::current_exception());
            } catch (...) { throw; }
        }
    }
};

class SayHelloTask {
public:
    int operator()() const {
        this->do_smth();
    }

    int execute() const {
        this->do_smth();
    }

private:
    int do_smth() const {
        auto val = rand() % 10;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (val == 5) throw std::invalid_argument("some error");

        return val;
    }
};

int basicDefinition() {
    srand(time(NULL));
    std::cout << std::thread::hardware_concurrency() << " concurrent threads are supported." << std::endl;

// promise and future
//    std::promise<int> p;
//    auto f = p.get_future();
//    std::thread t{SayHello(), std::move(p)}; // with () operator
//    std::thread t(&SayHello::execute, SayHello(), std::move(p)); // with function
//    t.detach();

// Packaged task
//    http://en.cppreference.com/w/cpp/thread/packaged_task
//    http://en.cppreference.com/w/cpp/utility/functional/bind

//    std::packaged_task<int()> task{SayHelloTask()}; // with () operator
//    std::packaged_task<int()> task(std::bind(&SayHelloTask::execute, SayHelloTask())); // with function
//    auto f = task.get_future();
//    std::thread t(std::move(task));
//    t.detach();

// Async
//    https://stackoverflow.com/questions/18143661/what-is-the-difference-between-packaged-task-and-async
//    http://en.cppreference.com/w/cpp/thread/async
//    auto f = std::async(std::launch::async, &SayHelloTask::execute, SayHelloTask());

// EXECUTE
//    f.wait();
//    try {
//        std::cout << f.get() << std::endl;
//    } catch (std::exception& e) {
//        std::cout << e.what() << std::endl;
//    }
//    std::cout << "JOIN!" << std::endl;
    return 0;
}