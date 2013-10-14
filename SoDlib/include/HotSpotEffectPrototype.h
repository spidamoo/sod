#include <SoDlib.h>
#ifdef EFFECTPROTOTYPE_H_COMPLETE
#ifndef HOTSPOTEFFECTPROTOTYPE_H
#define HOTSPOTEFFECTPROTOTYPE_H


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
#endif
