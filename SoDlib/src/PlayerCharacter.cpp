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
				return game->getHge()->Input_KeyDown(HGEK_A);
			} else {
				return game->getHge()->Input_KeyDown(HGEK_D);
			}
			break;
		case CHARACTER_KEY_FORTH:
			if (!turnedRight) {
				return game->getHge()->Input_KeyDown(HGEK_A);
			} else {
				return game->getHge()->Input_KeyDown(HGEK_D);
			}
			break;
		case CHARACTER_KEY_JUMP:
			return game->getHge()->Input_KeyDown(HGEK_W);
			break;
        case CHARACTER_KEY_ATTACK:
			return game->getHge()->Input_KeyDown(HGEK_LBUTTON);
			break;
        case CHARACTER_KEY_SATTACK:
			return game->getHge()->Input_KeyDown(HGEK_RBUTTON);
			break;
	}
}

bool PlayerCharacter::isKeyDown(int key) {
	switch (key) {
		case CHARACTER_KEY_BACK:
			if (turnedRight) {
				return game->getHge()->Input_GetKeyState(HGEK_A);
			} else {
				return game->getHge()->Input_GetKeyState(HGEK_D);
			}
			break;
		case CHARACTER_KEY_FORTH:
			if (!turnedRight) {
				return game->getHge()->Input_GetKeyState(HGEK_A);
			} else {
				return game->getHge()->Input_GetKeyState(HGEK_D);
			}
			break;
		case CHARACTER_KEY_JUMP:
			return game->getHge()->Input_GetKeyState(HGEK_W);
			break;
        case CHARACTER_KEY_ATTACK:
			return game->getHge()->Input_GetKeyState(HGEK_LBUTTON);
			break;
        case CHARACTER_KEY_SATTACK:
			return game->getHge()->Input_GetKeyState(HGEK_RBUTTON);
			break;
	}
}
