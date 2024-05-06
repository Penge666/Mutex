#define CATCH_CONFIG_MAIN
#include "simple_mutex.h"
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include "catch.hpp"

class SimpleMutexTester
{
private:
    int iter_count_;
    int thread_count_;
    SimpleMutex mutex_;
    int val_;

public:
    SimpleMutexTester() : iter_count_(0), thread_count_(0), val_(0) {}
    SimpleMutexTester(int iter_count, int thread_count)
        : iter_count_(iter_count), thread_count_(thread_count), val_(0) {}

    bool Test()
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < thread_count_; i++)
        {
            threads.emplace_back([this]
                                 {
                for (int i = 0;i < this->iter_count_;i ++) {
                    std::lock_guard<SimpleMutex> lock(this->mutex_);
                    this->val_ ++;
                } });
        }
        for (std::thread &t : threads)
            t.join();

        return (this->val_ == thread_count_ * iter_count_);
    }
};

TEST_CASE("SimpleMutex are sync", "[simplemutex]")
{
    SimpleMutexTester t(7, 5);
    REQUIRE(t.Test() == true);
    SimpleMutexTester t1(8, 1000);
    REQUIRE(t1.Test() == true);
    SimpleMutexTester t2(1, 1000);
    REQUIRE(t2.Test() == true);
}