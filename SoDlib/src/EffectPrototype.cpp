#include "EffectPrototype.h"


EffectPrototype::EffectPrototype(Game* game)
{
	this->game = game;

	animationsCount = actionsCount = positionType = areaType = hotSpotIndex = 0;
}

EffectPrototype::~EffectPrototype()
{
	//dtor
}

void EffectPrototype::loadFromXml(TiXmlElement* xml)
{
    expressionParsers      = new mu::Parser[EFFECT_FUNCTIONS_COUNT];
    startExpressionParsers = new mu::Parser[EFFECT_FUNCTIONS_COUNT];
    expressionExists = new bool[EFFECT_FUNCTIONS_COUNT];

    params = new float[EFFECT_PARAMS_COUNT];

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
            printf("%s formula is %s\n", EFFECT_FUNCTION_NAMES[i], xml->Attribute(formulaAttribName));
        } else {
            expressionParsers[i].SetExpr("0");
            expressionExists[i] = false;
        }

        char startFormulaAttribName[30];
        sprintf( startFormulaAttribName, "start_%s_formula", EFFECT_FUNCTION_NAMES[i] );

        if ( xml->Attribute(startFormulaAttribName) ) {
            startExpressionParsers[i].SetExpr( xml->Attribute(startFormulaAttribName) );
            startExpressionParsers[i].DefineFun("rand", frand);
            printf("start %s formula is %s\n", EFFECT_FUNCTION_NAMES[i], xml->Attribute(startFormulaAttribName));
        } else {
            startExpressionParsers[i].SetExpr("0");
        }
    }

	if (xml->Attribute("position_type")) {
        positionType = atoi(xml->Attribute("position_type"));
	} else {
	    positionType = EFFECT_POSITION_TYPE_STATIC;
	}
	if (xml->Attribute("area_type")) {
        areaType = atoi(xml->Attribute("area_type"));
	} else {
	    positionType = EFFECT_AREA_TYPE_POINT;
	}
	if (xml->Attribute("hot_spot_index")) {
        hotSpotIndex = atoi(xml->Attribute("hot_spot_index"));
	} else {
	    hotSpotIndex = -1;
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

Effect* EffectPrototype::spawnEffect(Character* character)
{
    Effect* newObject = new Effect(game, this);
    newObject->setOwner(character);
    newObject->setPosition( character->getPosition() );
    newObject->setAnimation( game->loadAnimation(animations[game->getHge()->Random_Int(1, animationsCount) - 1]), blendMode );
    return newObject;
}
Effect* EffectPrototype::spawnEffect(Effect* effect)
{
    clock_t st = clock();
    Effect* newObject = new Effect(game, this);
    newObject->setOwner( effect->getOwner() );
    newObject->setPosition( effect->getPosition() );
    newObject->setAnimation( game->loadAnimation( animations[game->getHge()->Random_Int(1, animationsCount) - 1] ), blendMode );
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
int EffectPrototype::getHotSpotIndex() {
    return hotSpotIndex;
}

int EffectPrototype::getActionsCount() {
    return actionsCount;
}
EffectAction* EffectPrototype::getAction(int index) {
    return actions[index];
}

float EffectPrototype::evalExpression(int ident)
{
    if (expressionExists[ident]) {
        return expressionParsers[ident].Eval();
    }
    return 0.0f;
}

float EffectPrototype::evalStartExpression(int ident)
{
    return startExpressionParsers[ident].Eval();
}

bool EffectPrototype::getExpressionExists(int index)
{
    return expressionExists[index];
}

void EffectPrototype::setParam(int index, float value) {
//    printf("param %s was %f set to %f\n", EFFECT_PARAM_NAMES[index], params[index], value);
    params[index] = value;
}
