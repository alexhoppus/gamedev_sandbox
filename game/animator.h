
#ifndef ANIMATOR_DEFS
#define ANIMATOR_DEFS

#include <map>
#include <string>
#include <mutex>
#include <string.h>
#include <SFML/Graphics.hpp>
#include "event.h"

typedef enum {
        ANIM_IDLE,        // 0
        ANIM_WALK,        // 1
        ANIM_MAX
} AnimId;

typedef enum {
        DIR_0,            // 0
        DIR_45,           // 1
        DIR_90,           // 2
        DIR_135,          // 3
        DIR_180,          // 4
        DIR_225,          // 5
        DIR_270,          // 6
        DIR_315,          // 7
        DIR_MAX
} DirId;

struct AnimatorResource {
    std::vector<sf::IntRect *>  frag[ANIM_MAX][DIR_MAX];
    sf::Texture                 texSheet;
    int                         animTime[ANIM_MAX];
    std::string                 name;

    void AddAnim(AnimId a, DirId d, int time, sf::IntRect *elem)
    {
        animTime[a] = time;
        frag[a][d].push_back(elem);
    }
};

class Animator {
private:
    int                         idx;
    int                         clkRestarted;
    int                         curA;
    int                         curD;
    sf::IntRect *               curAnim;
    unsigned                    lastTime;
    std::mutex                  animLock;
    int                         curAnimTask;
public:
    AnimatorResource           *ar;
    sf::Texture *GetTexSheet()
    {
        return &ar->texSheet;
    }

    bool ScheduleNextAnim(void)
    {
        animLock.lock();
        idx = (idx + 1) % ar->frag[curA][curD].size();
        animLock.unlock();
        return false;
    }

    sf::IntRect *GetCurAnim(void)
    {
        sf::IntRect *ret;
        animLock.lock();
        ret = ar->frag[curA][curD][idx];
        animLock.unlock();
        return ret;
    }

    void SetDir(DirId d)
    {
        animLock.lock();
        if (d != curD) {
            curD = d;
            idx = 0;
        }
        animLock.unlock();
    }

    void SetAnim(AnimId a)
    {
        animLock.lock();
        if (a != curA) {
            curA = a;
            gEm->SetTaskPeriodNoLock(curAnimTask, ar->animTime[curA]);
            idx = 0;
        }
        animLock.unlock();
    }

    Animator(AnimatorResource *resource)
    {
        curA = ANIM_IDLE;
        curD = DIR_0;
        ar = resource;
        idx = 0;
        curAnimTask = gEm->AddTask((new TaskPeriodic((TaskPeriodicCb)&Animator::ScheduleNextAnim,
                     (void *)this, ar->animTime[curA])));
        printf("task id %d set period %d\n", curAnimTask, ar->animTime[curA]);

    };
};

int LoadAnimLoop(std::string path, std::map<std::string, AnimatorResource> &resources);

#endif
