#include "EffectPrototype.h"


EffectPrototype::EffectPrototype(Game* game) {
	this->game = game;

	animationsCount = actionsCount = positionType = areaType = 0;

	name = "<effect>";

	expressionParsers      = new mu::Parser[EFFECT_FUNCTIONS_COUNT];
    startExpressionParsers = new mu::Parser[EFFECT_FUNCTIONS_COUNT];
    expressionExists = new bool[EFFECT_FUNCTIONS_COUNT];

    for ( int i = 0; i < EFFECT_FUNCTIONS_COUNT; i++ ) {
        expressionParsers[i].DefineFun("rand", frand);
        expressionExists[i] = false;
        startExpressionParsers[i].DefineFun("rand", frand);
    }

    startExpressionParsers[EFFECT_FUNCTION_TIME].SetExpr("1");
    startExpressionParsers[EFFECT_FUNCTION_XSPEED].SetExpr("0");
    startExpressionParsers[EFFECT_FUNCTION_YSPEED].SetExpr("0");
    startExpressionParsers[EFFECT_FUNCTION_ANGLE].SetExpr("0");

    startExpressionParsers[EFFECT_FUNCTION_R].SetExpr("255");
    startExpressionParsers[EFFECT_FUNCTION_G].SetExpr("255");
    startExpressionParsers[EFFECT_FUNCTION_B].SetExpr("255");
    startExpressionParsers[EFFECT_FUNCTION_A].SetExpr("255");

    startExpressionParsers[EFFECT_FUNCTION_SCALE].SetExpr("1");

    params = new float[EFFECT_PARAMS_COUNT];

    hotSpotName = "";

    positionType = EFFECT_POSITION_TYPE_STATIC;
    areaType = EFFECT_AREA_TYPE_POINT;
    blendMode = BLEND_DEFAULT;

    animationsCount = 0;
    animations = new char*[animationsCount];

    actionsCount = 0;
    actions = new EffectAction*[actionsCount];

}

EffectPrototype::~EffectPrototype() {
    delete name;

	delete expressionParsers;
	delete startExpressionParsers;
	delete expressionExists;

	delete params;

	delete hotSpotName;

	for (int i = 0; i < animationsCount; i++) {
        delete animations[i];
	}
	delete animations;

	for (int i = 0; i < actionsCount; i++) {
        delete actions[i];
	}
	delete actions;
}

