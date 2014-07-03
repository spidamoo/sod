#include "NonPlayerCharacter.h"

NonPlayerCharacter::NonPlayerCharacter(TiXmlElement* xml, Game * game, b2Vec2 origin) : Character(xml, game, origin) {
    loadFromXml(xml, origin);

	targetCharacter = -1;

	aiPatternsCount = 0;
	aiPatterns = new int[10];

    traverseMove = 0;

	for (int i = 0; i < movesCount; i++) {
        if ( compareStrings(moveNames[i], "traverse") ) {
            traverseMove = i;
            break;
        }
	}
}

NonPlayerCharacter::~NonPlayerCharacter() {
}

bool NonPlayerCharacter::loadFromXml(TiXmlElement* xml, b2Vec2 origin) {
    TiXmlElement* aiRoot = xml->FirstChildElement("ai");
    if (aiRoot) {
        TiXmlElement* element = aiRoot->FirstChildElement("pattern");
        aiPatternsCount = 0;
        while (element) {
            aiPatterns[aiPatternsCount] = atoi( element->Attribute("index") );
            aiPatternsCount++;
            element = element->NextSiblingElement("pattern");
        }
    }
}

void NonPlayerCharacter::update(float dt) {
    Character::update(dt);

	int priority = 0;
	int bestPattern = -1;
	for (int i = 0; i < aiPatternsCount; i++) {
        int p = game->getAiPattern( aiPatterns[i] )->calculatePriority(this);
        if ( p > priority ) {
            priority = p;
            bestPattern = i;
        }
	}

	if (bestPattern > -1) {
        switch ( game->getAiPattern( aiPatterns[bestPattern] )->calculateOperation(this) ) {
            case NPC_OPERATION_TRAVERSE:
                targetPosition = game->getAiPattern( aiPatterns[bestPattern] )->calculateTargetPosition(this);
                if ( (targetPosition.x > position.x && !turnedRight) || (targetPosition.x < position.x && turnedRight) ) {
                    turn();
                }
                setMove(traverseMove);
                break;
            case NPC_OPERATION_DOMOVE:
                break;
        }
	}
}
