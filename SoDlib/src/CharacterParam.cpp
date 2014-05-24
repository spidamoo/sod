#include "CharacterParam.h"

CharacterParam::CharacterParam(Character* _owner, CharacterParamPrototype* _prototype) {
    owner = _owner;
    prototype = _prototype;

    normalValue = prototype->getDefaultNormalValue();

    update();
}

CharacterParam::~CharacterParam() {
    //dtor
}

float CharacterParam::getNormalValue() {
    return normalValue;
}
float CharacterParam::getCurrentValue() {
    return currentValue;
}

void CharacterParam::update() {
    currentValue = normalValue;
    for (int i = 0; i < owner->getConditionsCount(); i++) {
        if (owner->getCondition(i)->getPrototype()->getType() == CONDITION_TYPE_ADD_PARAM) {
            currentValue += owner->getCondition(i)->getValue();
        }
    }
}
