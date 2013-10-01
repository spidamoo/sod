#ifndef EFFECTPROTOTYPE_H
#define EFFECTPROTOTYPE_H

#include <SoDlib.h>

const int EFFECT_TYPE_RECTANGULAR = 1;
const int EFFECT_TYPE_HOTSPOT = 2;

class EffectPrototype
{
	public:
		EffectPrototype();
		virtual ~EffectPrototype();

		void loadFromXml(TiXmlElement* xml);
	protected:
		std::string speedFormula, angleFormula, rFormula, gFormula, bFormula, aFormula, timeFormula, amountFormula, durationFormula;

		int type;
	private:
};

#endif // EFFECTPROTOTYPE_H
