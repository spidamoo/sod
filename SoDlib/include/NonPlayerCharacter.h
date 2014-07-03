#include <SoDlib.h>
#ifdef CHARACTER_H_COMPLETE

#ifndef NONPLAYERCHARACTER_H
#define NONPLAYERCHARACTER_H

const int NPC_OPERATION_IDLE     = 0;
const int NPC_OPERATION_TRAVERSE = 1;
const int NPC_OPERATION_DOMOVE   = 2;

const int MOVE_TYPE_IDLE     = 0;
const int MOVE_TYPE_TRAVERSE = 1;
const int MOVE_TYPE_ATTACK   = 2;

class NonPlayerCharacter : public Character {
    friend class AiPattern;
	public:
		NonPlayerCharacter(TiXmlElement* xml, Game * game, b2Vec2 origin);
		virtual ~NonPlayerCharacter();

		bool loadFromXml(TiXmlElement* xml, b2Vec2 origin);

		inline int getType() {return CHARACTER_TYPE_NONPLAYER;};

		void update(float dt);

	protected:
		int* aiPatterns; int aiPatternsCount;
		int traverseMove;

		int targetCharacter;
		b2Vec2 targetPosition;
	private:
};

#endif // NONPLAYERCHARACTER_H
#endif
