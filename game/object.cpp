#include "object.h"
#include "tilemap.h"
#include "path.h"

list<DrawableObject *>              gDobjs;
list<Object *>                      gObjs;
std::list<TransformableObject *>    gTobjs;
std::mutex                          dobjListLock;
std::mutex                          objListLock;

Object::Object()
{
    static int i;
    /* TODO: Reuse freed ids */
    id = i++;
    assert(id < OBJS_MAX);
    objListLock.lock();
    gObjs.push_back(this);
    thisObj = std::prev(gObjs.end());
    objListLock.unlock();
}

Object::~Object()
{
    objListLock.lock();
    gObjs.erase(thisObj);
    objListLock.unlock();
    printf("delete obj [%d]\n", id);
}

DrawableObject::DrawableObject(Animator *a, ScreenCoord p, Vector2f s) :
        an(a), pos(p), scale(s)
{
    sprite.setTexture(*(a->GetTexSheet()));
    sprite.scale(scale);
    /* Set origin to center */
    dobjListLock.lock();
    gDobjs.push_back(this);
    thisObj = std::prev(gDobjs.end());
    dobjListLock.unlock();
}

DrawableObject::~DrawableObject()
{
    dobjListLock.lock();
    gDobjs.erase(thisObj);
    dobjListLock.unlock();
}

int DrawableObject::ScheduleDraw(sf::RenderWindow *window)
{
#ifdef HIGHLIGHT_CENTER
    sf::CircleShape centerMarker(2);
    centerMarker.setFillColor(sf::Color(255, 0, 0));
#endif
    sprite.setTexture(*(an->GetTexSheet()));
    IntRect r = *(an->GetCurAnim());
    Vector2f center = Vector2f((r.width / 2) * scale.x,
            (r.height / 2) * scale.y);
#ifdef HIGHLIGHT_CENTER
    centerMarker.setOrigin(Vector2f(1,1));
    centerMarker.setPosition(pos);
#endif

    sprite.setOrigin(center);
    sprite.setPosition(pos);
    sprite.setTextureRect(r);
    window->draw(sprite);
#ifdef HIGHLIGHT_CENTER
    window->draw(centerMarker);
#endif
}

/*                  DIR6(270)
 *                    |
 *    DIR5(225)   \   |   / DIR7(315)
 *                 \  |  /
 *                  \ | /
 *  -DIR4(180) ------- ------>x DIR0 (0)
 *                  / |\
 *                 /  | \
 *                /   |  \
 *         DIR3 (135) |   \DIR1 (45)
 *                    |
 *                    |
 *                    v DIR2 (90)
 *                    y
 * */

struct Direction {
    DirId    id;
    Vector2f vector;
};

Direction dirTable[DIR_MAX] = {
    /*0 0*/
    {DIR_270, sf::Vector2f(0, -1)},
    /*0 1*/
    {DIR_225, sf::Vector2f(-sqrt(2) / 2, -sqrt(2) / 2)},
    /*0 2*/
    {DIR_180, sf::Vector2f(-1, 0)},
    /*1 0*/
    {DIR_315, sf::Vector2f(sqrt(2) / 2, -sqrt(2) / 2)},
    /*1 2*/
    {DIR_135, sf::Vector2f(-sqrt(2) / 2, sqrt(2) / 2)},
    /*2 0*/
    {DIR_0,   sf::Vector2f(1, 0)},
    /*2 1*/
    {DIR_45,  sf::Vector2f(sqrt(2) / 2, sqrt(2) / 2)},
    /*2 2*/
    {DIR_90,  sf::Vector2f(0, 1)}
};

DirId TransformableObject::ComputeDirection(Vector2f curDir)
{
    float cos, maxCos = 0;
    int maxId = 0;
    for (int i = 0; i < DIR_MAX; i++) {
        cos = Dot(curDir, dirTable[i].vector);
        if (cos > maxCos) {
            maxCos = cos;
            maxId = dirTable[i].id;
        }
    }
    return (DirId) maxId;
}

extern TileMap *gTm;

