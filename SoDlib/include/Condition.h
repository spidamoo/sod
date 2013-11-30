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

        float getTime();
    protected:
        ConditionPrototype* prototype;
        Character* inflictor; Character* bearer;

        float totalTime, tickTime, interval;
    private:
};

#endif // CONDITION_H
