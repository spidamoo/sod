#ifndef CHARACTERMOVETYPE_H
#define CHARACTERMOVETYPE_H

#include <SoDlib.h>

class CharacterMoveType
{
    public:
        CharacterMoveType();
        virtual ~CharacterMoveType();

        void loadFromXml(TiXmlElement* xml), saveToXml(TiXmlElement* xml);

        char* getName(); void setName(char* name);
    protected:
        char* name;
    private:
};

#endif // CHARACTERMOVETYPE_H
