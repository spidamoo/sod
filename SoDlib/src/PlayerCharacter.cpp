#include "PlayerCharacter.h"

PlayerCharacter::PlayerCharacter(TiXmlElement* xml, Game * game, b2Vec2 origin) : Character(xml, game, origin) {
	keyWasHit = new bool[7];
}

PlayerCharacter::~PlayerCharacter() {
	//dtor
}

bool PlayerCharacter::isKeyHit(int key) {
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
			return game->getHge()->Input_KeyDown(HGEK_LBUTTON);
			break;
        case CHARACTER_KEY_SATTACK:
			return game->getHge()->Input_KeyDown(HGEK_RBUTTON);
			break;
        case CHARACTER_KEY_SIT:
			return game->getHge()->Input_KeyDown(HGEK_DOWN);
			break;
	}
}

bool PlayerCharacter::wasKeyHit(int key) {
    return keyWasHit[key];
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
			return game->getHge()->Input_GetKeyState(HGEK_LBUTTON);
			break;
        case CHARACTER_KEY_SATTACK:
			return game->getHge()->Input_GetKeyState(HGEK_RBUTTON);
			break;
        case CHARACTER_KEY_SIT:
			return game->getHge()->Input_GetKeyState(HGEK_DOWN);
			break;
	}
}

void PlayerCharacter::setMove(int move) {
    Character::setMove(move);
    for (int i = 1; i < 7; i++) {
        keyWasHit[i] = false;
	}
}

void PlayerCharacter::update(float dt) {
    Character::update(dt);

    for (int i = 1; i < 7; i++) {
        keyWasHit[i] |= isKeyHit(i);
	}
}
