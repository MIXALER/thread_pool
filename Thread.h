#ifndef _THREAD_H
#define _THREAD_H

#include <functional>
#include <pthread.h>
#include <memory>
#include <iostream>

class ThreadPool;

typedef std::function<void *(void *)> TaskFunc;
typedef void *TaskArgs;

struct ThreadTask
{
    TaskFunc taskFunc_;
    TaskArgs taskArgs_;
};


class Thread
{
public:
    Thread(pthread_cond_t *cond, pthread_mutex_t *mutex, std::shared_ptr<ThreadPool> threadHolder);

    ~Thread();

    void Start();

    void Shut();

    bool IsJoin()
    { return joined_; }

    bool IsAlive();

    pthread_t GetPid()
    {
        return pid_;
    }

private:
    bool running_;
    bool joined_;
    bool shutDown_;
    pthread_t pid_;
    pthread_mutex_t *mutex_;
    pthread_cond_t *cond_;

    std::shared_ptr<ThreadPool> threadHolder_;
};

#endif
