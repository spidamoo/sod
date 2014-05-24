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
    protected:
        char* name; float defaultFullValue; float defaultRegen;
    private:
};

#endif // CHARACTERRESOURCEPROTOTYPE_H
