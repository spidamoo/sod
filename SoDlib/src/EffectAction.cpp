#include "EffectAction.h"

EffectAction::EffectAction() {
    type = EFFECTACTION_TYPE_DESTRUCT;
    param = 0;
    interval = 0.0f;
    startTime = 0.0f;
    condition = EFFECTACTION_CONDITION_NONE;
    targets = 0;
    maxInteractions = 1;
}
EffectAction::~EffectAction() {
    //dtor
}

void EffectAction::loadFromXml(TiXmlElement* xml) {

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
void EffectAction::saveToXml(TiXmlElement* xml)
{
    xml->SetAttribute("type", type);
    xml->SetAttribute("param", param);
    xml->SetDoubleAttribute("interval", interval);
    xml->SetAttribute("condition", condition);
    xml->SetAttribute("targets", targets);
    xml->SetDoubleAttribute("start_time", startTime);
    xml->SetAttribute("max_interactions", maxInteractions);
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

int EffectAction::getMaxInteractions() {
    return maxInteractions;
}

void EffectAction::setType(int _type) {
    type = _type;
}
void EffectAction::setParam(int _param) {
    param = _param;
}
void EffectAction::setInterval(float _interval) {
    interval = _interval;
}
void EffectAction::setStartTime(float _startTime) {
    startTime = _startTime;
}
void EffectAction::setCondition(int _condition) {
    condition = _condition;
}
void EffectAction::setTargets(int _targets) {
    targets = _targets;
}
void EffectAction::setTarget(int _target, bool _active) {
    if (_active) {
        targets = (targets | _target);
    }
    else {
        targets = (targets & ~_target);
    }
}
void EffectAction::setMaxInteractions(int _maxInteractions) {
    maxInteractions = _maxInteractions;
}
