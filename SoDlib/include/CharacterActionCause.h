#ifndef CHARACTERACTIONCAUSE_H
#define CHARACTERACTIONCAUSE_H

#include <SoDlib.h>

const int ACTIONCAUSE_TYPE_NONE        = 0;
const int ACTIONCAUSE_TYPE_KEYHIT      = 1;
const int ACTIONCAUSE_TYPE_KEYRELEASED = 2;
const int ACTIONCAUSE_TYPE_KEYDOWN     = 3;

const int ACTIONCAUSE_TYPE_NPC_TARGETSIDE    = 101;
const int ACTIONCAUSE_TYPE_NPC_TARGETFARTHER = 102;
const int ACTIONCAUSE_TYPE_NPC_TARGETCLOSER  = 103;

const int ACTIONCAUSE_TYPE_ONGROUND    = 201;
const int ACTIONCAUSE_TYPE_ANGLE_MORE  = 202;
const int ACTIONCAUSE_TYPE_ANGLE_LESS  = 203;

const int ACTIONCAUSE_TYPE_ANIM_TIME_PASSED    = 301;
const int ACTIONCAUSE_TYPE_ANIM_TIME_IS    = 302;

const int ACTIONCAUSE_TYPE_HAVE_STATUS    = 401;

const int ACTIONCAUSE_TYPE_RESORCE_LESS    = 501;
const int ACTIONCAUSE_TYPE_RESORCE_MORE    = 502;


class CharacterActionCause
{
	public:
		CharacterActionCause(int type, float param);
		CharacterActionCause(TiXmlElement* elem);
		virtual ~CharacterActionCause();

		void save(TiXmlElement* elem);

		bool test(Game* game, Character* character);

		int   getType();
		void  setType(int type);
		float getParam(), getParam(int index);
		void  setParam(float param), setParam(int index, float param);
	protected:
		int type;
		float param; float param2;
	private:
};

#endif // CHARACTERACTIONCAUSE_H
