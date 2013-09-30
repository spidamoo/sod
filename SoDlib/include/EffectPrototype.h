#ifndef EFFECTPROTOTYPE_H
#define EFFECTPROTOTYPE_H

#include <SoDlib.h>

class EffectPrototype
{
	public:
		EffectPrototype();
		virtual ~EffectPrototype();
	protected:
		std::string speedFormula, angleFormula, rFormula, gFormula, bFormula, aFormula, timeFormula, amountFormula, durationFormula;
	private:
};

#endif // EFFECTPROTOTYPE_H
