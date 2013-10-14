#ifndef EFFECT_H
#define EFFECT_H

#include <SoDlib.h>

class Effect
{
	public:
		Effect();
		virtual ~Effect();

		void update(float dt), draw();
	protected:
		Game* game;
		b2Vec2 position, prevPosition, speed;
		float time;
	private:
};
#define EFFECT_H_COMPLETE
#endif // EFFECT_H
