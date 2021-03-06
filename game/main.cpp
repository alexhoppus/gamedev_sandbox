#include <stdio.h>
#include <iostream>
#include <list>
#include <cmath>
#include <unistd.h>
#include <set>

#include <time.h>
#include <stdlib.h>

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

sf::Clock gClock;

#define DEBUG 1

#ifdef DEBUG
    Animator *gNoBlockAnim;
    Animator *gBlockAnim;
#endif

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    gEm = EventManager::getInstance();
    gTm = new TileMap(48, 24);
    srand(time(NULL));
    gClock.restart();

    sf::View view = window.getView();
    window.setView(view);

    std::map<string, AnimatorResource> ar;
    LoadAnimLoop("./resources/base.anim", ar);
    {
        Animator *nonblock = new Animator(&ar["grass-terrain-debug"]);
        Animator *block = new Animator(&ar["forbid"]);
#ifdef DEBUG
        gBlockAnim = block;
        gNoBlockAnim = nonblock;
#endif

        gTm->GenerateTileArea(nonblock, 0, 0, 64, 64, false);
        gTm->GenerateTileArea(block, 5, 5, 1, 2, true);
        gTm->GenerateTileArea(block, 10, 7, 3, 1, true);
        gTm->GenerateTileArea(block, 0, 7, 2, 1, true);
        gTm->GenerateTileArea(block, 7, 3, 9, 1, true);
        gTm->GenerateTileArea(block, 7, 36, 2, 13, true);
        gTm->GenerateTileArea(block, 10, 23, 2, 13, true);
        gTm->GenerateTileArea(block, 10, 3, 3, 23, true);
    }
    TransformableObject obj1(&ar["soldier1"], TileCoord(4, 1), sf::Vector2f(1.0, 1.0),
            2.0);
    TransformableObject obj2(&ar["soldier1"], TileCoord(5, 7), sf::Vector2f(1.0, 1.0),
            2.0);
    TransformableObject obj3(&ar["soldier1"], TileCoord(1, 7), sf::Vector2f(1.0, 1.0),
            2.0);
    TransformableObject obj4(&ar["soldier1"], TileCoord(2, 3), sf::Vector2f(1.0, 1.0),
            2.0);
    TransformableObject obj5(&ar["soldier1"], TileCoord(4, 2), sf::Vector2f(1.0, 1.0),
            2.0);
    TransformableObject obj6(&ar["soldier1"], TileCoord(5, 8), sf::Vector2f(1.0, 1.0),
            2.0);
    TransformableObject obj7(&ar["soldier1"], TileCoord(2, 7), sf::Vector2f(1.0, 1.0),
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
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                // convert it to world coordinates
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

                obj1.MoveTo(ScreenCoord(worldPos.x, worldPos.y));
                obj2.MoveTo(ScreenCoord(worldPos.x, worldPos.y));
                obj3.MoveTo(ScreenCoord(worldPos.x, worldPos.y));
                obj4.MoveTo(ScreenCoord(worldPos.x, worldPos.y));
                obj5.MoveTo(ScreenCoord(worldPos.x, worldPos.y));
                obj6.MoveTo(ScreenCoord(worldPos.x, worldPos.y));
                obj7.MoveTo(ScreenCoord(worldPos.x, worldPos.y));

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
