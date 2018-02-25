#ifndef PATH_DEFS
#define PATH_DEFS

#include <iostream>
#include <list>
#include <cstdint>
#include <climits>
#include <stack>
#include "animator.h"
#include "object.h"
#include "tilemap.h"
#include "mutex"

using namespace std;
using namespace sf;

// Creating a shortcut for int, int pair type
typedef pair<int, int> Pair;

// Creating a shortcut for pair<int, pair<int, int>> type
typedef pair<double, pair<int, int>> pPair;

// A structure to hold the neccesary parameters
struct cell
{
    // Row and Column index of its parent
    // Note that 0 <= i <= MAP_Y-1 & 0 <= j <= MAP_X-1
    int parent_i, parent_j;
    // f = g + h
    double f, g, h;
};


class PathFinder {
private:
    TileMap *tm;

    /* TODO cache of PathFinders: keep one path finder per unit
     * is memory - expensive .
     * */
    bool closedList[MAP_Y][MAP_X];
    // Declare a 2D array of structure to hold the details
    //of that cell
    cell cellDetails[MAP_Y][MAP_X];

    bool isValid(int row, int col);
    bool isUnBlocked(int row, int col);
    bool isDestination(int row, int col, Pair dest);
    double calculateHValue(int row, int col, Pair dest);
    void tracePath(Pair dest, stack<Pair> &Path);
    bool aStarSearch(Pair src, Pair dest, stack<Pair> &Path);
public:
    int PathFind(int sx, int sy, int dstx, int dsty, list<Tile *> &lpath);
    PathFinder(TileMap *map) : tm(map)
    {
    };
    TileMap *GetBaseTileMap(void)
    {
        return tm;
    }
};

#endif
