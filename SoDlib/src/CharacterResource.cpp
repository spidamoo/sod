#include "CharacterResource.h"

CharacterResource::CharacterResource(Character* _owner, CharacterResourcePrototype* _prototype) {
    owner = _owner;
    prototype = _prototype;

    fullValue = prototype->getDefaultFullValue();
    currentValue = fullValue;
    currentBoost = 0.0f;
    normalRegen = prototype->getDefaultRegen();
    currentRegen = normalRegen;

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
}
void CharacterResource::add(float value) {
    currentValue += value;
    if (currentValue > fullValue) {
        currentValue = fullValue;
    }
    /// TODO: события
}
