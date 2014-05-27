#include "CharacterResourcePrototype.h"

CharacterResourcePrototype::CharacterResourcePrototype()
{
    name = "character_resource";
    defaultFullValue = 1.0f;
    defaultRegen = 0.0f;
    eventsCount = 0;

    eventPercents = new float[eventsCount];
    eventConditions = new int[eventsCount];
}

CharacterResourcePrototype::~CharacterResourcePrototype()
{
    delete name;
    delete eventPercents;
    delete eventConditions;
}

void CharacterResourcePrototype::loadFromXml(TiXmlElement* xml) {
    printf("loading resource prototype\n");
    if (xml->Attribute("name")) {
        delete name;
        name = copyString(xml->Attribute("name"));
        printf("name is %s\n", name);
	}

	if (xml->Attribute("default_full_value")) {
        defaultFullValue = atof(xml->Attribute("default_full_value"));
	}
	if (xml->Attribute("default_regen")) {
        defaultRegen = atof(xml->Attribute("default_regen"));
	}
	printf("loaded\n");
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

int CharacterResourcePrototype::getEventsCount() {
    return eventsCount;
}
void CharacterResourcePrototype::addEvent() {
    float* _eventPercents = new float[eventsCount + 1];
    int* _eventConditions = new int[eventsCount + 1];
    for (int i = 0; i < eventsCount; i++) {
        _eventPercents[i] = eventPercents[i];
        _eventConditions[i] = eventConditions[i];
    }
    _eventPercents[eventsCount] = 0.0f;
    _eventConditions[eventsCount] = 0;

    delete eventPercents;
    delete eventConditions;
    eventPercents = _eventPercents;
    eventConditions = _eventConditions;
    eventsCount++;
}
void CharacterResourcePrototype::removeEvent(int index) {
    for (int i = index; i < eventsCount - 1; i++) {
        eventPercents[i] = eventPercents[i - 1];
        eventConditions[i] = eventConditions[i - 1];
    }
    eventsCount--;
}

float CharacterResourcePrototype::getEventPercent(int index) {
    if (index > -1 && index < eventsCount)
        return eventPercents[index];
    return 0.0f;
}
void CharacterResourcePrototype::setEventPercent(int index, float percent) {
    if (index > -1 && index < eventsCount)
        eventPercents[index] = percent;
}
int CharacterResourcePrototype::getEventCondition(int index) {
    if (index > -1 && index < eventsCount)
        return eventConditions[index];
    return 0;
}
void CharacterResourcePrototype::setEventCondition(int index, int condition) {
    if (index > -1 && index < eventsCount)
        eventConditions[index] = condition;
}
