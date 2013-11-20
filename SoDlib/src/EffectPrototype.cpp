#include "EffectPrototype.h"

EffectPrototype::EffectPrototype(Game* game)
{
	this->game = game;
}

EffectPrototype::~EffectPrototype()
{
	//dtor
}

void EffectPrototype::loadFromXml(TiXmlElement* xml)
{
	if (xml->Attribute("speed_formula")) {
		speedFormula = xml->Attribute("speed_formula");
	} else {
		speedFormula = "";
	}
	if (xml->Attribute("angle_formula")) {
		angleFormula = xml->Attribute("angle_formula");
	} else {
		angleFormula = "";
	}
	if (xml->Attribute("r_formula")) {
		rFormula = xml->Attribute("r_formula");
	} else {
		rFormula = "";
	}
	if (xml->Attribute("g_formula")) {
		gFormula = xml->Attribute("g_formula");
	} else {
		gFormula = "";
	}
	if (xml->Attribute("b_formula")) {
		bFormula = xml->Attribute("b_formula");
	} else {
		bFormula = "";
	}
	if (xml->Attribute("a_formula")) {
		aFormula = xml->Attribute("a_formula");
	} else {
		aFormula = "";
	}
	if (xml->Attribute("time_formula")) {
		timeFormula = xml->Attribute("time_formula");
	} else {
		timeFormula = "";
	}
	if (xml->Attribute("amount_formula")) {
		amountFormula = xml->Attribute("amount_formula");
	} else {
		amountFormula = "";
	}
	if (xml->Attribute("duration_formula")) {
		durationFormula = xml->Attribute("duration_formula");
	} else {
		durationFormula = "";
	}


    startSpeedExpr.register_symbol_table(symbolTable);
    startAngleExpr.register_symbol_table(symbolTable);
    startRExpr.register_symbol_table(symbolTable);
    startGExpr.register_symbol_table(symbolTable);
    startBExpr.register_symbol_table(symbolTable);
    startAExpr.register_symbol_table(symbolTable);
    startTimeExpr.register_symbol_table(symbolTable);
    startAmountExpr.register_symbol_table(symbolTable);
    startDurationExpr.register_symbol_table(symbolTable);

    startXExpr.register_symbol_table(symbolTable);
    startYExpr.register_symbol_table(symbolTable);

	if (xml->Attribute("start_speed_formula")) {
        game->getExprtkParser()->compile(xml->Attribute("start_speed_formula"), startSpeedExpr);
	} else {
		game->getExprtkParser()->compile("0", startSpeedExpr);
	}
	if (xml->Attribute("start_angle_formula")) {
	    game->getExprtkParser()->compile(xml->Attribute("start_angle_formula"), startAngleExpr);
	} else {
		game->getExprtkParser()->compile("0", startAngleExpr);
	}
	if (xml->Attribute("start_r_formula")) {
	    game->getExprtkParser()->compile(xml->Attribute("start_r_formula"), startRExpr);
	} else {
		game->getExprtkParser()->compile("0", startRExpr);
	}
	if (xml->Attribute("start_g_formula")) {
	    game->getExprtkParser()->compile(xml->Attribute("start_g_formula"), startGExpr);
	} else {
		game->getExprtkParser()->compile("0", startGExpr);
	}
	if (xml->Attribute("start_b_formula")) {
	    game->getExprtkParser()->compile(xml->Attribute("start_b_formula"), startBExpr);
	} else {
		game->getExprtkParser()->compile("0", startBExpr);
	}
	if (xml->Attribute("start_a_formula")) {
	    game->getExprtkParser()->compile(xml->Attribute("start_a_formula"), startAExpr);
	} else {
		game->getExprtkParser()->compile("0", startAExpr);
	}
	if (xml->Attribute("start_time_formula")) {
	    game->getExprtkParser()->compile(xml->Attribute("start_time_formula"), startTimeExpr);
	} else {
		game->getExprtkParser()->compile("0", startTimeExpr);
	}
	if (xml->Attribute("start_amount_formula")) {
	    game->getExprtkParser()->compile(xml->Attribute("start_amount_formula"), startAmountExpr);
	} else {
		game->getExprtkParser()->compile("0", startAmountExpr);
	}
	if (xml->Attribute("start_duration_formula")) {
	    game->getExprtkParser()->compile(xml->Attribute("start_duration_formula"), startDurationExpr);
	} else {
		game->getExprtkParser()->compile("0", startDurationExpr);
	}

	if (xml->Attribute("start_x_formula")) {
	    game->getExprtkParser()->compile(xml->Attribute("start_x_formula"), startXExpr);
	} else {
		game->getExprtkParser()->compile("0", startXExpr);
	}
	if (xml->Attribute("start_y_formula")) {
	    game->getExprtkParser()->compile(xml->Attribute("start_y_formula"), startYExpr);
	} else {
		game->getExprtkParser()->compile("0", startYExpr);
	}
}
