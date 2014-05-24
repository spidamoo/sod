#include "CharacterResourcePrototype.h"

CharacterResourcePrototype::CharacterResourcePrototype()
{
    name = "<character resource>";
    defaultFullValue = 1.0f;
    defaultRegen = 0.0f;

}

CharacterResourcePrototype::~CharacterResourcePrototype()
{
    delete name;
}

void CharacterResourcePrototype::loadFromXml(TiXmlElement* xml) {
    if (xml->Attribute("name")) {
        delete name;
        name = copyString(xml->Attribute("name"));
	}

	if (xml->Attribute("default_full_value")) {
        defaultFullValue = atof(xml->Attribute("default_full_value"));
	}
	if (xml->Attribute("default_regen")) {
        defaultRegen = atof(xml->Attribute("default_regen"));
	}
}
void CharacterResourcePrototype::saveToXml(TiXmlElement* xml) {
    xml->SetAttribute("name", name);
    xml->SetDoubleAttribute("default_full_value", defaultFullValue);
    xml->SetDoubleAttribute("default_regen", defaultRegen);
}


char* CharacterResourcePrototype::getName() {
    return name;
}
void CharacterResourcePrototype::setName(char* _name) {
    if (name)
        delete name;
    name = copyString(_name);
}

float CharacterResourcePrototype::getDefaultFullValue() {
    return defaultFullValue;
}
void CharacterResourcePrototype::setDefaultFullValue(float _value) {
    defaultFullValue = _value;
}

float CharacterResourcePrototype::getDefaultRegen() {
    return defaultRegen;
}
void CharacterResourcePrototype::setDefaultRegen(float value) {
    defaultRegen = value;
}
