#ifndef GROUNDLINE_H
#define GROUNDLINE_H

#include <SoDlib.h>

class GroundLine
{
	public:
		GroundLine(Game* game, float x1, float y1, float x2, float y2);
		virtual ~GroundLine();

		b2Vec2 characterIntersects(Character* character);
		float yAt(float x);
		float getCosA(), getSinA();
		b2Vec2 getStartPoint(), getEndPoint();

		void debugDraw();
	protected:
		b2Vec2 startPoint, endPoint;
		float k;
		float angle, sinA, cosA;
		Game* game;
	private:
};

#endif // GROUNDLINE_H
