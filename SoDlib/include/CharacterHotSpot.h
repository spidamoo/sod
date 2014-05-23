#ifndef CHARACTERHOTSPOT_H
#define CHARACTERHOTSPOT_H

#include <SoDlib.h>

const int HOT_SPOT_TYPE_WEAPON = 1;

class CharacterHotSpot
{
	public:
		CharacterHotSpot(b2Shape* shape, int type);
		CharacterHotSpot(TiXmlElement* xml);
		virtual ~CharacterHotSpot();

		b2Shape* getShape();
		char* getName(); void setName(char* name);
	protected:
		b2Shape* shape;
		int type;
		char* name;
	private:
};

#endif // CHARACTERHOTSPOT_H
