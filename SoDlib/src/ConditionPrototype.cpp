#include "ConditionPrototype.h"

ConditionPrototype::ConditionPrototype(Game* _game)
{
    game = _game;
    type = CONDITION_TYPE_ADD_RESOURCE;
    param = 0;

    inflictor_params    = new float[game->getCharacterParamPrototypesCount()];
    bearer_params       = new float[game->getCharacterParamPrototypesCount()];
    inflictor_resources = new float[game->getCharacterResourcePrototypesCount()];
    bearer_resources    = new float[game->getCharacterResourcePrototypesCount()];

    name = "<condition>";
}

ConditionPrototype::~ConditionPrototype()
{
    delete name;
    delete inflictor_params;
    delete bearer_params;
    delete inflictor_resources;
    delete bearer_resources;
}

void ConditionPrototype::loadFromXml(TiXmlElement* xml) {
    printf("loading condition ");
    if ( xml->Attribute("value_formula") ) {
        valueParser.SetExpr( xml->Attribute("value_formula") );
        valueParser.DefineFun("rand", frand);
    } else {
        valueParser.SetExpr("0");
    }

    if ( xml->Attribute("duration_formula") ) {
        durationParser.SetExpr( xml->Attribute("duration_formula") );
        durationParser.DefineFun("rand", frand);
    } else {
        durationParser.SetExpr("0");
    }

    if ( xml->Attribute("interval_formula") ) {
        intervalParser.SetExpr( xml->Attribute("interval_formula") );
        intervalParser.DefineFun("rand", frand);
    } else {
        intervalParser.SetExpr("0");
    }

    char buffer[64];
    for (int i = 0; i < game->getCharacterParamPrototypesCount(); i++) {
        sprintf(buffer, "inflictor_%s", game->getCharacterParamPrototype(i)->getName());
        valueParser.DefineVar(buffer, &inflictor_params[i]);
        durationParser.DefineVar(buffer, &inflictor_params[i]);
        intervalParser.DefineVar(buffer, &inflictor_params[i]);

        sprintf(buffer, "bearer_%s", game->getCharacterParamPrototype(i)->getName());
        valueParser.DefineVar(buffer, &bearer_params[i]);
        durationParser.DefineVar(buffer, &bearer_params[i]);
        intervalParser.DefineVar(buffer, &bearer_params[i]);
    }
    for (int i = 0; i < game->getCharacterResourcePrototypesCount(); i++) {
        sprintf(buffer, "inflictor_%s", game->getCharacterResourcePrototype(i)->getName());
        valueParser.DefineVar(buffer, &inflictor_resources[i]);
        durationParser.DefineVar(buffer, &inflictor_resources[i]);
        intervalParser.DefineVar(buffer, &inflictor_resources[i]);

        sprintf(buffer, "bearer_%s", game->getCharacterResourcePrototype(i)->getName());
        valueParser.DefineVar(buffer, &bearer_resources[i]);
        durationParser.DefineVar(buffer, &bearer_resources[i]);
        intervalParser.DefineVar(buffer, &bearer_resources[i]);
    }


	if (xml->Attribute("type")) {
        type = atoi(xml->Attribute("type"));
        printf("type %d ", type);
	}

	if (xml->Attribute("name")) {
        name = copyString(xml->Attribute("name"));
	}

	if (xml->Attribute("param")) {
        param = atoi(xml->Attribute("param"));
	}

	printf(" done\n");

}
void ConditionPrototype::saveToXml(TiXmlElement* xml) {
    char* f = copyString( getValueFormula() );
    xml->SetAttribute("value_formula", f);
    delete f;

    f = copyString( getDurationFormula() );
    xml->SetAttribute("duration_formula", f);
    delete f;

    f = copyString( getIntervalFormula() );
    xml->SetAttribute("interval_formula", f);
    delete f;

    xml->SetAttribute("type", type);
    xml->SetAttribute("name", name);
    xml->SetAttribute("param", param);
}

void ConditionPrototype::setParamsFromCharacters(Character* inflictor, Character* bearer)
{
    for (int i = 0; i < game->getCharacterParamPrototypesCount(); i++) {
        inflictor_params[i] = inflictor->getParam(i)->getCurrentValue();
        bearer_params[i] = bearer->getParam(i)->getCurrentValue();
    }
    for (int i = 0; i < game->getCharacterResourcePrototypesCount(); i++) {
        inflictor_resources[i] = inflictor->getResource(i)->getCurrentValue();
        bearer_resources[i] = bearer->getResource(i)->getCurrentValue();
    }
}

int ConditionPrototype::getType() {
    return type;
}
void ConditionPrototype::setType(int _type) {
    type = _type;
}

int ConditionPrototype::getParam() {
    return param;
}
void ConditionPrototype::setParam(int _param) {
    param = _param;
}

float ConditionPrototype::getValue() {
    return valueParser.Eval();
}
float ConditionPrototype::getDuration() {
    return durationParser.Eval();
}
float ConditionPrototype::getInterval() {
    return intervalParser.Eval();
}
const char* ConditionPrototype::getValueFormula() {
    return valueParser.GetExpr().data();
}
const char* ConditionPrototype::getDurationFormula() {
    return durationParser.GetExpr().data();
}
const char* ConditionPrototype::getIntervalFormula() {
    return intervalParser.GetExpr().data();
}

void ConditionPrototype::setValueFormula(char* formula) {
    valueParser.SetExpr( formula );
}
void ConditionPrototype::setDurationFormula(char* formula) {
    durationParser.SetExpr( formula );
}
void ConditionPrototype::setIntervalFormula(char* formula) {
    intervalParser.SetExpr( formula );
}

char* ConditionPrototype::getName() {
    return name;
}
void ConditionPrototype::setName(const char* _name) {
    if (name)
        delete name;
    name = copyString(_name);
}


Condition* ConditionPrototype::spawnCondition(Character* inflictor, Character* bearer) {
    Condition* newObject = new Condition(this, inflictor, bearer);
    newObject->initialize();
    return newObject;
}
