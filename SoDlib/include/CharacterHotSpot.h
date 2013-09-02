#ifndef CHARACTERHOTSPOT_H
#define CHARACTERHOTSPOT_H

#include <SoDlib.h>

class CharacterHotSpot
{
	public:
		CharacterHotSpot(b2Shape* shape, int type);
		CharacterHotSpot(TiXmlElement* xml);
		virtual ~CharacterHotSpot();

		b2Shape* getShape();
	protected:
		b2Shape* shape;
		int type;
	private:
};

#endif // CHARACTERHOTSPOT_H
