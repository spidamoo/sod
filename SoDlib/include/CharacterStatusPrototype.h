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
    protected:
        char* name;
    private:
};

#endif // CHARACTERSTATUSPROTOTYPE_H
