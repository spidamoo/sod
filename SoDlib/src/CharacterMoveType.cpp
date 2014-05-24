#include "CharacterMoveType.h"

CharacterMoveType::CharacterMoveType() {
    name = "<move type>";
}

CharacterMoveType::~CharacterMoveType() {
    delete name;
}

void CharacterMoveType::loadFromXml(TiXmlElement* xml) {
    if (xml->Attribute("name")) {
        delete name;
        name = copyString(xml->Attribute("name"));
	}
}
void CharacterMoveType::saveToXml(TiXmlElement* xml) {
    xml->SetAttribute("name", name);
}


char* CharacterMoveType::getName() {
    return name;
}
void CharacterMoveType::setName(char* _name) {
    if (name)
        delete name;
    name = copyString(_name);
}
