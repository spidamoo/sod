#ifndef GROUNDLINE_H
#define GROUNDLINE_H

#include <SoDlib.h>

class GroundLine
{
	public:
		GroundLine(Game* game, float x1, float y1, float x2, float y2);
		virtual ~GroundLine();

		b2Vec2 characterIntersects(Character* character, int type);
		float yAt(float x), xAt(float y);
		float getAngle(), getCosA(), getSinA();
		float getK();
		int getType();
		b2Vec2 getStartPoint(), getEndPoint();
		float getLeft(), getRight(), getTop(), getBottom();
		float getLength();
		float horizontalDistanceTo(float x);
		float distanceTo(float x, float y);
		void debugDraw(), debugDraw(DWORD color);
		void move(float dx, float dy);
	protected:
		b2Vec2 startPoint, endPoint;
		float k, inversedK;
		float angle, sinA, cosA;
		float length;
		Game* game;
	private:
};

const int GROUND_LINE_TYPE_FLOOR = 0;
const int GROUND_LINE_TYPE_WALL  = 1;

const int GROUND_LINE_INTERSECTION_TYPE_FULL = 0;
const int GROUND_LINE_INTERSECTION_TYPE_MIDDLE = 1;

#endif // GROUNDLINE_H
