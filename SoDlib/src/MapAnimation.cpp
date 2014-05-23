#include "MapAnimation.h"

MapAnimation::MapAnimation(Game* game, hgeAnimation* anim, float x, float y, float angle)
{
    this->game = game;
    this->animation = anim;
    this->x = x;
    this->y = y;
    this->angle = angle;
}

MapAnimation::~MapAnimation()
{
    //dtor
}

void MapAnimation::draw(float ratio, bool schematicMode)
{
    animation->RenderEx(
        game->screenX(x, ratio),
        game->screenY(y, ratio),
        angle,
        game->getScaleFactor(),
        game->getScaleFactor()
    );
}

void MapAnimation::move(float dx, float dy)
{
    x += dx; y += dy;
}
