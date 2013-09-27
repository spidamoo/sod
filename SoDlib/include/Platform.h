#ifndef PLATFORM_H
#define PLATFORM_H

#include <SoDlib.h>

class Platform
{
    public:
        Platform(Game* game, TiXmlElement* elem);
        virtual ~Platform();

        void update(float dt);

        bool characterOn(int charIndex);
    protected:
        Game* game;
        int groundLinesCount, animationsCount;
        int* groundLines; int *animations;
        int spotsCount;
        float* spotX; float* spotY; float* spotAngle; float* spotTime;

        float currentTime; float currentSpotStart; int currentSpot;
        float currentX, currentY;
    private:
};

#endif // PLATFORM_H
