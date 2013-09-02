#ifndef CONSTRUCTION_H
#define CONSTRUCTION_H

#include <SoDlib.h>

class Construction
{
	public:
		Construction();
		Construction(char * fileName, Game * game, b2Vec2 origin, b2BodyType type);
		virtual ~Construction();

		void draw(Game* game, bool schematicMode);

	protected:
		b2Body** bodies;
		hgeAnimation** animations;
		int bodiesCount;

	private:
};

#endif // CONSTRUCTION_H
