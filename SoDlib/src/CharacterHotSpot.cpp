#include "CharacterHotSpot.h"

CharacterHotSpot::CharacterHotSpot(b2Shape* shape, int type)
{
	this->shape = shape;
	this->type = type;
}

CharacterHotSpot::CharacterHotSpot(TiXmlElement* xml)
{
	float w = atof(xml->Attribute("w"));
	float h = atof(xml->Attribute("h"));

	shape = new b2PolygonShape();
	((b2PolygonShape*)shape)->SetAsBox(0.5f * w, 0.5f * h);
	type = 1;
}

CharacterHotSpot::~CharacterHotSpot()
{
	//dtor
}

b2Shape* CharacterHotSpot::getShape()
{
	return this->shape;
}
