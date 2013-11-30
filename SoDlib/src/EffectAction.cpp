#include "EffectAction.h"

EffectAction::EffectAction()
{
    //ctor
}

EffectAction::~EffectAction()
{
    //dtor
}

void EffectAction::loadFromXml(TiXmlElement* xml)
{

    if ( xml->Attribute("type") ) {
        type = atoi( xml->Attribute("type") );
    } else {
        type = 0;
    }
    if ( xml->Attribute("param") ) {
        param = atoi( xml->Attribute("param") );
    } else {
        param = 0;
    }
    if ( xml->Attribute("interval") ) {
        interval = atof( xml->Attribute("interval") );
    } else {
        interval = 0.0f;
    }
    if ( xml->Attribute("condition") ) {
        condition = atoi( xml->Attribute("condition") );
    } else {
        condition = EFFECTACTION_CONDITION_NONE;
    }
    if ( xml->Attribute("targets") ) {
        targets = atoi( xml->Attribute("targets") );
    } else {
        targets = 0;
    }
    if ( xml->Attribute("start_time") ) {
        startTime = atof( xml->Attribute("start_time") );
    } else {
        startTime = 0.0f;
    }
    if ( xml->Attribute("max_interactions") ) {
        maxInteractions = atoi( xml->Attribute("max_interactions") );
    } else {
        maxInteractions = 1;
    }

}

float EffectAction::getStartTime() {
    return startTime;
}

float EffectAction::getInterval() {
    return interval;
}

int EffectAction::getCondition() {
    return condition;
}
int EffectAction::getTargets() {
    return targets;
}

int EffectAction::getType() {
    return type;
}
int EffectAction::getParam() {
    return param;
}

int EffectAction::getMaxInteractions()
{
    return maxInteractions;
}
