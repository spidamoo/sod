#include "GroundLine.h"

GroundLine::GroundLine(Game* game, float x1, float y1, float x2, float y2)
{
	this->game = game;
	if (x1 < x2) {
		startPoint.x = x1;
		startPoint.y = y1;
		endPoint.x = x2;
		endPoint.y = y2;
	} else {
		startPoint.x = x2;
		startPoint.y = y2;
		endPoint.x = x1;
		endPoint.y = y1;
	}

	angle = atan2(endPoint.y - startPoint.y, endPoint.x - startPoint.x);
	sinA = sin(angle);
	cosA = cos(angle);
	k = (y2 - y1) / (x2 - x1);
}

GroundLine::~GroundLine()
{
	//dtor
}

b2Vec2 GroundLine::characterIntersects(Character* character, int type)
{
	if (type == GROUND_LINE_INTERSECTION_TYPE_FULL) {
		b2Vec2 intersectPoint = intersection(
			startPoint,
			endPoint,
			b2Vec2(character->getX() - character->getHalfWidth(), character->getY() - character->getHalfHeight()),
			b2Vec2(character->getX() + character->getHalfWidth(), character->getY() + character->getHalfHeight())
		);
		if (intersectPoint == b2Vec2_zero) {
			intersectPoint = intersection(
				startPoint,
				endPoint,
				b2Vec2(character->getX() - character->getHalfWidth(), character->getY() + character->getHalfHeight()),
				b2Vec2(character->getX() + character->getHalfWidth(), character->getY() - character->getHalfHeight())
			);
		}
		return intersectPoint;
	} else if (type == GROUND_LINE_INTERSECTION_TYPE_MIDDLE) {
		b2Vec2 intersectPoint = intersection(
			startPoint,
			endPoint,
			b2Vec2(character->getX(), character->getY() - character->getHalfHeight()),
			b2Vec2(character->getX(), character->getY() + character->getHalfHeight())
		);
		return intersectPoint;
	}
}

float GroundLine::yAt(float x)
{
	return startPoint.y + (x - startPoint.x) * k;
}

float GroundLine::getCosA()
{
	return cosA;
}
float GroundLine::getSinA()
{
	return sinA;
}
float GroundLine::getK()
{
	return k;
}

int GroundLine::getType()
{
    if (angle > M_PI_4 || angle < -M_PI_4) {
        return GROUND_LINE_TYPE_WALL;
    } else {
        return GROUND_LINE_TYPE_FLOOR;
    }
}

b2Vec2 GroundLine::getStartPoint()
{
	return startPoint;
}
b2Vec2 GroundLine::getEndPoint()
{
	return endPoint;
}

void GroundLine::debugDraw(DWORD color)
{
	game->drawLine(game->screenPos(startPoint), game->screenPos(endPoint), color);
}
void GroundLine::debugDraw()
{
	debugDraw(0xFFFFFFFF);
}

float GroundLine::horizontalDistanceTo(float x)
{
    if (x < startPoint.x) {
        return startPoint.x - x;
    }
    if (x > endPoint.x) {
        return x - endPoint.x;
    }
    return 0;
}
