#include "ConditionPrototype.h"

ConditionPrototype::ConditionPrototype()
{
    type = CONDITION_TYPE_DAMAGE;

    params = new float[CONDITION_PROTOTYPE_PARAMS_COUNT];

    name = "<condition>";
}

ConditionPrototype::~ConditionPrototype()
{
    //dtor
}

void ConditionPrototype::loadFromXml(TiXmlElement* xml) {
    printf("loading condition ");
    if ( xml->Attribute("value_formula") ) {
        valueParser.SetExpr( xml->Attribute("value_formula") );
        for ( int j = 0; j < CONDITION_PROTOTYPE_PARAMS_COUNT; j++ ) {
            valueParser.DefineVar(CONDITION_PROTOTYPE_PARAM_NAMES[j], &params[j]);
        }

        valueParser.DefineFun("rand", frand);
    } else {
        valueParser.SetExpr("0");
    }

    if ( xml->Attribute("duration_formula") ) {
        durationParser.SetExpr( xml->Attribute("duration_formula") );
        for ( int j = 0; j < CONDITION_PROTOTYPE_PARAMS_COUNT; j++ ) {
            durationParser.DefineVar(CONDITION_PROTOTYPE_PARAM_NAMES[j], &params[j]);
        }

        durationParser.DefineFun("rand", frand);
    } else {
        durationParser.SetExpr("0");
    }

    if ( xml->Attribute("interval_formula") ) {
        intervalParser.SetExpr( xml->Attribute("interval_formula") );
        for ( int j = 0; j < CONDITION_PROTOTYPE_PARAMS_COUNT; j++ ) {
            intervalParser.DefineVar(CONDITION_PROTOTYPE_PARAM_NAMES[j], &params[j]);
        }

        intervalParser.DefineFun("rand", frand);
    } else {
        intervalParser.SetExpr("0");
    }

	if (xml->Attribute("type")) {
        type = atoi(xml->Attribute("type"));
        printf("type %d ", type);
	} else {
	    type = CONDITION_TYPE_DAMAGE;
	}

	if (xml->Attribute("name")) {
        name = copyString(xml->Attribute("name"));
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
}

void ConditionPrototype::setParam(int index, float value)
{
    params[index] = value;
}
void ConditionPrototype::setParamsFromCharacter(Character* character)
{
    setParam( CONDITION_PROTOTYPE_PARAM_DAMAGE, character->getDamage() );
}

int ConditionPrototype::getType() {
    return type;
}
void ConditionPrototype::setType(int _type) {
    type = _type;
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


Condition* ConditionPrototype::spawnCondition(Character* inflictor, Character* bearer)
{
//    printf("spawning condition\n");
    Condition* newObject = new Condition(this, inflictor, bearer);
    newObject->initialize();
    return newObject;
}