void EffectPrototype::loadFromXml(TiXmlElement* xml) {
    for ( int i = 0; i < EFFECT_FUNCTIONS_COUNT; i++ ) {
        char formulaAttribName[20];
        sprintf( formulaAttribName, "%s_formula", EFFECT_FUNCTION_NAMES[i] );

        if ( xml->Attribute(formulaAttribName) ) {
            expressionParsers[i].SetExpr( xml->Attribute(formulaAttribName) );
            for ( int j = 0; j < EFFECT_PARAMS_COUNT; j++ ) {
                expressionParsers[i].DefineVar(EFFECT_PARAM_NAMES[j], &params[j]);
            }

            expressionParsers[i].DefineFun("rand", frand);

            expressionExists[i] = true;
            printf("%s formula is <%s>\n", EFFECT_FUNCTION_NAMES[i], xml->Attribute(formulaAttribName));
        } else {
            expressionParsers[i].SetExpr("0");
            expressionParsers[i].DefineFun("rand", frand);
            expressionExists[i] = false;
        }

        char startFormulaAttribName[30];
        sprintf( startFormulaAttribName, "start_%s_formula", EFFECT_FUNCTION_NAMES[i] );

        if ( xml->Attribute(startFormulaAttribName) ) {
            startExpressionParsers[i].SetExpr( xml->Attribute(startFormulaAttribName) );
            startExpressionParsers[i].DefineFun("rand", frand);
            printf("start %s formula is <%s>\n", EFFECT_FUNCTION_NAMES[i], xml->Attribute(startFormulaAttribName));
        } else {
            startExpressionParsers[i].SetExpr("0");
            startExpressionParsers[i].DefineFun("rand", frand);
        }
    }

    if (xml->Attribute("name")) {
        name = copyString(xml->Attribute("name"));
	}

	if (xml->Attribute("position_type")) {
        positionType = atoi(xml->Attribute("position_type"));
	} else {
	    positionType = EFFECT_POSITION_TYPE_STATIC;
	}
	if (xml->Attribute("area_type")) {
        areaType = atoi(xml->Attribute("area_type"));
	} else {
	    areaType = EFFECT_AREA_TYPE_POINT;
	}
	if (xml->Attribute("hot_spot_name")) {
        hotSpotName = copyString(xml->Attribute("hot_spot_name"));
	} else {
	    hotSpotName = "";
	}

	if (xml->Attribute("blend_mode")) {
        blendMode = atoi(xml->Attribute("blend_mode"));
	} else {
	    blendMode = BLEND_DEFAULT;
	}

    if (xml->Attribute("animations_count")) {
        animationsCount = atoi( xml->Attribute("animations_count") );
        animations = new char*[animationsCount];

        TiXmlElement* element = xml->FirstChildElement("animation");
        int i = 0;
        while (element) {
            animations[i] = copyString( element->Attribute("file") );

            i++;
            element = element->NextSiblingElement("animation");
        }
        animationsCount = i;
	} else {
	    animationsCount = 0;
	    animations = NULL;
	}

	if (xml->Attribute("actions_count")) {
        actionsCount = atoi( xml->Attribute("actions_count") );
        actions = new EffectAction*[actionsCount];

        TiXmlElement* actionElement = xml->FirstChildElement("action");
        int i = 0;
        while (actionElement) {
            EffectAction* newObject = new EffectAction();
            newObject->loadFromXml(actionElement);
            actions[i] = newObject;

            i++;
            actionElement = actionElement->NextSiblingElement("action");
        }
        actionsCount = i;
	} else {
	    actionsCount = 0;
	    actions = NULL;
	}
}
void EffectPrototype::saveToXml(TiXmlElement* xml) {
    for ( int i = 0; i < EFFECT_FUNCTIONS_COUNT; i++ ) {
        char formulaAttribName[20];
        sprintf( formulaAttribName, "%s_formula", EFFECT_FUNCTION_NAMES[i] );

        if (expressionExists[i]) {
            char* f = copyString(getFunction(i));
            xml->SetAttribute(formulaAttribName, f);
            delete f;
        }

        char startFormulaAttribName[30];
        sprintf( startFormulaAttribName, "start_%s_formula", EFFECT_FUNCTION_NAMES[i] );

        char* sf = copyString(getStartFunction(i));
        xml->SetAttribute(startFormulaAttribName, sf);
        delete sf;
    }

    xml->SetAttribute("name", name);
    xml->SetAttribute("position_type", positionType);
    xml->SetAttribute("area_type", areaType);
    xml->SetAttribute("hot_spot_name", hotSpotName);
    xml->SetAttribute("blend_mode", blendMode);
    xml->SetAttribute("animations_count", animationsCount);
    for (int i = 0; i < animationsCount; i++) {
        TiXmlElement* element = new TiXmlElement( "animation" );
        element->SetAttribute("file", animations[i]);
        xml->LinkEndChild(element);
    }
    xml->SetAttribute("actions_count", actionsCount);
    for (int i = 0; i < actionsCount; i++) {
        TiXmlElement* element = new TiXmlElement( "action" );
        actions[i]->saveToXml(element);
        xml->LinkEndChild(element);
    }
}

