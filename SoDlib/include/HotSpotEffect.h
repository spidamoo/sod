#include <SoDlib.h>
#ifdef CHARACTER_H_COMPLETE
#ifndef HOTSPOTEFFECT_H
#define HOTSPOTEFFECT_H


class HotSpotEffect : public Effect
{
	public:
		HotSpotEffect();
		virtual ~HotSpotEffect();
	protected:
		int character, hotSpot;
	private:
};

#endif // HOTSPOTEFFECT_H
#endif
