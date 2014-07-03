#include "AiPattern.h"

AiPattern::AiPattern(Game* _game) {
    game = _game;

    params = new float[16];
    for (int i = 0; i < 16; i++) {
        params[i] = 0;
    }
    type = AI_PATTERN_TYPE_STUPID_MELEE;
}

AiPattern::~AiPattern() {
    delete params;
}

void AiPattern::loadFromXml(TiXmlElement* xml) {
    if (xml->Attribute("name")) {
        delete name;
        name = copyString(xml->Attribute("name"));
	}

	if (xml->Attribute("type")) {
        type = atoi(xml->Attribute("type"));
	}

	for (int i = 0; i < 16; i++) {
        char buffer[16];
        sprintf(buffer, "param_%d", i);
        if (xml->Attribute(buffer)) {
            params[i] = atof( xml->Attribute("default_normal_value") );
        }
	}
}

char* AiPattern::getName() {
    return name;
}
void AiPattern::setName(char* _name) {
    if (name)
        delete name;
    name = copyString(_name);
}

int AiPattern::getType() {
    return type;
}
void AiPattern::setType(int _type) {
    type = _type;
}

float AiPattern::getParam(int index) {
    return params[index];
}
void AiPattern::setParam(int index, float _param) {
    params[index] = _param;
}

int AiPattern::calculatePriority(NonPlayerCharacter* character) {
    switch (type) {
        default:
        return 1;
    }
}
int AiPattern::calculateOperation(NonPlayerCharacter* character) {
    switch (type) {
        case AI_PATTERN_TYPE_STUPID_MELEE:
            if (character->targetCharacter == -1) {
                for (int i = 0; i < game->getCharactersCount(); i++) {
                    if (game->getCharacter(i)->getType() == CHARACTER_TYPE_PLAYER) {
                        character->targetCharacter = i;
                        break;
                    }
                }
            }

            if ( fabs( game->getCharacter(character->targetCharacter)->getPosition().x - character->position.x ) > params[0] ) {
                return NPC_OPERATION_TRAVERSE;
            }
            else {
                return NPC_OPERATION_DOMOVE;
            }

        default:
            return NPC_OPERATION_IDLE;
    }
}
b2Vec2 AiPattern::calculateTargetPosition(NonPlayerCharacter* character) {
    switch (type) {
        case AI_PATTERN_TYPE_STUPID_MELEE:
            if (character->targetCharacter != -1) {
                return game->getCharacter(character->targetCharacter)->getPosition();
            }
        default:
            return character->getPosition();
    }
}
int AiPattern::calculateMove(NonPlayerCharacter* character) {

}
