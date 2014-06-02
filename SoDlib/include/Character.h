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
		void updateParams(Condition* condition); void updateStatus();

		void turn();
		void run(float speed);
		void jump(b2Vec2 speed);
		void move(float dx, float dy);

		void setMove(int anim);

		b2Vec2 getPosition();
		void setPosition(b2Vec2 newPos);
		float getX();
		float getY();
		float getHalfHeight();
		float getHalfWidth();
		int getOnGround();
		int getDirection();
		float getAngle();

		float getHotSpotX(int index), getHotSpotY(int index); int getHotSpotIndex(char* name);

		float getAnimTime();

        int getConditionsCount(); Condition* getCondition(int index);
		void addCondition(Condition* condition); void removeCondition(int index);

        CharacterParam* getParam(int index);
        CharacterResource* getResource(int index);

        void setStatusAction(int status, CharacterAction* action), setResourceLessAction(int resource, float value, CharacterAction* action);
	protected:
	    float animatedValue(float prev, float next), midanglePosition(float prev, float next), midangleAngle(float prev, float next);

		Game* game;

		b2Vec2 position, prevPosition, speed;
		float angle;

		hgeAnimation** animations;
		CharacterAction*** actions; int* actionsCounts;
		CharacterHotSpot** hotSpots;

		float*** frameAnimX;
		float*** frameAnimY;
		float*** frameAnimAngle;
		float*** frameAnimScaleX = new float**[256];
        float*** frameAnimScaleY = new float**[256];
		int*** frameAnimLayer;
		bool*** frameAnimShow;

		float*** frameHotSpotX;
		float*** frameHotSpotY;
		float*** frameHotSpotAngle;
		bool*** frameHotSpotShow;

		float*** angleAnimX; float*** angleAnimY; float*** angleAnimAngle;
		float*** angleHotSpotX; float*** angleHotSpotY; float*** angleHotSpotAngle;
		bool*** angleHotSpotShow;
		bool*** angleAnimShow;
		int* angleCounts; float** angles;
		bool** animRotating, hotSpotRotating;

		float height, width, halfHeight, halfWidth;
		int* framesCounts;
		float** framesLengths;
		float* movePriorities;
		int currentFrame, nextFrame;
		int movesCount;
		int currentMove;
		float currentTime;
		int prevAngle, nextAngle;
		float frameProgress, angleProgress;

		int bodiesCount;
		int hotSpotsCount;

		bool turnedRight;
		int onGround;

		Condition** conditions; int conditionsCount;

		CharacterParam** params;
		CharacterResource** resources;

		CharacterAction** statusActions;
		int currentStatus; float statusPriority;
};
#define CHARACTER_H_COMPLETE
#endif // CHARACTER_H
