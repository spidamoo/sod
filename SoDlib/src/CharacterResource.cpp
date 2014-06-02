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

    barWidth = 2.0f;
    barHeight = 0.2f;
    barPositionX = 0.0f;
    barPositionY = -1.5f;
    barFullColor = 0xFFFFFFFF;
    barEmptyColor = 0xFF000000;
    barBoostColor = 0xFF0000FF;
    barFrameColor = 0xFF000000;

    barHalfWidth = barWidth * 0.5f;
    barHalfHeight = barHeight * 0.5f;

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

    if (currentRegen < 0.0f)
        currentRegen = 0.0f;
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
void CharacterResource::set(float value) {
    currentValue = value;
    if (currentValue > fullValue) {
        currentValue = fullValue;
    }
    checkEvents();
}

void CharacterResource::draw(bool schematicMode, int index) {
    if (schematicMode) {
        char buffer[64];
        sprintf(
            buffer,
            "%s: %.2f/%.2f",
            prototype->getName(),
            getCurrentValue(),
            getFullValue()
        );
        game->drawText(
            game->screenX(owner->getX()),
            game->screenY(owner->getY() - owner->getHalfHeight()) - 30.0f - 30.0f * index,
            buffer
        );
    }
    else {
        game->drawRect(
            game->screenX(owner->getX() + barPositionX - barHalfWidth) - 1.0f,
            game->screenY(owner->getY() + barPositionY - barHalfHeight) - 1.0f,
            game->screenX(owner->getX() + barPositionX + barHalfWidth) + 1.0f,
            game->screenY(owner->getY() + barPositionY + barHalfHeight) + 1.0f,
            0, barFrameColor
        );
        game->drawRect(
            game->screenX(owner->getX() + barPositionX - barHalfWidth),
            game->screenY(owner->getY() + barPositionY - barHalfHeight),
            game->screenX(owner->getX() + barPositionX + barHalfWidth),
            game->screenY(owner->getY() + barPositionY + barHalfHeight),
            0, barEmptyColor
        );
        float currentWidth = barWidth * currentValue / (fullValue + currentBoost);
        if (currentWidth < 0.0f)
            currentWidth = 0.0f;
        float boostWidth = barWidth * currentBoost / (fullValue + currentBoost);
        if (boostWidth < 0.0f)
            boostWidth = 0.0f;
        game->drawRect(
            game->screenX(owner->getX() + barPositionX - barHalfWidth),
            game->screenY(owner->getY() + barPositionY - barHalfHeight),
            game->screenX(owner->getX() + barPositionX - barHalfWidth + currentWidth),
            game->screenY(owner->getY() + barPositionY + barHalfHeight),
            0, barFullColor
        );
        game->drawRect(
            game->screenX(owner->getX() + barPositionX - barHalfWidth + currentWidth),
            game->screenY(owner->getY() + barPositionY - barHalfHeight),
            game->screenX(owner->getX() + barPositionX - barHalfWidth + currentWidth + boostWidth),
            game->screenY(owner->getY() + barPositionY + barHalfHeight),
            0, barBoostColor
        );
    }
}

void CharacterResource::setLessEvent(float value, CharacterAction* action) {
    for (int i = 0; i < lessEventsCount; i++) {
        if (lessEventValues[i] == value) {
            lessEventActions[i] = action;
//            printf("set action #%d on value %f\n", i, lessEventValues[i]);
            return;
        }
    }
//    printf("add action #%d on value %f\n", lessEventsCount, value);
    lessEventValues[lessEventsCount] = value;
    lessEventActions[lessEventsCount] = action;
    lessEventsCount++;

    findCurrentLessEvents();
}
void CharacterResource::setMoreEvent(float value, CharacterAction* action) {
    for (int i = 0; i < moreEventsCount; i++) {
        if (moreEventValues[i] == value) {
            moreEventActions[i] = action;
            return;
        }
    }
    moreEventValues[moreEventsCount] = value;
    moreEventActions[moreEventsCount] = action;
    moreEventsCount++;

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
//    printf("now: %.2f/%.2f: next: %d prev: %d\n", (currentValue + currentBoost), (fullValue + currentBoost), nextLessEvent, prevLessEvent);
    if ( nextLessEvent != -1 && (currentValue + currentBoost) / (fullValue + currentBoost) <= lessEventValues[nextLessEvent] ) {
        dispatchLessEvent();
        findNextLessEvent();
    }
    else if ( prevLessEvent != -1 &&  (currentValue + currentBoost) / (fullValue + currentBoost) >= lessEventValues[prevLessEvent] ) {
        findPrevLessEvent();
    }

    if ( nextMoreEvent != -1 &&  (currentValue + currentBoost) / (fullValue + currentBoost) >= moreEventValues[nextMoreEvent] ) {
        dispatchMoreEvent();
        findNextMoreEvent();
    }
    else if ( prevMoreEvent != -1 && (currentValue + currentBoost) / (fullValue + currentBoost) <= moreEventValues[prevMoreEvent] ) {
        findPrevMoreEvent();
    }
}

void CharacterResource::loadBarParamsFromXml(TiXmlElement* xml) {
    if ( xml->Attribute("width") )
        barWidth = atof( xml->Attribute("width") );
    if ( xml->Attribute("height") )
        barHeight = atof( xml->Attribute("height") );
    if ( xml->Attribute("x") )
        barPositionX = atof( xml->Attribute("x") );
    if ( xml->Attribute("y") )
        barPositionY = atof( xml->Attribute("y") );

    if ( xml->Attribute("full_color") )
        barFullColor = atoi( xml->Attribute("full_color") );
    if ( xml->Attribute("empty_color") )
        barEmptyColor = atoi( xml->Attribute("empty_color") );
    if ( xml->Attribute("boost_color") )
        barBoostColor = atoi( xml->Attribute("boost_color") );
    if ( xml->Attribute("frame_color") )
        barFrameColor = atoi( xml->Attribute("frame_color") );

    barHalfWidth = barWidth * 0.5f;
    barHalfHeight = barHeight * 0.5f;
}
