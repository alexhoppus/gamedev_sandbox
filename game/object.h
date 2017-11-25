#ifndef OBJ_DEFS
#define OBJ_DEFS

#include <assert.h>
#include <mutex>
#include <list>
#include "animator.h"
#include "math.h"

using namespace sf;
using namespace std;

typedef Vector2f ScreenCoord;
typedef Vector2i TileCoord;

class Tile;
class PathFinder;

class Object {
public:
    int id;
    list<Object *>::iterator thisObj;

    Object();
    ~Object();
};

class DrawableObject : public Object {
private:
    Vector2f                         scale;
    list<DrawableObject *>::iterator thisObj;
protected:
    Sprite    sprite;
public:
    Animator     *an;
    ScreenCoord  pos;
    ScreenCoord GetScreenCoord(void) { return pos;};

    DrawableObject(Animator *a, ScreenCoord p, Vector2f s);
    ~DrawableObject();

    int ScheduleDraw(sf::RenderWindow *window);
};

class TransformableObject : public DrawableObject {
private:
    float         velocity;
    list<Tile *>  path;
    int           curTask;
    PathFinder   *pf;

    DirId ComputeDirection(Vector2f curDir);
    bool ScheduleTranslate(void);
public:
    TransformableObject(AnimatorResource *ar, TileCoord p, Vector2f s, PathFinder *pf,
            float v);
    void MoveTo(TileCoord dst);
};

void DrawAll(sf::RenderWindow *window);
#endif
