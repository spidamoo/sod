#include "CharacterStatusPrototype.h"

CharacterStatusPrototype::CharacterStatusPrototype()
{
    name = "<character status>";
}

CharacterStatusPrototype::~CharacterStatusPrototype()
{
    delete name;
}

void CharacterStatusPrototype::loadFromXml(TiXmlElement* xml) {
    if (xml->Attribute("name")) {
        delete name;
        name = copyString(xml->Attribute("name"));
	}
}
void CharacterStatusPrototype::saveToXml(TiXmlElement* xml) {
    xml->SetAttribute("name", name);
}

char* CharacterStatusPrototype::getName() {
    return name;
}
void CharacterStatusPrototype::setName(char* _name) {
    if (name)
        delete name;
    name = copyString(_name);
}
