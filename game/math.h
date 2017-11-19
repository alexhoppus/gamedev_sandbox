
#ifndef MATH_DEFS
#define MATH_DEFS

#include <SFML/Graphics.hpp>

using namespace sf;

static inline bool isInsideArea(Vector2f center, Vector2f pt, float radius)
{
    return ((pt.x - center.x) * (pt.x - center.x) + (pt.y - center.y) *
            (pt.y - center.y) < radius);
}

static inline float Norm(sf::Vector2f vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

static inline float Dot(sf::Vector2f vec1, sf::Vector2f vec2)
{
    return vec1.x * vec2.x + vec1.y * vec2.y;
}

#endif
