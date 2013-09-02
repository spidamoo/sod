#ifndef CHARACTER_H
#define CHARACTER_H

#include <SoDlib.h>

const int CHARACTER_TYPE_PLAYER = 1;
const int CHARACTER_TYPE_NONPLAYER = 2;


class Character
{
    public:
        Character(TiXmlElement* xml, Game * game, b2Vec2 origin);
        virtual ~Character();

        virtual int getType()=0;

        bool loadFromXml(TiXmlElement* xml, b2Vec2 origin);

		void draw(bool schematicMode);
		virtual void control(float dt)=0;
		void update(float dt);

		void turn();
		void run(float speed);
		void jump(b2Vec2 speed);

		void setAnim(int anim);

		float animatedValue(float prev, float next);

		b2Vec2 getPosition();
		void setPosition(b2Vec2 newPos);
		float getX();
		float getY();
		float getHalfHeight();
		float getHalfWidth();
		int getOnGround();

	protected:
		Game* game;

		b2Vec2 position, prevPosition, speed;
		float angle;

		hgeAnimation** animations;
		CharacterAction*** actions;
		CharacterHotSpot** hotSpots;

		float*** frameAnimX;
		float*** frameAnimY;
		float*** frameAnimAngle;
		int*** frameAnimLayer;
		bool*** frameAnimShow;

		float*** angleAnimX, angleAnimY, angleAnimAngle;
		int*** angleAnimLayer; bool angleAnimShow;

		float*** frameHotSpotX;
		float*** frameHotSpotY;
		float*** frameHotSpotAngle;
		bool*** frameHotSpotShow;

		float height, width, halfHeight, halfWidth;
		int* framesCounts;
		float** framesLengths;
		int currentFrame;
		int nextFrame;
		int animationsCount;
		int currentAnimation;
		float currentTime;
		float frameProgress;

		int* actionsCounts;

		int bodiesCount;
		int hotSpotsCount;

		bool turnedRight;
		int onGround;
};
#define CHARACTER_H_COMPLETE
#endif // CHARACTER_H
