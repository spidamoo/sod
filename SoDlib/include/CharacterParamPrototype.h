#ifndef CHARACTERPARAMPROTOTYPE_H
#define CHARACTERPARAMPROTOTYPE_H

#include <SoDlib.h>


class CharacterParamPrototype
{
    public:
        CharacterParamPrototype();
        virtual ~CharacterParamPrototype();

        void loadFromXml(TiXmlElement* xml), saveToXml(TiXmlElement* xml);

        char* getName(); void setName(char* name);
        float getDefaultNormalValue(); void setDefaultNormalValue(float defaultNormalValue);

    protected:
        char* name; float defaultNormalValue;

    private:
};

#endif // CHARACTERPARAMPROTOTYPE_H
