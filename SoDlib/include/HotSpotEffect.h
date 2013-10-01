#ifndef HOTSPOTEFFECT_H
#define HOTSPOTEFFECT_H

#include <SoDlib.h>


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
