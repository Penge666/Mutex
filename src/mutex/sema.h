#pragma once

#ifndef SIMPLEMUTEX_SEMA_H
#define SIMPLEMUTEX_SEMA_H

#include <semaphore.h>
#include <cassert>
#include <cerrno>

class Semaphore
{
public:
    Semaphore(int init_count = 0)
    {
        assert(init_count >= 0);
        sem_init(&sema_, 0, init_count);
    }

    ~Semaphore()
    {
        sem_destroy(&sema_);
    }

    void wait()
    {
        int rc;
        do
        {
            // 核心：
            // 用来等待信号量的值大于0（value > 0），等待时该线程为阻塞状态
            // 解除阻塞后sem值会减去1
            rc = sem_wait(&sema_);
        } while (rc == 1 && errno == EINTR);
    }

    void signal()
    {
        //        用来增加信号量的值（value + 1）
        sem_post(&sema_);
    }
    void signal(int count)
    {
        while (count-- > 0)
        {
            sem_post(&sema_);
        }
    }

private:
    sem_t sema_;

    Semaphore(const Semaphore &other) = delete;
    Semaphore &operator=(const Semaphore &other) = delete;
};

#endif // SIMPLEMUTEX_SEMA_H
