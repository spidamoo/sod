#include "Condition.h"

Condition::Condition(ConditionPrototype* prototype, Character* inflictor, Character* bearer)
{
    this->prototype = prototype;
    this->inflictor = inflictor;
    this->bearer    = bearer;

    totalTime = 0.0f;
    tickTime = 0.0f;
}

Condition::~Condition()
{
    //dtor
}

void Condition::initialize()
{
    prototype->setParamsFromCharacter(inflictor);

    totalTime = prototype->getDuration();
    interval  = prototype->getInterval();
}

void Condition::update(float dt)
{
    totalTime -= dt;
    tickTime += dt;

    prototype->setParamsFromCharacter(inflictor);

    while ( tickTime >= interval ) {
        switch ( prototype->getType() ) {
            case CONDITION_TYPE_DAMAGE:
                bearer->doDamage( prototype->getValue() );
                break;
        }

        tickTime -= interval;
        if (interval <= 0) {
            break;
        }
    }
}

float Condition::getTime()
{
    return totalTime;
}
