#ifndef AIPATTERN_H
#define AIPATTERN_H

#include <SoDlib.h>

const int AI_PATTERN_TYPE_STUPID_MELEE   = 1;
const int AI_PATTERN_TYPE_COWARDY_RANGED = 2;


class AiPattern {
    public:
        AiPattern(Game* game);
        virtual ~AiPattern();

        void loadFromXml(TiXmlElement* xml), saveToXml(TiXmlElement* xml);

        char* getName(); void setName(char* name);
        int getType(); void setType(int type);
        float getParam(int index); void setParam(int index, float param);

        int calculatePriority(NonPlayerCharacter* character); int calculateOperation(NonPlayerCharacter* character);
        b2Vec2 calculateTargetPosition(NonPlayerCharacter* character); int calculateMove(NonPlayerCharacter* character);
    protected:
        Game* game;
        char* name;
        int type; float* params;
    private:
};

#endif // AIPATTERN_H
