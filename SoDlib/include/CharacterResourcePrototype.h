#ifndef CHARACTERRESOURCEPROTOTYPE_H
#define CHARACTERRESOURCEPROTOTYPE_H

#include <SoDlib.h>

class CharacterResourcePrototype
{
    public:
        CharacterResourcePrototype();
        virtual ~CharacterResourcePrototype();

        void loadFromXml(TiXmlElement* xml), saveToXml(TiXmlElement* xml);

        char* getName(); void setName(char* name);
        float getDefaultFullValue(); void setDefaultFullValue(float value);
        float getDefaultRegen(); void setDefaultRegen(float value);

        int getEventsCount(); void addEvent(); void removeEvent(int index);
        float getEventPercent(int index); void setEventPercent(int index, float percent); int getEventCondition(int index); void setEventCondition(int index, int condition);
    protected:
        char* name; float defaultFullValue; float defaultRegen;
        int eventsCount;
        float* eventPercents; int* eventConditions;
    private:
};

#endif // CHARACTERRESOURCEPROTOTYPE_H
