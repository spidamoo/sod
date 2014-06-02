#ifndef CHARACTERRESOURCE_H
#define CHARACTERRESOURCE_H

#include <SoDlib.h>

class CharacterResource
{
    public:
        CharacterResource(Game* game, Character* owner, CharacterResourcePrototype* prototype);
        virtual ~CharacterResource();

        float getFullValue(), getCurrentValue(), getCurrentBoost(), getNormalRegen(), getCurrentRegen();
        void update(); void regen(float dt); void add(float value), set(float value);
        void draw(bool schematicMode, int index);

        void setLessEvent(float value, CharacterAction* action); void setMoreEvent(float value, CharacterAction* action);
        void dispatchLessEvent(); void dispatchMoreEvent();
        void findNextLessEvent(), findPrevLessEvent(); void findNextMoreEvent(), findPrevMoreEvent();
        void findCurrentLessEvents(); void findCurrentMoreEvents();
        void checkEvents();

        void loadBarParamsFromXml(TiXmlElement* xml);
    protected:
        Game* game;
        Character* owner;
        CharacterResourcePrototype* prototype;
        float fullValue, currentValue, currentBoost, normalRegen, currentRegen;
        float* lessEventValues; CharacterAction** lessEventActions; int lessEventsCount;
        float* moreEventValues; CharacterAction** moreEventActions; int moreEventsCount;
        int nextLessEvent, prevLessEvent, nextMoreEvent, prevMoreEvent;

        float barWidth, barHalfWidth;
        float barHeight, barHalfHeight;
        float barPositionX;
        float barPositionY;
        DWORD barEmptyColor;
        DWORD barFullColor;
        DWORD barBoostColor;
        DWORD barFrameColor;
    private:
};

#endif // CHARACTERRESOURCE_H
