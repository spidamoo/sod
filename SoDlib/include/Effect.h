#ifndef EFFECT_H
#define EFFECT_H

#include <SoDlib.h>

class Effect
{
	public:
		Effect(Game* game, EffectPrototype* prototype);
		virtual ~Effect();

		void setOwner(Character* character); Character* getOwner();
		void initialize();

		void update(float dt), draw(bool schematicMode);

		float getTime();
		void setPosition(b2Vec2 position); void setPosition(b2Vec2 position, float angle); b2Vec2 getPosition();
		void setHotSpotIndex(int index);

		void setAnimation(hgeAnimation* animation, int blendMode);

		bool characterCrosses(Character* character);
	protected:
		Game* game; Character* owner;
		float angle, speed;
		b2Vec2 position, prevPosition, componentSpeed;
		float width, height;
		float time;
		int r, g, b, a;
		float scale;
		int hotSpotIndex;

		EffectPrototype* prototype;

		float* actionTimes;

		hgeAnimation* animation;

		int** actionInteractions;
	private:
};
#endif // EFFECT_H
