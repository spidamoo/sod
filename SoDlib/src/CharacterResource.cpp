#include "CharacterResource.h"

CharacterResource::CharacterResource(Game* _game, Character* _owner, CharacterResourcePrototype* _prototype) {
    game = _game;
    owner = _owner;
    prototype = _prototype;

    fullValue = prototype->getDefaultFullValue();
    currentValue = fullValue;
    currentBoost = 0.0f;
    normalRegen = prototype->getDefaultRegen();
    currentRegen = normalRegen;

    lessEventsCount = 0;
    moreEventsCount = 0;

    lessEventActions = new CharacterAction*[16];
    lessEventValues = new float[16];

    moreEventActions = new CharacterAction*[16];
    moreEventValues = new float[16];

    nextLessEvent = -1;
    prevLessEvent = -1;
    nextMoreEvent = -1;
    prevMoreEvent = -1;

    update();
}

CharacterResource::~CharacterResource() {

}

float CharacterResource::getFullValue() {
    return fullValue;
}
float CharacterResource::getCurrentValue() {
    return currentValue + currentBoost;
}
float CharacterResource::getCurrentBoost() {
    return currentBoost;
}
float CharacterResource::getNormalRegen() {
    return normalRegen;
}
float CharacterResource::getCurrentRegen() {
    return currentRegen;
}

void CharacterResource::update() {
    currentRegen = normalRegen;
    currentBoost = 0.0f;
    for (int i = 0; i < owner->getConditionsCount(); i++) {
        if (owner->getCondition(i)->getPrototype()->getType() == CONDITION_TYPE_REGEN_RESOURCE) {
            currentRegen += owner->getCondition(i)->getValue();
        }
        if (owner->getCondition(i)->getPrototype()->getType() == CONDITION_TYPE_BOOST_RESOURCE) {
            currentBoost += owner->getCondition(i)->getValue();
        }
    }
}
void CharacterResource::regen(float dt) {
    currentValue += currentRegen * dt;
    if (currentValue > fullValue)
        currentValue = fullValue;

    checkEvents();
}
void CharacterResource::add(float value) {
    currentValue += value;
    if (currentValue > fullValue) {
        currentValue = fullValue;
    }
    checkEvents();
}

void CharacterResource::setLessEvent(float value, CharacterAction* action) {
    for (int i = 0; i < lessEventsCount; i++) {
        if (lessEventValues[i] == value) {
            lessEventActions[i] = action;
            return;
        }
    }
    lessEventValues[lessEventsCount] = value;
    lessEventActions[lessEventsCount] = action;

    findCurrentLessEvents();
}
void CharacterResource::setMoreEvent(float value, CharacterAction* action) {
    for (int i = 0; i < moreEventsCount; i++) {
        if (moreEventValues[i] == value) {
            moreEventActions[i] = action;
            return;
        }
    }
    moreEventValues[lessEventsCount] = value;
    moreEventActions[lessEventsCount] = action;
    findCurrentMoreEvents();
}

void CharacterResource::dispatchLessEvent() {
    lessEventActions[nextLessEvent]->perform(game, owner);
}
void CharacterResource::dispatchMoreEvent() {
    moreEventActions[nextMoreEvent]->perform(game, owner);
}

void CharacterResource::findNextLessEvent() {
    prevLessEvent = nextLessEvent;
    nextLessEvent = -1;
    float maxLessValue = -INFINITY;
    for (int i = 0; i < lessEventsCount; i++) {
        if (lessEventValues[i] < lessEventValues[nextLessEvent] && lessEventValues[i] > maxLessValue) {
            maxLessValue = lessEventValues[i];
            nextLessEvent = i;
        }
    }
    prevLessEvent = nextLessEvent;
}
void CharacterResource::findPrevLessEvent() {
    nextLessEvent = prevLessEvent;
    prevLessEvent = -1;
    float minLessValue = INFINITY;
    for (int i = 0; i < lessEventsCount; i++) {
        if (lessEventValues[i] > lessEventValues[nextLessEvent] && lessEventValues[i] < minLessValue) {
            minLessValue = lessEventValues[i];
            prevLessEvent = i;
        }
    }
}
void CharacterResource::findNextMoreEvent() {
    prevMoreEvent = nextMoreEvent;
    nextMoreEvent = -1;
    float minMoreValue = INFINITY;
    for (int i = 0; i < moreEventsCount; i++) {
        if (moreEventValues[i] > moreEventValues[nextMoreEvent] && moreEventValues[i] < minMoreValue) {
            minMoreValue = moreEventValues[i];
            nextMoreEvent = i;
        }
    }
}
void CharacterResource::findPrevMoreEvent() {
    nextMoreEvent = prevMoreEvent;
    prevMoreEvent = -1;
    float maxMoreValue = -INFINITY;
    for (int i = 0; i < moreEventsCount; i++) {
        if (moreEventValues[i] < moreEventValues[nextMoreEvent] && moreEventValues[i] > maxMoreValue) {
            maxMoreValue = moreEventValues[i];
            prevMoreEvent = i;
        }
    }
}

void CharacterResource::findCurrentLessEvents() {
    float maxLessValue = -INFINITY;
    for (int i = 0; i < lessEventsCount; i++) {
        if (lessEventValues[i] < (currentValue + currentBoost) / (fullValue + currentBoost) && lessEventValues[i] > maxLessValue) {
            maxLessValue = lessEventValues[i];
            nextLessEvent = i;
        }
    }
    float minLessValue = INFINITY;
    for (int i = 0; i < lessEventsCount; i++) {
        if (lessEventValues[i] > (currentValue + currentBoost) / (fullValue + currentBoost) && lessEventValues[i] < minLessValue) {
            minLessValue = lessEventValues[i];
            prevLessEvent = i;
        }
    }
}
void CharacterResource::findCurrentMoreEvents() {
    float minMoreValue = INFINITY;
    for (int i = 0; i < moreEventsCount; i++) {
        if (moreEventValues[i] > (currentValue + currentBoost) / (fullValue + currentBoost) && moreEventValues[i] < minMoreValue) {
            minMoreValue = moreEventValues[i];
            nextMoreEvent = i;
        }
    }
    float maxMoreValue = -INFINITY;
    for (int i = 0; i < moreEventsCount; i++) {
        if (moreEventValues[i] < (currentValue + currentBoost) / (fullValue + currentBoost) && moreEventValues[i] > maxMoreValue) {
            maxMoreValue = moreEventValues[i];
            prevMoreEvent = i;
        }
    }
}

void CharacterResource::checkEvents() {
    if ( nextLessEvent != -1 && (currentValue + currentBoost) / (fullValue + currentBoost) < lessEventValues[nextLessEvent] ) {
        dispatchLessEvent();
        findNextLessEvent();
    }
    else if ( prevLessEvent != -1 &&  (currentValue + currentBoost) / (fullValue + currentBoost) > lessEventValues[prevLessEvent] ) {
        findPrevLessEvent();
    }

    if ( nextMoreEvent != -1 &&  (currentValue + currentBoost) / (fullValue + currentBoost) > moreEventValues[nextMoreEvent] ) {
        dispatchMoreEvent();
        findNextMoreEvent();
    }
    else if ( prevMoreEvent != -1 && (currentValue + currentBoost) / (fullValue + currentBoost) < moreEventValues[prevMoreEvent] ) {
        findPrevMoreEvent();
    }
}
