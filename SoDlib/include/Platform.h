#ifndef PLATFORM_H
#define PLATFORM_H

#include <SoDlib.h>

class Platform
{
    public:
        Platform(Game* game, TiXmlElement* elem);
        virtual ~Platform();
    protected:
        Game* game;
        int groundLinesCount, animationsCount;
        int* groundLines; int *animations;
        int spotsCount;
        float* spotX; float* spotY; float*spotAngle; float*spotTime;
    private:
};

#endif // PLATFORM_H
