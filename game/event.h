#ifndef EVENT_DEFS
#define EVENT_DEFS
#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <assert.h>
#include <sys/timerfd.h>
#include <sys/time.h>
#include <poll.h>
#include <unistd.h>
#include <mutex>

#include "common.h"

using namespace std;

void EventLoop(void);

typedef bool (*TaskPeriodicCb)(void *);
#define FPS 50
class EventManager;
extern EventManager                     *gEm;

struct Task {
    int      id;
    virtual void Call(void) {};
    unsigned idx;
    void   *arg;
    unsigned loopIdx;

    Task(unsigned i, unsigned li, void *ar) : idx(i), loopIdx(li), arg(ar) {};
};

struct TaskPeriodic : public Task {
    TaskPeriodicCb cb;

    TaskPeriodic(TaskPeriodicCb cb_, void *arg_, unsigned loopTime) :
        Task(0, loopTime / FPS, arg_), cb(cb_) {};

    void Call(void);
};

class EventManager {
private:
    std::mutex           taskMutex;
    struct pollfd        schedTimer;
    std::map<int, Task *>  tasks;
    std::thread         *t;

    EventManager()
    {
        int ret;
        t = new std::thread(&EventManager::EventLoop, this);
        ret = CreateTimer();
        assert(ret == 0);
        TimerSetPeriod(1000000 / FPS);
    };
    int CreateTimer(void)
    {
        int ret, fd, handle;
        struct pollfd timer;

        /* Create the timer */
        fd = timerfd_create(CLOCK_MONOTONIC, 0);
        if (fd == -1)
            return fd;

        timer.fd = fd;
        timer.events = POLLIN;
        schedTimer = timer;

        return 0;
    }
    int TimerSetPeriod(unsigned period)
    {
        int ret;
        unsigned int ns, sec;
        struct itimerspec itval;
        /* Make the timer periodic */
        sec = period / 1000000;
        ns = (period - (sec * 1000000)) * 1000;
        itval.it_interval.tv_sec = sec;
        itval.it_interval.tv_nsec = ns;
        itval.it_value.tv_sec = sec;
        itval.it_value.tv_nsec = ns;

        return timerfd_settime(schedTimer.fd, 0, &itval, NULL);
    }

    void EventLoop(void);
public:
    int AddTask(Task *task)
    {
        static int i;
        taskMutex.lock();
        task->id = i;
        tasks[i++] = task;
        taskMutex.unlock();
        return (i - 1);
    }
    int RemoveTaskNoLock(int id)
    {
        int ret;
        ret = tasks.erase(id);
        return ret;
    }

    int RemoveTask(int id)
    {
        int ret;
        taskMutex.lock();
        ret = RemoveTaskNoLock(id);
        taskMutex.unlock();
        return ret;
    }
    int SetTaskPeriodNoLock(int id, unsigned time)
    {
        TaskPeriodic *t = dynamic_cast<TaskPeriodic *>(tasks[id]);
        if (t) {
            t->loopIdx = time / FPS;
            t->idx = 0;
        }
    }
    static EventManager* getInstance()
    {
        return new EventManager();
    }
};
#endif
