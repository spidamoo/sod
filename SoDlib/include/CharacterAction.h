#ifndef CHARACTERACTION_H
#define CHARACTERACTION_H

#include <SoDlib.h>

class CharacterAction
{
	public:
		CharacterAction();
		CharacterAction(TiXmlElement* elem);
		virtual ~CharacterAction();

		void save(TiXmlElement* elem);

		bool take(Game* game, Character* character);

		int getEffectsCount();
		CharacterActionEffect* getEffect(int index);

		int getCausesCount();
		CharacterActionCause* getCause(int index);
	protected:
		int causesCount;
		CharacterActionCause** causes;
		int effectsCount;
		CharacterActionEffect** effects;
	private:
};

#endif // CHARACTERACTION_H
