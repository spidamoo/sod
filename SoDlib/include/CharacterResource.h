#ifndef CHARACTERRESOURCE_H
#define CHARACTERRESOURCE_H

#include <SoDlib.h>

class CharacterResource
{
    public:
        CharacterResource(Game* game, Character* owner, CharacterResourcePrototype* prototype);
        virtual ~CharacterResource();

        float getFullValue(), getCurrentValue(), getCurrentBoost(), getNormalRegen(), getCurrentRegen();
        void update(); void regen(float dt); void add(float value);

        void setLessEvent(float value, CharacterAction* action); void setMoreEvent(float value, CharacterAction* action);
        void dispatchLessEvent(); void dispatchMoreEvent();
        void findNextLessEvent(), findPrevLessEvent(); void findNextMoreEvent(), findPrevMoreEvent();
        void findCurrentLessEvents(); void findCurrentMoreEvents();
        void checkEvents();
    protected:
        Game* game;
        Character* owner;
        CharacterResourcePrototype* prototype;
        float fullValue, currentValue, currentBoost, normalRegen, currentRegen;
        float* lessEventValues; CharacterAction** lessEventActions; int lessEventsCount;
        float* moreEventValues; CharacterAction** moreEventActions; int moreEventsCount;
        int nextLessEvent, prevLessEvent, nextMoreEvent, prevMoreEvent;
    private:
};

#endif // CHARACTERRESOURCE_H
