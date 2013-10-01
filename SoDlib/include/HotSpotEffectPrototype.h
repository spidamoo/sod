#ifndef HOTSPOTEFFECTPROTOTYPE_H
#define HOTSPOTEFFECTPROTOTYPE_H

#include <SoDlib.h>


class HotSpotEffectPrototype : public EffectPrototype
{
	public:
		HotSpotEffectPrototype();
		virtual ~HotSpotEffectPrototype();

		void loadFromXml(TiXmlElement* xml);
	protected:
		int hotSpotType;
	private:
};

#endif // HOTSPOTEFFECTPROTOTYPE_H
