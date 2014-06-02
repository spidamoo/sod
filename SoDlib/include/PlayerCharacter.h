#include <SoDlib.h>
#ifdef CHARACTER_H_COMPLETE
#ifndef PLAYERCHARACTER_H
#define PLAYERCHARACTER_H

const int CHARACTER_KEY_BACK    = 1;
const int CHARACTER_KEY_FORTH   = 2;
const int CHARACTER_KEY_JUMP    = 3;
const int CHARACTER_KEY_ATTACK  = 4;
const int CHARACTER_KEY_SATTACK = 5;

class PlayerCharacter : public Character
{
	public:
		PlayerCharacter(TiXmlElement* xml, Game * game, b2Vec2 origin);
		virtual ~PlayerCharacter();

		inline int getType() {return CHARACTER_TYPE_PLAYER;};

		void control(float dt);

		bool isKeyHit(int key);
		bool isKeyDown(int key);
	protected:
	private:
};

#endif // PLAYERCHARACTER_H
#endif
