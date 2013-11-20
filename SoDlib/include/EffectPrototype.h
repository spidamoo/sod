#ifndef EFFECTPROTOTYPE_H
#define EFFECTPROTOTYPE_H

#include <SoDlib.h>

const int EFFECT_POSITION_TYPE_STATIC  = 1;
const int EFFECT_POSITION_TYPE_HOTSPOT = 2;
const int EFFECT_POSITION_TYPE_DYNAMIC = 3;

const int EFFECT_AREA_TYPE_POINT  = 1;
const int EFFECT_AREA_TYPE_RECT   = 2;
const int EFFECT_AREA_TYPE_CIRCLE = 3;

class EffectPrototype
{
	public:
		EffectPrototype(Game* game);
		virtual ~EffectPrototype();

		void loadFromXml(TiXmlElement* xml);
	protected:
	    Game* game;

		std::string speedFormula, angleFormula, rFormula, gFormula, bFormula, aFormula,
            timeFormula, amountFormula, durationFormula;

        exprtk::expression<float> startSpeedExpr, startAngleExpr, startRExpr, startGExpr, startBExpr,
            startAExpr, startTimeExpr, startAmountExpr, startDurationExpr, startXExpr, startYExpr;
        exprtk::symbol_table<float> symbolTable;

		int positionType, areaType;
	private:
};
#define EFFECTPROTOTYPE_H_COMPLETE
#endif // EFFECTPROTOTYPE_H
