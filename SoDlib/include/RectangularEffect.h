#ifndef RECTANGULAREFFECT_H
#define RECTANGULAREFFECT_H

#include <SoDlib.h>


class RectangularEffect : public Effect
{
	public:
		RectangularEffect();
		virtual ~RectangularEffect();
	protected:
		b2Vec2 size;
	private:
};

#endif // RECTANGULAREFFECT_H
