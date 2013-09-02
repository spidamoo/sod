#ifndef ANIMATION_H
#define ANIMATION_H

#include <SoDlib.h>

class Animation
{
	public:
		Animation();
		virtual ~Animation();

		void load(char* fn);
	protected:
		float** x;
		float** y;
		float** angle;
		int framesCount;
		int bodiesCount;
	private:
};

#endif // ANIMATION_H