Effect* EffectPrototype::spawnEffect(Character* character) {
    Effect* newObject = new Effect(game, this);
    newObject->setOwner(character);
    newObject->setPosition( character->getPosition() );
    newObject->setHotSpotIndex( character->getHotSpotIndex(hotSpotName) );
    return newObject;
}
Effect* EffectPrototype::spawnEffect(Effect* effect) {
    clock_t st = clock();
    Effect* newObject = new Effect(game, this);
    newObject->setOwner( effect->getOwner() );
    newObject->setPosition( effect->getPosition() );
    //printf("new effect position is %f %f\n", effect->getPosition().x, effect->getPosition().y);
    clock_t et = clock();
    game->updateCounter(COUNTER_EFFECTS_SPAWN, (float)(et - st));
    return newObject;
}

int EffectPrototype::getPositionType() {
    return positionType;
}
int EffectPrototype::getAreaType() {
    return areaType;
}
char* EffectPrototype::getHotSpotName() {
    return hotSpotName;
}

void EffectPrototype::setPositionType(int _type) {
    positionType = _type;
}
void EffectPrototype::setAreaType(int _type) {
    areaType = _type;
}
void EffectPrototype::setHotSpotName(char* name) {
    delete hotSpotName;
    hotSpotName = copyString(name);
}

int EffectPrototype::getActionsCount() {
    return actionsCount;
}
EffectAction* EffectPrototype::getAction(int index) {
    return actions[index];
}
void EffectPrototype::addAction() {
    EffectAction** _actions = new EffectAction*[actionsCount + 1];
    for (int i = 0; i < actionsCount; i++) {
        _actions[i] = actions[i];
    }

    delete actions;
    actions = _actions;

    actions[actionsCount] = new EffectAction();
    actionsCount++;
}
void EffectPrototype::removeAction(int index) {
    delete actions[index];

    for (int i = index; i < actionsCount - 1; i++) {
        actions[i] = actions[i + 1];
    }

    actionsCount--;
}

float EffectPrototype::evalExpression(int ident) {
    if (expressionExists[ident]) {
        return expressionParsers[ident].Eval();
    }
    return 0.0f;
}

float EffectPrototype::evalStartExpression(int ident) {
    return startExpressionParsers[ident].Eval();
}

bool EffectPrototype::getExpressionExists(int index) {
    return expressionExists[index];
}
void EffectPrototype::setExpressionExists(int ident, bool exists) {
    expressionExists[ident] = exists;
}

void EffectPrototype::setParam(int index, float value) {
//    printf("param %s was %f set to %f\n", EFFECT_PARAM_NAMES[index], params[index], value);
    params[index] = value;
}

char* EffectPrototype::getName() {
    return name;
}
void EffectPrototype::setName(const char* _name) {
    if (name)
        delete name;
    name = copyString(_name);
}

const char* EffectPrototype::getFunction(int ident) {
    if (!expressionExists[ident])
        return NULL;
    printf("<%s>\n", expressionParsers[ident].GetExpr().data());
    return expressionParsers[ident].GetExpr().data();
}
const char* EffectPrototype::getStartFunction(int ident) {
    return startExpressionParsers[ident].GetExpr().data();
}

void EffectPrototype::setFunction(int ident, const char* expression) {
    expressionParsers[ident].SetExpr( expression );
}
void EffectPrototype::setStartFunction(int ident, const char* expression) {
    startExpressionParsers[ident].SetExpr( expression );
}

const char* EffectPrototype::getAnimation(int index) {
    return animations[index];
}
int EffectPrototype::getAnimationsCount() {
    return animationsCount;
}
void EffectPrototype::addAnimation(const char* filename) {
    char** _animations = new char*[animationsCount + 1];
    for (int i = 0; i < animationsCount; i++) {
        _animations[i] = animations[i];
    }
    _animations[animationsCount] = copyString(filename);
    delete animations;
    animations = _animations;
    animationsCount++;

}
void EffectPrototype::removeAnimation(int index) {
    delete animations[index];
    for (int i = index; i < animationsCount - 1; i++) {
        animations[i] = animations[i + 1];
    }
    animationsCount--;
}
int EffectPrototype::getBlendMode() {
    return blendMode;
}
void EffectPrototype::setBlendMode(int mode) {
    blendMode = mode;
}
