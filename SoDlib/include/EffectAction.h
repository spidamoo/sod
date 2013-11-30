#ifndef EFFECTACTION_H
#define EFFECTACTION_H

#include <SoDlib.h>

const int EFFECTACTION_TYPE_SPAWN_EFFECT      = 1;
const int EFFECTACTION_TYPE_DESTRUCT          = 2;
const int EFFECTACTION_TYPE_INFLICT_CONDITION = 3;

const int EFFECTACTION_CONDITION_NONE              = 0;
const int EFFECTACTION_CONDITION_CROSS_GROUND      = 1;
const int EFFECTACTION_CONDITION_CROSS_ENEMY_BODY  = 2;
const int EFFECTACTION_CONDITION_CROSS_FRIEND_BODY = 3;

const int EFFECTACTION_TARGET_FRIEND = 1;
const int EFFECTACTION_TARGET_ENEMY  = 2;

class EffectAction
{
    public:
        EffectAction();
        virtual ~EffectAction();

        void loadFromXml(TiXmlElement* xml);
        float getStartTime();
        float getInterval();
        int getCondition();
        int getTargets();
        int getType(); int getParam();
        int getMaxInteractions();
    protected:
        int type; int param;
        float interval;
        int condition;
        float startTime;
        int targets;
        int maxInteractions;
    private:
};

#endif // EFFECTACTION_H
