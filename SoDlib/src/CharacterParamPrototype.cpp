#include "CharacterParamPrototype.h"

CharacterParamPrototype::CharacterParamPrototype()
{
    name = "<character param>";
    defaultNormalValue = 1.0f;
}

CharacterParamPrototype::~CharacterParamPrototype()
{
    delete name;
}

void CharacterParamPrototype::loadFromXml(TiXmlElement* xml) {
    if (xml->Attribute("name")) {
        delete name;
        name = copyString(xml->Attribute("name"));
	}

	if (xml->Attribute("default_normal_value")) {
        defaultNormalValue = atof(xml->Attribute("default_normal_value"));
	}
}
void CharacterParamPrototype::saveToXml(TiXmlElement* xml) {
    xml->SetAttribute("name", name);
    xml->SetDoubleAttribute("default_normal_value", defaultNormalValue);
}

char* CharacterParamPrototype::getName() {
    return name;
}
void CharacterParamPrototype::setName(char* _name) {
    if (name)
        delete name;
    name = copyString(_name);
}

float CharacterParamPrototype::getDefaultNormalValue() {
    return defaultNormalValue;
}
void CharacterParamPrototype::setDefaultNormalValue(float _value) {
    defaultNormalValue = _value;
}
