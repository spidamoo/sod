#include "EffectPrototype.h"

EffectPrototype::EffectPrototype()
{
	//ctor
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



	if (xml->Attribute("start_speed_formula")) {
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
}