bool TransformableObject::ScheduleTranslate(void)
{
    if (!path.empty()) {
        ScreenCoord dst;
        Tile *dstTile = path.front();

        an->SetAnim(ANIM_WALK);

        dst = dstTile->GetScreenCoord();

        if (!isInsideArea(dst, this->GetScreenCoord(), velocity * 1)) {
            Vector2f vector = (dst - this->GetScreenCoord()) /
                Norm(dst - this->GetScreenCoord());
            Vector2f nextPos;
            DirId dId = ComputeDirection(vector);

            an->SetDir(dId);

            nextPos.x = pos.x + vector.x * velocity;
            nextPos.y = pos.y + vector.y * velocity;

            Tile *t = gTm->GetTile(nextPos);
            if ((t == dstTile) && (dstTile != curTile)) {
                assert(curTile);
                /* TODO: atomic */
                if (!t->GetOccupied()) {
                    curTile->SetOccupied(false);
                    curTile = t;
                    curTile->SetOccupied(true);
                } else {
                    int  ret = 0;
                    Tile *altDstTile = NULL;
                    list<Tile *> altPath;
                    std::list<Tile *>::iterator it;

                    an->SetAnim(ANIM_IDLE);
                    if (blockWaitCtr++ == blockWaitLimit) {
                        printf("curTask %d -> IDLE\n", curTask);
                        for (it = std::begin(path); it != std::end(path); ++it) {
                            if (!(*it)->GetOccupied()) {
                                assert(it != std::begin(path));
                                altDstTile = *it;
                                break;
                            }
                        }
                        if (altDstTile) {
                            ret = pf->PathFind(curTile->GetTileCoord().x,
                                    curTile->GetTileCoord().y,
                                    altDstTile->GetTileCoord().x,
                                    altDstTile->GetTileCoord().y, altPath);
                            if (ret)
                                return true;
                        } else {
                            return true;
                        }
                        path.erase(std::begin(path), it);
                        assert(altPath.size() > 0);
                        if (altPath.size() > 1)
                            path.insert(std::begin(path), altPath.begin(), prev(altPath.end()));
                        blockWaitCtr = 0;
                    }
                    return false;
                }
            }

            pos = nextPos;
        } else {
            path.pop_front();
        }
        return false;
    }
exit:
    an->SetAnim(ANIM_IDLE);
    printf("[%p] task %d finished\n", this, curTask);
    curTask = -1;
    return true;
}

TransformableObject::TransformableObject(AnimatorResource *ar, TileCoord p, Vector2f s,
        float v) :
        DrawableObject(new Animator(ar), Vector2f(gTm->GetTile(p)->pos), s),
        velocity(v)
{
    gTobjs.push_back(this);
    curTask = -1;
    pf = new PathFinder(gTm);
    curTile = gTm->GetTile(pos);
    curTile->SetOccupied(true);
    blockWaitCtr = 0;
    blockWaitLimit = 60;
};

void TransformableObject::MoveTo(ScreenCoord dst)
{
    int ret;
    printf("[%p] begin move, cur task %d\n", this, curTask);
    if (curTask != -1) {
        printf("[%p] remove task %d\n", this, curTask);
        gEm->RemoveTask(curTask);
        path.resize(0);
        an->SetAnim(ANIM_IDLE);
        curTask = -1;
    }
    Tile *t = gTm->GetTile(this->pos);
    int sx = t->GetTileCoord().x;
    int sy = t->GetTileCoord().y;
    Tile *dstt = gTm->GetTile(dst);
    int dx = dstt->GetTileCoord().x;
    int dy = dstt->GetTileCoord().y;
    dstPt = dst;
    ret = pf->PathFind(sx, sy, dx, dy, path);

    if (!ret && path.size() > 0) {
        curTask = gEm->AddTask((new TaskPeriodic(
                        (TaskPeriodicCb)&TransformableObject::ScheduleTranslate,
                        (void *)this, 50 /* 50 miliseconds */)));
        printf("[%p] add task %d\n", this, curTask);
    }
};

void DrawAll(sf::RenderWindow *window)
{
    for (list<DrawableObject *>::iterator it = gDobjs.begin(); it != gDobjs.end(); it++) {
        (*it)->ScheduleDraw(window);
    }
}
