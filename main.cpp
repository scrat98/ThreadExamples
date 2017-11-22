#include <iostream>
#include <thread>
#include <ctime>
#include <tuple>
#include <chrono>
#include "ThreadPool.hpp"

int main() {
    srand(time(NULL));
    std::cout << std::thread::hardware_concurrency() << " concurrent threads are supported." << std::endl;

    ThreadPool threadPool(4);
    std::vector<std::future<std::pair<int, int>>> results;

    for (int i = 0; i < 1e6; ++i) {
        results.emplace_back(
                threadPool.addTask([](int i, int rand) {
                    int x = (i + rand) * rand;
                    return std::make_pair(i, x);
                }, i, rand())
        );
    }

    for (auto& result : results) {
        try {
            auto res = result.get();
            std::cout << "Task = " << res.first
                      << " Number = " << res.second << std::endl;
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
    std::cout << std::endl;

    system("pause");

    return 0;
}