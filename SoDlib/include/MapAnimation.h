#ifndef MAPANIMATION_H
#define MAPANIMATION_H

#include <SoDlib.h>

class MapAnimation
{
    public:
        MapAnimation(Game* game);
        virtual ~MapAnimation();

        void loadFromXml(TiXmlElement* xml);

        void draw(float ratio, bool schematicMode);
        void move(float dx, float dy);
    protected:
        float x, y, angle, scaleX, scaleY;
        hgeAnimation* animation;
        Game* game;
    private:
};

#endif // MAPANIMATION_H
