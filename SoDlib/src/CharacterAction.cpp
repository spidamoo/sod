#include "CharacterAction.h"

CharacterAction::CharacterAction() {
    causesCount = 0;
	causes = new CharacterActionCause*[causesCount];
	effectsCount = 0;
	effects = new CharacterActionEffect*[effectsCount];
}

CharacterAction::CharacterAction(TiXmlElement* elem) {
	TiXmlElement* causesRoot = elem->FirstChildElement("on");
	causesCount = atoi(causesRoot->Attribute("causes"));
	causes = new CharacterActionCause*[causesCount];
	int i = 0;
	TiXmlElement* causeElem = causesRoot->FirstChildElement("cause");
	while (causeElem) {
		causes[i] = new CharacterActionCause(causeElem);
		i++;
		causeElem = causeElem->NextSiblingElement("cause");
	}

	TiXmlElement* effectsRoot = elem->FirstChildElement("do");
	effectsCount = atoi(effectsRoot->Attribute("effects"));
	effects = new CharacterActionEffect*[effectsCount];
	i = 0;
	TiXmlElement* effectElem = effectsRoot->FirstChildElement("effect");
	while (effectElem) {
		effects[i] = new CharacterActionEffect(effectElem);
		i++;
		effectElem = effectElem->NextSiblingElement("effect");
	}
}

CharacterAction::~CharacterAction() {
    for (int i = 0; i < causesCount; i++) {
        delete causes[i];
    }
    delete causes;
    for (int i = 0; i < effectsCount; i++) {
        delete effects[i];
    }
    delete effects;
}

void CharacterAction::save(TiXmlElement* elem) {
	TiXmlElement* causesRoot = new TiXmlElement( "on" );
	causesRoot->SetAttribute("causes", causesCount);
	for (int i = 0; i < causesCount; i++) {
		TiXmlElement* causeElem = new TiXmlElement( "cause" );
		causes[i]->save(causeElem);
		causesRoot->LinkEndChild( causeElem );
	}
	elem->LinkEndChild(causesRoot);

	TiXmlElement* effectsRoot = new TiXmlElement( "do" );
	effectsRoot->SetAttribute("effects", effectsCount);
	for (int i = 0; i < effectsCount; i++) {
		TiXmlElement* effectElem = new TiXmlElement( "effect" );
		effects[i]->save(effectElem);
		effectsRoot->LinkEndChild( effectElem );
	}
	elem->LinkEndChild(effectsRoot);
}

bool CharacterAction::take(Game* game, Character* character) {
	bool fulfilled = true;
	for (int i = 0; i < causesCount; i++) {
		fulfilled &= causes[i]->test(game, character);
	}

	if (fulfilled) {
        perform(game, character);
        return true;
	}

	return false;
}
void CharacterAction::perform(Game* game, Character* character) {
    for (int i = 0; i < effectsCount; i++) {
        effects[i]->perform(game, character);
    }
}

void CharacterAction::prepare(Character* character) {
    prepareResource(character);
    prepareStatus(character);
    for (int i = 0; i < causesCount; i++) {
		causes[i]->setUsed(false);
	}
}
void CharacterAction::prepareStatus(Character* character) {
	for (int i = 0; i < causesCount; i++) {
		if (causes[i]->getType() == ACTIONCAUSE_TYPE_HAVE_STATUS) {
            character->setStatusAction(causes[i]->getParam(), this);
		}
	}
}
void CharacterAction::prepareResource(Character* character) {
	for (int i = 0; i < causesCount; i++) {
		if (causes[i]->getType() == ACTIONCAUSE_TYPE_RESORCE_LESS) {
            printf("cause %d: %.2f %.2f\n", i, causes[i]->getParam(), causes[i]->getParam(2));
            character->setResourceLessAction(causes[i]->getParam(), causes[i]->getParam(2), this);
		}
	}
}

int CharacterAction::getEffectsCount() {
    return effectsCount;
}
CharacterActionEffect* CharacterAction::getEffect(int index) {
    if (index >= effectsCount)
        return NULL;

    return effects[index];
}
void CharacterAction::addEffect() {
    CharacterActionEffect** _effects = new CharacterActionEffect*[effectsCount + 1];
    for (int i = 0; i < effectsCount; i++) {
        _effects[i] = effects[i];
    }
    _effects[effectsCount] = new CharacterActionEffect(ACTIONEFFECT_TYPE_TURN, 0.0f);

    delete effects;
    effects = _effects;
    effectsCount++;
}
void CharacterAction::removeEffect(int index) {
    delete effects[index];
    for (int i = index; i < effectsCount - 1; i++) {
        effects[i] = effects[i + 1];
    }
    effectsCount--;
}

int CharacterAction::getCausesCount() {
    return causesCount;
}
CharacterActionCause* CharacterAction::getCause(int index) {
    if (index >= causesCount)
        return NULL;

    return causes[index];
}
void CharacterAction::addCause() {
    CharacterActionCause** _causes = new CharacterActionCause*[causesCount + 1];
    for (int i = 0; i < causesCount; i++) {
        _causes[i] = causes[i];
    }
    _causes[causesCount] = new CharacterActionCause(ACTIONCAUSE_TYPE_NONE, 0.0f);

    delete causes;
    causes = _causes;
    causesCount++;
}
void CharacterAction::removeCause(int index) {
    delete causes[index];
    for (int i = index; i < causesCount - 1; i++) {
        causes[i] = causes[i + 1];
    }
    causesCount--;
}

