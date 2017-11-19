#ifndef PATH_DEFS
#define PATH_DEFS

#include <iostream>
#include <list>
#include <cstdint>
#include <climits>
#include "animator.h"
#include "object.h"
#include "tilemap.h"

using namespace std;
using namespace sf;

class PathFinder {
private:
    TileMap *tm;
    list<Tile *> l;
    uint8_t *visited;
    int     *distMap;
    Tile   **parent;

    void SetVisited(int x, int y)
    {
        visited[x * tm->bounds.height + y] = 1;
    };
    bool IsVisited(int x, int y)
    {
        return (visited[x * tm->bounds.height + y] == 1);
    };
    int GetDistance(int x, int y)
    {
        return (distMap[x * tm->bounds.height + y]);
    };
    void SetDistance(int x, int y, int dist)
    {
        distMap[x * tm->bounds.height + y] = dist;
    };

    list<Tile *>::iterator GetClosestTile(void)
    {
        list<Tile *>::iterator ret;
        int minDist = INT_MAX;
        for (list<Tile *>::iterator it = l.begin(); it != l.end(); ++it) {
            int dist = GetDistance((*it)->GetTileCoord().x,
                                   (*it)->GetTileCoord().y);
            if (minDist > dist) {
                minDist = dist;
                ret = it;
            }
        }
        return ret;
    }
    Tile *GetParent(int x, int y)
    {
        return parent[x * tm->bounds.height + y];
    }
    void SetParent(int x, int y, Tile *par)
    {
        parent[x * tm->bounds.height + y] = par;
    }

public:
    int PathFind(int sx, int sy, int dstx, int dsty, list<Tile *> &lpath);
    PathFinder(TileMap *map) : tm(map)
    {
        int tileMapWidth = tm->bounds.height * tm->bounds.width;
        visited = new uint8_t[tileMapWidth];
        distMap = new int    [tileMapWidth];
        parent  = new Tile * [tileMapWidth];
    };
    TileMap *GetBaseTileMap(void)
    {
        return tm;
    }
};

#endif
