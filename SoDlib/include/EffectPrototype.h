#ifndef EFFECTPROTOTYPE_H
#define EFFECTPROTOTYPE_H

#include <SoDlib.h>

const int EFFECT_POSITION_TYPE_STATIC  = 1;
const int EFFECT_POSITION_TYPE_HOTSPOT = 2;
const int EFFECT_POSITION_TYPE_DYNAMIC = 3;

const int EFFECT_AREA_TYPE_POINT  = 1;
const int EFFECT_AREA_TYPE_RECT   = 2;
const int EFFECT_AREA_TYPE_CIRCLE = 3;

const int EFFECT_FUNCTION_SPEED    = 0;
const int EFFECT_FUNCTION_ANGLE    = 1;
const int EFFECT_FUNCTION_R        = 2;
const int EFFECT_FUNCTION_G        = 3;
const int EFFECT_FUNCTION_B        = 4;
const int EFFECT_FUNCTION_A        = 5;
const int EFFECT_FUNCTION_TIME     = 6;
const int EFFECT_FUNCTION_AMOUNT   = 7;
const int EFFECT_FUNCTION_DURATION = 8;
const int EFFECT_FUNCTION_X        = 9;
const int EFFECT_FUNCTION_Y        = 10;
const int EFFECT_FUNCTION_XSPEED   = 11;
const int EFFECT_FUNCTION_YSPEED   = 12;
const int EFFECT_FUNCTION_SCALE    = 13;

const int EFFECT_FUNCTIONS_COUNT = 14;

static const char* EFFECT_FUNCTION_NAMES[EFFECT_FUNCTIONS_COUNT] = {
    "speed", "angle", "r", "g", "b", "a", "time", "amount", "duration", "x", "y", "xspeed", "yspeed", "scale"
};

const int EFFECT_PARAM_SPEED    = 0;
const int EFFECT_PARAM_ANGLE    = 1;
const int EFFECT_PARAM_R        = 2;
const int EFFECT_PARAM_G        = 3;
const int EFFECT_PARAM_B        = 4;
const int EFFECT_PARAM_A        = 5;
const int EFFECT_PARAM_TIME     = 6;
const int EFFECT_PARAM_AMOUNT   = 7;
const int EFFECT_PARAM_DURATION = 8;
const int EFFECT_PARAM_X        = 9;
const int EFFECT_PARAM_Y        = 10;
const int EFFECT_PARAM_XSPEED   = 11;
const int EFFECT_PARAM_YSPEED   = 12;
const int EFFECT_PARAM_SCALE    = 13;
const int EFFECT_PARAM_DT       = 14;

const int EFFECT_PARAMS_COUNT = 15;

static const char* EFFECT_PARAM_NAMES[EFFECT_PARAMS_COUNT] = {
    "speed", "angle", "r", "g", "b", "a", "time", "amount", "duration", "x", "y", "xspeed", "yspeed", "scale", "dt"
};


class EffectPrototype
{
	public:
		EffectPrototype(Game* game);
		virtual ~EffectPrototype();

		void loadFromXml(TiXmlElement* xml);
		void saveToXml(TiXmlElement* elem);

		Effect* spawnEffect(Character* character);
		Effect* spawnEffect(Effect* effect);

		int getPositionType(), getAreaType(), getHotSpotIndex();
		void setPositionType(int type), setAreaType(int type);

		int getActionsCount(); EffectAction* getAction(int index);
		void addAction(), removeAction(int index);

        float evalStartExpression(int ident);
		float evalExpression(int ident);
		bool getExpressionExists(int ident);
		void setExpressionExists(int ident, bool exists);
		const char* getFunction(int ident);
		const char* getStartFunction(int ident);
		void setFunction(int ident, const char* expression);
		void setStartFunction(int ident, const char* expression);

		void setParam(int index, float value);
		void setName(const char* name);
		char* getName();

		const char* getAnimation(int index);
		int getAnimationsCount();
		void addAnimation(const char* filename), removeAnimation(int index);

		int getBlendMode(); void setBlendMode(int mode);
	protected:
	    Game* game;

	    char* name;

        mu::Parser* expressionParsers;
        mu::Parser* startExpressionParsers;
		bool* expressionExists;

		float* params;

		int positionType, areaType, hotSpotIndex;

		EffectAction** actions; int actionsCount;
		char** animations; int animationsCount; int blendMode;
	private:
};
#endif // EFFECTPROTOTYPE_H
