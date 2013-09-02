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
	protected:
		int causesCount;
		CharacterActionCause** causes;
		int effectsCount;
		CharacterActionEffect** effects;
	private:
};

#endif // CHARACTERACTION_H
