#ifndef MAPANIMATION_H
#define MAPANIMATION_H

#include <SoDlib.h>

class MapAnimation
{
    public:
        MapAnimation(Game* game, hgeAnimation* anim, float x, float y, float angle);
        virtual ~MapAnimation();

        void draw(bool schematicMode);
    protected:
        float x, y, angle;
        hgeAnimation* animation;
        Game* game;
    private:
};

#endif // MAPANIMATION_H
