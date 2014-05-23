#include "CharacterHotSpot.h"

CharacterHotSpot::CharacterHotSpot(b2Shape* shape, int type)
{
	this->shape = shape;
	this->type = type;

	name = "";
}

CharacterHotSpot::CharacterHotSpot(TiXmlElement* xml)
{
	float w = atof(xml->Attribute("w"));
	float h = atof(xml->Attribute("h"));

	shape = new b2PolygonShape();
	((b2PolygonShape*)shape)->SetAsBox(0.5f * w, 0.5f * h);
	type = 1;
	if (xml->Attribute("name")) {
        name = copyString( xml->Attribute("name") );
	}
	else {
        name = "";
	}
}

CharacterHotSpot::~CharacterHotSpot()
{
	delete name;
	delete shape;
}

b2Shape* CharacterHotSpot::getShape()
{
	return this->shape;
}

char* CharacterHotSpot::getName() {
    return name;
}
void CharacterHotSpot::setName(char* _name) {
    delete name;

    name = copyString(_name);
}
