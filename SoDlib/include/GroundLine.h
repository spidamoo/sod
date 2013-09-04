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
		int getType();
		b2Vec2 getStartPoint(), getEndPoint();
		float horizontalDistanceTo(float x);

		void debugDraw(), debugDraw(DWORD color);
	protected:
		b2Vec2 startPoint, endPoint;
		float k;
		float angle, sinA, cosA;
		Game* game;
	private:
};

const int GROUND_LINE_TYPE_FLOOR = 0;
const int GROUND_LINE_TYPE_WALL  = 1;

#endif // GROUNDLINE_H
