#include "HotSpotEffectPrototype.h"

HotSpotEffectPrototype::HotSpotEffectPrototype()
{
	//ctor
}

HotSpotEffectPrototype::~HotSpotEffectPrototype()
{
	//dtor
}

void HotSpotEffectPrototype::loadFromXml(TiXmlElement* xml)
{
	EffectPrototype::loadFromXml(xml);
	if (xml->Attribute("hot_spot_type")) {
		hotSpotType = atoi(xml->Attribute("hot_spot_type"));
	} else {
		hotSpotType = HOT_SPOT_TYPE_WEAPON;
	}
}
