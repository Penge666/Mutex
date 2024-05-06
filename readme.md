# 基于原子变量和信号量实现互斥锁

## 前言

互斥锁在并发编程中经常碰到，因此对其需要有个更加深入的理解。这篇博客将使用信号量实现的互斥锁。

简易互斥锁（SimpleMutex）是一个基于原子变量和信号量的互斥锁实现，用于保护并管理多线程环境下的共享资源访问。它提供了一种简单而有效的方式来确保在多线程并发访问时，只有一个线程可以同时访问受保护的资源，从而避免数据竞争和不一致性。基于 POSIX 标准的信号量库实现，包含 Catch2 单元测试，附带了基于 Catch2 框架的单元测试，用于验证互斥锁的正确性和稳定性，使用bazel编译，google编码规范。

其中涉及C++知识（RAII、信号量、lock_guard、线程安全编程）

## 前置知识

> **信号量API**

- 信号量的类型：`sem_t`

- `int sem_init(sem_t *sem, int pshared, unsigned int value);`


  - 功能：初始化信号量
  - 参数
    - `sem`：信号量变量的地址
    - `pshared`：0 用在线程间 ，非0 用在进程间
    - `value `：信号量中的值，代表容器大小

- `int sem_destroy(sem_t *sem);`
  - 功能：释放资源

- `int sem_wait(sem_t *sem);`
  - 功能：对信号量加锁，调用一次对信号量的值-1，如果值为0，就阻塞

- `int sem_trywait(sem_t *sem);`

- `int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);`

- int sem_post(sem_t *sem);

  - 功能：对信号量解锁，调用一次对信号量的值+1

- `int sem_getvalue(sem_t *sem, int *sval);`

## 核心逻辑

> **Semaphore类**

```cpp
Semaphore(int init_count = 0) {
    assert(init_count >= 0);
    sem_init(&sema_, 0, init_count);
}

void wait() {
    int rc;
    do {
        rc = sem_wait(&sema_);
    } while (rc == 1 && errno == EINTR);
}

void signal() {
    sem_post(&sema_);
}
```

比较核心就是以上函数，需要注意的是，这里信号量初始化设置成0是因为只处理需要等待的线程。

当需要等待的线程进入到wait函数中，sem_wait函数将检查sema是否大于0，如果没有，则等待，等待时该线程为阻塞状态。当其他线程调用post增强sem的值的时候,即大于0的时候，线程将解除阻塞， 解除阻塞后sem值会减去1。

sem_post 则是 用来增加信号量的值。

> **SimpleMutex 类**

**lock() 和 unlock()**

SimpleMutex 类包含一个名为 count_ 的 std::atomic 变量和一个名为 sema_ 的 Semaphore 对象。

在构造函数中，count_ 被初始化为 0。

- lock() 函数用于获取互斥锁。它使用 fetch_add 操作和 std::memory_order_acquire 参数对 count_ 进行原子增加，并获取锁。 如果在增加之前 count_ 的值大于 0，说明互斥锁已经被其他线程锁定。在这种情况下，函数调用 sema_.wait() 来阻塞当前线程，直到信号量被发信号，表示互斥锁可用。
- unlock() 函数用于释放互斥锁。它使用 fetch_sub 操作和 std::memory_order_release 参数对 count_ 进行原子减少，并释放锁。 如果减少之前的 count_ 值仍大于 1，说明其他线程正在等待互斥锁。在这种情况下，函数调用 sema_.signal() 发信号给信号量，允许一个等待的线程获取互斥锁。

通过结合原子变量 count_ 和信号量 sema_，该实现确保等待获取互斥锁的线程能够高效地阻塞，直到当前持有者释放锁。

Note:

## 代码

```bash
g++ src/mutex/test.cpp -std=c++11 -Ithird/catch2/ -o test -lpthread
```

```c++
./test
```

