#include "event.h"

#include <SFML/Graphics.hpp>

using namespace sf;

EventManager* EventManager::em = NULL;

void EventManager::EventLoop(void)
{
    int ret;
    sf::Clock clock;

    while(true) {
        unsigned long long missed;
        ret = read(schedTimer.fd, &missed, sizeof(missed));
        assert(ret > 0);
        /* TODO: missed */

        Time t1 = clock.getElapsedTime();
        taskMutex.lock();
        for (map<int, Task*>::iterator it = tasks.begin();
                it != tasks.end(); ++it) {
            (it->second)->Call();
        }
        Time t2 = clock.restart();
        taskMutex.unlock();
#if DEBUG
        printf("%d objects: time %llu us\n", (int) tasks.size(),
                t2.asMicroseconds() - t1.asMicroseconds());
#endif
    }
}

void TaskPeriodic::Call(void)
{
    bool finish = false;
    if (idx == 0) {
        finish = cb(arg);
        if (finish) {
            printf("finished\n");
            gEm->RemoveTaskNoLock(this->id);
            /* TODO: cleanup memory */
        }
    }
    idx = (idx + 1) % loopIdx;
//    printf("task id %d idx %d loopidx %d\n", id, idx, loopIdx);
}

