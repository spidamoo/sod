#ifndef CONDITION_PROTOTYPE_H
#define CONDITION_PROTOTYPE_H

#include <SoDlib.h>

const int CONDITION_TYPE_ADD_RESOURCE           = 1;
const int CONDITION_TYPE_ADD_FULL_RESOURCE      = 2;
const int CONDITION_TYPE_BOOST_RESOURCE         = 3;
const int CONDITION_TYPE_REGEN_RESOURCE         = 4;
const int CONDITION_TYPE_SET_RESOURCE           = 9;

const int CONDITION_TYPE_ADD_PARAM              = 5;

const int CONDITION_TYPE_ADD_STATUS             = 6;
const int CONDITION_TYPE_REMOVE_STATUS          = 7;

const int CONDITION_TYPE_CHANGE_MOVETYPE_SPEED  = 8;


class ConditionPrototype
{
    public:
        ConditionPrototype(Game* game);
        virtual ~ConditionPrototype();

        void loadFromXml(TiXmlElement* xml);
        void saveToXml(TiXmlElement* elem);

        void setParamsFromCharacters(Character* inflictor, Character* bearer);
        void setName(const char* name);
		char* getName();

        int getType(); void setType(int type);
        int getParam(); void setParam(int param);
        float getValue(), getDuration(), getInterval();
        const char* getValueFormula(); const char* getDurationFormula(); const char* getIntervalFormula();
        void setValueFormula(char* formula), setDurationFormula(char* formula), setIntervalFormula(char* formula);

        Condition* spawnCondition(Character* inflictor, Character* bearer);
    protected:
        Game* game;
        int type; int param;
        float* inflictor_params; float* bearer_params; float* inflictor_resources; float* bearer_resources;
        mu::Parser valueParser, durationParser, intervalParser;
        char* name;
    private:
};

#endif // CONDITION_PROTOTYPE_H
