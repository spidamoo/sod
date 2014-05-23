#ifndef CONDITION_PROTOTYPE_H
#define CONDITION_PROTOTYPE_H

#include <SoDlib.h>

const int CONDITION_TYPE_DAMAGE = 1;
const int CONDITION_TYPE_STUN   = 2;

const int CONDITION_PROTOTYPE_PARAM_DAMAGE    = 0;

const int CONDITION_PROTOTYPE_PARAMS_COUNT = 1;

static const char* CONDITION_PROTOTYPE_PARAM_NAMES[CONDITION_PROTOTYPE_PARAMS_COUNT] = {
    "damage"
};

class ConditionPrototype
{
    public:
        ConditionPrototype();
        virtual ~ConditionPrototype();

        void loadFromXml(TiXmlElement* xml);
        void saveToXml(TiXmlElement* elem);

        void setParam(int index, float value);
        void setParamsFromCharacter(Character* character);
        void setName(const char* name);
		char* getName();

        int getType(); void setType(int type);
        float getValue(), getDuration(), getInterval();
        const char* getValueFormula(); const char* getDurationFormula(); const char* getIntervalFormula();
        void setValueFormula(char* formula), setDurationFormula(char* formula), setIntervalFormula(char* formula);

        Condition* spawnCondition(Character* inflictor, Character* bearer);
    protected:
        int type;
        float* params;
        mu::Parser valueParser, durationParser, intervalParser;
        char* name;
    private:
};

#endif // CONDITION_PROTOTYPE_H
