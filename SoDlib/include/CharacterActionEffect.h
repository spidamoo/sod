#ifndef CHARACTERACTIONEFFECT_H
#define CHARACTERACTIONEFFECT_H

#include <SoDlib.h>

const int ACTIONEFFECT_TYPE_TURN = 1;
const int ACTIONEFFECT_TYPE_CHANGEANIM = 2;
const int ACTIONEFFECT_TYPE_RUN = 3;
const int ACTIONEFFECT_TYPE_JUMP = 4;
const int ACTIONEFFECT_TYPE_SPAWN_EFFECT = 5;

class CharacterActionEffect
{
	public:
		CharacterActionEffect(float type, float param);
		CharacterActionEffect(TiXmlElement* elem);
		virtual ~CharacterActionEffect();

		void save(TiXmlElement* elem);

		void perform(Game* game, Character* character);
	protected:
		int type;
		float param;
		float param2;
	private:
};

#endif // CHARACTERACTIONEFFECT_H
