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

		bool take(Game* game, Character* character); void perform(Game* game, Character* character);
		void prepareStatus(Character* character), prepareResource(Character* character);

		int getEffectsCount();
		CharacterActionEffect* getEffect(int index);
		void addEffect(), removeEffect(int index);

		int getCausesCount();
		CharacterActionCause* getCause(int index);
		void addCause(), removeCause(int index);
	protected:
		int causesCount;
		CharacterActionCause** causes;
		int effectsCount;
		CharacterActionEffect** effects;
	private:
};

#endif // CHARACTERACTION_H
