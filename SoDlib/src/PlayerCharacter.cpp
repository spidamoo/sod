#include "PlayerCharacter.h"

PlayerCharacter::PlayerCharacter(TiXmlElement* xml, Game * game, b2Vec2 origin) : Character(xml, game, origin)
{
	//ctor
}

PlayerCharacter::~PlayerCharacter()
{
	//dtor
}

void PlayerCharacter::control(float dt)
{

}

bool PlayerCharacter::isKeyHit(int key)
{
	switch (key) {
		case CHARACTER_KEY_BACK:
			if (turnedRight) {
				return game->getHge()->Input_KeyDown(HGEK_LEFT);
			} else {
				return game->getHge()->Input_KeyDown(HGEK_RIGHT);
			}
			break;
		case CHARACTER_KEY_FORTH:
			if (!turnedRight) {
				return game->getHge()->Input_KeyDown(HGEK_LEFT);
			} else {
				return game->getHge()->Input_KeyDown(HGEK_RIGHT);
			}
			break;
		case CHARACTER_KEY_JUMP:
			return game->getHge()->Input_KeyDown(HGEK_UP);
			break;
        case CHARACTER_KEY_ATTACK:
			return game->getHge()->Input_KeyDown(HGEK_SPACE);
			break;
	}
}

bool PlayerCharacter::isKeyDown(int key) {
	switch (key) {
		case CHARACTER_KEY_BACK:
			if (turnedRight) {
				return game->getHge()->Input_GetKeyState(HGEK_LEFT);
			} else {
				return game->getHge()->Input_GetKeyState(HGEK_RIGHT);
			}
			break;
		case CHARACTER_KEY_FORTH:
			if (!turnedRight) {
				return game->getHge()->Input_GetKeyState(HGEK_LEFT);
			} else {
				return game->getHge()->Input_GetKeyState(HGEK_RIGHT);
			}
			break;
		case CHARACTER_KEY_JUMP:
			return game->getHge()->Input_GetKeyState(HGEK_UP);
			break;
        case CHARACTER_KEY_ATTACK:
			return game->getHge()->Input_GetKeyState(HGEK_SPACE);
			break;
	}
}
