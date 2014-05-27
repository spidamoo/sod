#ifndef CONDITION_H
#define CONDITION_H

#include <SoDlib.h>

class Condition
{
    public:
        Condition(ConditionPrototype* prototype, Character* inflictor, Character* bearer);
        virtual ~Condition();

        void initialize();

        void update(float dt);

        ConditionPrototype* getPrototype();
        float getTime(); void setTime(float time);
        float getValue();
    protected:
        ConditionPrototype* prototype;
        Character* inflictor; Character* bearer;

        float totalTime, tickTime, interval, value;
    private:
};

#endif // CONDITION_H
