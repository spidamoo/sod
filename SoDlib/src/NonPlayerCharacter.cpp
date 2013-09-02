#include "NonPlayerCharacter.h"

NonPlayerCharacter::NonPlayerCharacter(TiXmlElement* xml, Game * game, b2Vec2 origin) : Character(xml, game, origin)
{
	targetCharacter = -1;
	//ctor
}

NonPlayerCharacter::~NonPlayerCharacter()
{
	//dtor
}

void NonPlayerCharacter::control(float dt)
{
	if (targetCharacter == -1) {
		for (int i = 0; i < game->getCharactersCount(); i++) {
			if (game->getCharacter(i)->getType() == CHARACTER_TYPE_PLAYER) {
				targetCharacter = i;
				break;
			}
		}
	}
}

int NonPlayerCharacter::getTargetSide()
{
	if (targetCharacter != -1) {
		if (game->getCharacter(targetCharacter)->getPosition().x > getPosition().x) {
			return turnedRight ? TARGETSIDE_INFRONT : TARGETSIDE_BEHIND;
		} else {
			return turnedRight ? TARGETSIDE_BEHIND : TARGETSIDE_INFRONT;
		}
	}
	return TARGETSIDE_UNDEFINED;
}

float NonPlayerCharacter::getTargetDistance()
{
	if (targetCharacter != -1) {
		return fabs(game->getCharacter(targetCharacter)->getPosition().x - getPosition().x);
	}
	return 0;
}
