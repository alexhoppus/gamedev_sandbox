#include <stdio.h>
#include <iostream>
#include <list>
#include <cmath>
#include <unistd.h>
#include <set>

#include <time.h>

#include "animator.h"
#include "object.h"
#include "event.h"
#include "common.h"
#include "tilemap.h"
#include "path.h"

using namespace sf;
using namespace std;

EventManager                     *gEm;
TileMap                          *gTm;
PathFinder                       *gPf;

sf::Clock gClock;

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    gEm = EventManager::getInstance();
    gTm = new TileMap(50, 25);
    gPf = new PathFinder(gTm);
    gClock.restart();

//    sf::View view(sf::FloatRect(0, 0, 1500, 1500));
    // activate it
//    window.setView(view);

    sf::View view = window.getView();
    window.setView(view);


    std::map<string, AnimatorResource> ar;
    LoadAnimLoop("./resources/base.anim", ar);
    {
        Animator *nonblock = new Animator(&ar["grass-terrain-debug"]);
        Animator *block = new Animator(&ar["forbid"]);
        gTm->GenerateTileArea(nonblock, 0, 0, 64, 64, false);
        gTm->GenerateTileArea(block, 5, 5, 1, 2, true);
        gTm->GenerateTileArea(block, 10, 7, 3, 1, true);
        gTm->GenerateTileArea(block, 0, 7, 2, 1, true);
        gTm->GenerateTileArea(block, 7, 3, 9, 1, true);
        gTm->GenerateTileArea(block, 7, 36, 2, 13, true);
        gTm->GenerateTileArea(block, 10, 23, 2, 13, true);
        gTm->GenerateTileArea(block, 10, 3, 3, 23, true);
    }
    TransformableObject obj1(&ar["soldier1"], TileCoord(4, 1), sf::Vector2f(1.0, 1.0), gPf,
            2.0);
    TransformableObject obj2(&ar["soldier1"], TileCoord(5, 7), sf::Vector2f(1.0, 1.0), gPf,
            2.0);

    while (window.isOpen())
    {
        sf::Event event;
        window.clear();

        DrawAll(&window);
        while (window.pollEvent(event))
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                //float Mx = (float) event.mouseButton.x;
                //float My = (float) event.mouseButton.y;
                //printf("%d %d\n", event.mouseButton.x, event.mouseButton.y);

                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                // convert it to world coordinates
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

                Tile *t = gTm->GetTile(ScreenCoord(worldPos.x, worldPos.y));
                printf("tile (%f %f)\n", t->GetScreenCoord().x,
                        t->GetScreenCoord().y);
                obj1.MoveTo(t->GetTileCoord());
                obj2.MoveTo(t->GetTileCoord());
            }
            if (event.type == sf::Event::KeyPressed)
            {
                sf::View view = window.getView();
                if (event.key.code == sf::Keyboard::Left) {
                    view.move(Vector2f(-10, 0));
                } else if (event.key.code == sf::Keyboard::Right) {
                    view.move(Vector2f(10, 0));
                } else if (event.key.code == sf::Keyboard::Up) {
                    view.move(Vector2f(0, -10));
                } else if (event.key.code == sf::Keyboard::Down) {
                    view.move(Vector2f(0, 10));
                }
                window.setView(view);

            }
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.display();
    }

    return 0;
}
