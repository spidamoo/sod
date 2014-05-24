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
    prototype->setParamsFromCharacters(inflictor, bearer);

    totalTime = prototype->getDuration();
    interval  = prototype->getInterval();
    value = prototype->getValue();
}

void Condition::update(float dt)
{
    totalTime -= dt;
    tickTime += dt;

    prototype->setParamsFromCharacters(inflictor, bearer);

    while ( tickTime >= interval ) {
        value = prototype->getValue();
        switch ( prototype->getType() ) {
            case CONDITION_TYPE_ADD_RESOURCE:
                bearer->getResource( prototype->getParam() )->add( value );
                break;
        }

        tickTime -= interval;
        if (interval <= 0) {
            break;
        }
    }
}

ConditionPrototype* Condition::getPrototype() {
    return prototype;
}

float Condition::getTime() {
    return totalTime;
}

float Condition::getValue() {
    return value;
}
