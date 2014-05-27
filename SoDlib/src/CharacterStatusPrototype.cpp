#include "CharacterStatusPrototype.h"

CharacterStatusPrototype::CharacterStatusPrototype()
{
    name = "<character status>";
    priority = 0.0f;
}

CharacterStatusPrototype::~CharacterStatusPrototype()
{
    delete name;
}

void CharacterStatusPrototype::loadFromXml(TiXmlElement* xml) {
    if (xml->Attribute("name")) {
        delete name;
        name = copyString( xml->Attribute("name") );
	}
	if (xml->Attribute("priority")) {
        priority = atof( xml->Attribute("priority") );
	}
}
void CharacterStatusPrototype::saveToXml(TiXmlElement* xml) {
    xml->SetAttribute("name", name);
    xml->SetDoubleAttribute("priority", priority);
}

char* CharacterStatusPrototype::getName() {
    return name;
}
void CharacterStatusPrototype::setName(char* _name) {
    if (name)
        delete name;
    name = copyString(_name);
}

float CharacterStatusPrototype::getPriority() {
    return priority;
}
void CharacterStatusPrototype::setPriority(float _priority) {
    priority = _priority;
}
