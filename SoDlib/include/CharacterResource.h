#ifndef CHARACTERRESOURCE_H
#define CHARACTERRESOURCE_H

#include <SoDlib.h>

class CharacterResource
{
    public:
        CharacterResource(Character* owner, CharacterResourcePrototype* prototype);
        virtual ~CharacterResource();

        float getFullValue(), getCurrentValue(), getCurrentBoost(), getNormalRegen(), getCurrentRegen();
        void update(); void regen(float dt); void add(float value);
    protected:
        Character* owner;
        CharacterResourcePrototype* prototype;
        float fullValue, currentValue, currentBoost, normalRegen, currentRegen;
    private:
};

#endif // CHARACTERRESOURCE_H
