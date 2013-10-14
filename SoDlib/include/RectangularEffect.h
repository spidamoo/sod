#include <SoDlib.h>
#ifdef CHARACTER_H_COMPLETE
#ifndef RECTANGULAREFFECT_H
#define RECTANGULAREFFECT_H


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
#endif
