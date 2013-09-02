#include <SoDlib.h>
#ifdef CHARACTER_H_COMPLETE

#ifndef NONPLAYERCHARACTER_H
#define NONPLAYERCHARACTER_H

const int ANIMATION_OPERATION_IDLE    = 0;
const int ANIMATION_OPERATION_ASSAULT = 1;

const int TARGETSIDE_INFRONT = 0;
const int TARGETSIDE_BEHIND = 1;
const int TARGETSIDE_UNDEFINED = 2;

class NonPlayerCharacter : public Character
{
	public:
		NonPlayerCharacter(TiXmlElement* xml, Game * game, b2Vec2 origin);
		virtual ~NonPlayerCharacter();

		inline int getType() {return CHARACTER_TYPE_NONPLAYER;};

		void control(float dt);

		int getTargetSide();
		float getTargetDistance();
	protected:
		int targetCharacter;
	private:
};

#endif // NONPLAYERCHARACTER_H
#endif
