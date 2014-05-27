#ifndef CHARACTERSTATUSPROTOTYPE_H
#define CHARACTERSTATUSPROTOTYPE_H

#include <SoDlib.h>


class CharacterStatusPrototype
{
    public:
        CharacterStatusPrototype();
        virtual ~CharacterStatusPrototype();

        void loadFromXml(TiXmlElement* xml), saveToXml(TiXmlElement* xml);

        char* getName(); void setName(char* name);
        float getPriority(); void setPriority(float priority);
    protected:
        char* name; float priority;
    private:
};

#endif // CHARACTERSTATUSPROTOTYPE_H
