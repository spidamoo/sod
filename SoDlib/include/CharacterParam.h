#ifndef CHARACTERPARAM_H
#define CHARACTERPARAM_H

#include <SoDlib.h>

class CharacterParam
{
    public:
        CharacterParam(Character* owner, CharacterParamPrototype* prototype);
        virtual ~CharacterParam();

        float getNormalValue(), getCurrentValue();
        void update();
    protected:
        Character* owner;
        CharacterParamPrototype* prototype;
        float normalValue, currentValue;
    private:
};

#endif // CHARACTERPARAM_H
