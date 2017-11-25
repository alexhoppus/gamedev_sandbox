#ifndef TILEMAP_DEFS
#define TILEMAP_DEFS

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <stdio.h>
#include <mutex>

#include "object.h"

using namespace sf;
using namespace std;

#define MAP_X 64
#define MAP_Y 64

typedef Vector2i TileCoord;

class DrawableObject;


#ifdef DEBUG
extern Animator *gNoBlockAnim;
extern Animator *gBlockAnim;
#endif

struct Tile : public DrawableObject {
    bool  occupied;
    float w, h;
    TileCoord tc;

    void SetOccupied(bool state)
    {
        occupied = state;
#ifdef DEBUG
        if (occupied)
            an = gBlockAnim;
        else
            an = gNoBlockAnim;
#endif
    }

    bool GetOccupied(void)
    {
        return occupied;
    }


    TileCoord GetTileCoord(void)
    {
        return tc;
    }

    Tile(Animator *a, ScreenCoord pos, Vector2f size, bool occ, TileCoord tp) :
        DrawableObject(a, pos, Vector2f(1.0, 1.0)), occupied(occ), tc(tp)
    {
        w = size.x;
        h = size.y;
        sprite.scale(w / sprite.getGlobalBounds().width,
                h / sprite.getGlobalBounds().height);
    };

    ~Tile()
    {
//        printf("delte tile %d %d\n", tc.x, tc.y);
    }
};

/*   map   x                         screen view
 * ------------------                     .
 *| 0,0 | 1,0 | 2,0 |                   .0,0.
 *|------------------                 .0,1|1,0.
 *| 0,1 | 1,1 | 2,1 | y    ------>  .0,2|1,1|2,0.
 *|------------------                 .1,2|2,1.
  | 0,2 | 1,2 | 2,2 |                   .2,2.
  -------------------                     .
 * */


class TileMap {
private:
    Tile *map[MAP_X][MAP_Y];
    int tW, tH;

    void AddTile(Animator *a, TileCoord tc, bool occupied)
    {
        if (map[tc.x][tc.y]) {
            delete(map[tc.x][tc.y]);
            map[tc.x][tc.y] = NULL;
        }

        map[tc.x][tc.y] = new Tile(a, ScreenCoord(tc.x * tW / 2 - tc.y * tW / 2,
                                         tc.x * tH / 2 + tc.y * tH / 2),
                             Vector2f(tW, tH), occupied, tc);
//        printf("add tile %d %d [%d]\n", tc.x, tc.y, map[tc.x][tc.y]->id);
    }
public:
    sf::Rect<int> bounds;

    Tile *GetTile(ScreenCoord sc)
    {
        TileCoord tc = TileCoord((sc.x / (tW / 2) + (sc.y + tH / 2) / (tH / 2)) / 2,
                                 (((sc.y + tH / 2) + 0) / (tH / 2) - sc.x / (tW / 2)) / 2);
        return map[tc.x][tc.y];
    }

    Tile *GetTile(TileCoord tc)
    {
        return map[tc.x][tc.y];
    }

    void GenerateTileArea(Animator *a, int x, int y, int w, int h, bool occupied)
    {
        for (int i = x; i < x + w; i++) {
            for (int j = y; j < y + h; j++) {
                AddTile(a, TileCoord(i, j), occupied);
            }
        }
    }
    TileMap(int tileWidth, int tileHeight) : tW(tileWidth), tH(tileHeight),
        bounds(0, 0, MAP_X, MAP_Y)
    {
        memset(map, 0x0, MAP_X*MAP_Y*sizeof(Tile *));
    };
};

#endif
