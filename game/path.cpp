#include <iostream>
#include <list>
#include <assert.h>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include "animator.h"
#include "path.h"

struct MovementQuant {
    int xR;
    int yR;
    int cost;
    DirId direction;
};

MovementQuant mq[8] = {
    {-1, -1, 20, DIR_270},
    {0, -1,  10, DIR_315},
    {1, -1,  20, DIR_0},
    {1,  0,  10, DIR_45},
    {1,  1,  20, DIR_90},
    {0, 1,  10, DIR_135},
    {-1, 1,  20, DIR_180},
    {-1, 0,  10, DIR_225}
};

int PathFinder::PathFind(int sx, int sy, int dstx, int dsty, list<Tile *> &lpath)
{
    memset(visited, 0x0, tm->bounds.height * tm->bounds.width);
    for (int i = 0; i < tm->bounds.height * tm->bounds.width; i++)
        distMap[i] = INT_MAX;

    Tile *sTile = tm->GetTile(TileCoord(sx, sy));
    Tile *dTile = tm->GetTile(TileCoord(dstx, dsty));
    if (sTile->occupied || dTile->occupied)
        return -1;

    SetDistance(sx, sy, 0);

    l.push_back(sTile);
    SetVisited(sTile->GetTileCoord().x, sTile->GetTileCoord().y);

    while(l.size() != 0) {
        list<Tile *>::iterator res = GetClosestTile();
        Tile *curTile = *res;

        TileCoord c = curTile->GetTileCoord();
        if (curTile == dTile) {
            Tile *nextTile = dTile;
            while (nextTile != sTile) {
                printf("%d %d\n", nextTile->GetTileCoord().x,
                                  nextTile->GetTileCoord().y);
                lpath.push_front(nextTile);
                nextTile = GetParent(nextTile->GetTileCoord().x,
                                     nextTile->GetTileCoord().y);
            }
            return 0;
        }
        l.erase(res);

        for (int i = 0; i < 8; i++) {
            int x_i = c.x + mq[i].xR;
            int y_i = c.y + mq[i].yR;
            int cost = mq[i].cost;
            if (tm->bounds.contains(x_i, y_i)) {
                Tile *t;
                int curDist, proposedDist;

                t = tm->GetTile(TileCoord(x_i, y_i));
                if (!t->occupied && !IsVisited(x_i, y_i)) {
                    l.push_back(t);
                    SetVisited(t->GetTileCoord().x, t->GetTileCoord().y);
                }

                curDist = GetDistance(x_i, y_i);
                proposedDist = GetDistance(c.x, c.y) + cost;
                if (curDist > proposedDist) {
#if DEBUG
                    printf("%d %d set parent %d %d\n",
                            x_i, y_i, curTile->GetTileCoord().x, curTile->GetTileCoord().y);
#endif
                    SetDistance(x_i, y_i, proposedDist);
                    SetParent(x_i, y_i, curTile);
                }
#if DEBUG
                printf("we are %d %d consider %d %d cur %d prop %d\n",
                        curTile->GetTileCoord().x, curTile->GetTileCoord().y, x_i, y_i,
                        curDist, proposedDist);
#endif
            }
        }
    }
    return -1;
}

