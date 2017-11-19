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

bool TransformableObject::ScheduleTranslate(void)
{
    if (!path.empty()) {
        TileMap *tm = pf->GetBaseTileMap();
        Tile *dstTile = path.front();
        Tile *curTile = tm->GetTile(this->GetScreenCoord());

        an->SetAnim(ANIM_WALK);

        ScreenCoord dst = dstTile->GetScreenCoord();
        if (!isInsideArea(dst, this->GetScreenCoord(), velocity * 1)) {
            Vector2f vector = (dst - this->GetScreenCoord()) /
                Norm(dst - this->GetScreenCoord());
            DirId dId = ComputeDirection(vector);
            an->SetDir(dId);

            pos.x += vector.x * velocity;
            pos.y += vector.y * velocity;
        } else {
            path.pop_front();
        }
        return false;
    }
    an->SetAnim(ANIM_IDLE);
    curTask = -1;
    return true;
}

TransformableObject::TransformableObject(Animator *a, TileCoord p, Vector2f s,
        PathFinder *_pf, float v) :
        DrawableObject(a, Vector2f(_pf->GetBaseTileMap()->GetTile(p)->pos), s),
        velocity(v), pf(_pf)
{
    gTobjs.push_back(this);
    curTask = -1;
};

void TransformableObject::MoveTo(TileCoord dst)
{
    int ret;
    if (curTask != -1) {
        gEm->RemoveTask(curTask);
        path.resize(0);
        an->SetAnim(ANIM_IDLE);
    }
    TileMap *tm = pf->GetBaseTileMap();
    Tile *t = tm->GetTile(this->GetScreenCoord());
    int sx = t->GetTileCoord().x;
    int sy = t->GetTileCoord().y;
    ret = pf->PathFind(sx, sy, dst.x, dst.y, path);

    if (!ret && path.size() > 0) {
        curTask = gEm->AddTask((new TaskPeriodic(
                        (TaskPeriodicCb)&TransformableObject::ScheduleTranslate,
                        (void *)this, 50 /* 50 miliseconds */)));
    }
};

void DrawAll(sf::RenderWindow *window)
{
    for (list<DrawableObject *>::iterator it = gDobjs.begin(); it != gDobjs.end(); it++) {
        (*it)->ScheduleDraw(window);
    }
}
