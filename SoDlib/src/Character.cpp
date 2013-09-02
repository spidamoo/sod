#include "Character.h"

Character::Character(TiXmlElement* xml, Game * game, b2Vec2 origin)
{
	this->game = game;

	height = 2.0f;
	halfHeight = 1.0f;
	width = 0.5f;
	halfWidth = 0.25f;
	loadFromXml(xml, origin);

    currentAnimation = 0;
    currentFrame = 0;
    currentTime = 0;
    turnedRight = false;
    onGround = -1;

    speed = b2Vec2_zero;
    prevPosition = position;
    angle = 0.25f * M_PI;

    printf("character loaded\n");
}

Character::~Character()
{
    //dtor
}

bool Character::loadFromXml(TiXmlElement* xml, b2Vec2 origin)
{
	if (xml->Attribute("height")) {
		height = atof(xml->Attribute("height"));
		halfHeight = height * 0.5f;
	}
	if (xml->Attribute("width")) {
		width = atof(xml->Attribute("width"));
		halfWidth = width * 0.5f;
	}

	bodiesCount = atoi(xml->Attribute("bodies"));
	animations = new hgeAnimation*[bodiesCount];
	TiXmlElement* element = xml->FirstChildElement("body");
	int i = 0;
	while (element) {
		printf("loading body...\n");

		char* animationName = (char*)element->Attribute("animation");
		hgeAnimation* animation = game->loadAnimation(animationName);

		animations[i] = animation;

		i++;
		element = element->NextSiblingElement("body");
	}
	bodiesCount = i;

	hotSpotsCount = atoi(xml->Attribute("hotspots"));
	hotSpots = new CharacterHotSpot*[hotSpotsCount];

	element = xml->FirstChildElement("hotspot");
	i = 0;
	while (element) {
		printf("loading hotspot...\n");

		hotSpots[i] = new CharacterHotSpot(element);


		i++;
		element = element->NextSiblingElement("hotspot");
	}
	hotSpotsCount = i;

	TiXmlElement* animationElem = xml->FirstChildElement("animation");

	animationsCount = atoi(xml->Attribute("animations"));
	printf("%i animations\n", animationsCount);
	frameAnimAngle = new float**[animationsCount];
	frameAnimLayer = new int**[animationsCount];
	frameAnimShow = new bool**[animationsCount];
	frameAnimX = new float**[animationsCount];
	frameAnimY = new float**[animationsCount];
	framesCounts = new int[animationsCount];
	framesLengths = new float*[animationsCount];

	frameHotSpotAngle = new float**[animationsCount];
	frameHotSpotShow = new bool**[animationsCount];
	frameHotSpotX = new float**[animationsCount];
	frameHotSpotY = new float**[animationsCount];

	actionsCounts = new int[animationsCount];
	actions = new CharacterAction**[animationsCount];
	i = 0;
	while (animationElem) {
		printf("animation\n");
		framesCounts[i] = atoi(animationElem->Attribute("frames"));

		frameAnimAngle[i] = new float*[framesCounts[i]];
		frameAnimLayer[i] = new int*[framesCounts[i]];
		frameAnimShow[i] = new bool*[framesCounts[i]];
		frameAnimX[i] = new float*[framesCounts[i]];
		frameAnimY[i] = new float*[framesCounts[i]];
		framesLengths[i] = new float[framesCounts[i]];

		frameHotSpotAngle[i] = new float*[framesCounts[i]];
		frameHotSpotShow[i] = new bool*[framesCounts[i]];
		frameHotSpotX[i] = new float*[framesCounts[i]];
		frameHotSpotY[i] = new float*[framesCounts[i]];

		TiXmlElement* frameElem = animationElem->FirstChildElement("frame");
		int j = 0;
		while (frameElem) {
			frameAnimAngle[i][j] = new float[bodiesCount];
			frameAnimLayer[i][j] = new int[bodiesCount];
			frameAnimShow[i][j] = new bool[bodiesCount];
			frameAnimX[i][j] = new float[bodiesCount];
			frameAnimY[i][j] = new float[bodiesCount];

			frameHotSpotAngle[i][j] = new float[hotSpotsCount];
			frameHotSpotShow[i][j] = new bool[hotSpotsCount];
			frameHotSpotX[i][j] = new float[hotSpotsCount];
			frameHotSpotY[i][j] = new float[hotSpotsCount];

			framesLengths[i][j] = atof(frameElem->Attribute("length"));

			TiXmlElement* bodyElem = frameElem->FirstChildElement("body");
			int k = 0;
			while (bodyElem) {
				frameAnimX[i][j][k] = atof(bodyElem->Attribute("x"));
				frameAnimY[i][j][k] = atof(bodyElem->Attribute("y"));
				frameAnimAngle[i][j][k] = atof(bodyElem->Attribute("angle"));
				frameAnimShow[i][j][k] = atoi(bodyElem->Attribute("show"));
				frameAnimLayer[i][j][k] = atoi(bodyElem->Attribute("layer"));
				k++;
				bodyElem = bodyElem->NextSiblingElement("body");
			}

			bodyElem = frameElem->FirstChildElement("hotspot");
			k = 0;
			while (bodyElem) {
				frameHotSpotX[i][j][k] = atof(bodyElem->Attribute("x"));
				frameHotSpotY[i][j][k] = atof(bodyElem->Attribute("y"));
				frameHotSpotAngle[i][j][k] = atof(bodyElem->Attribute("angle"));
				frameHotSpotShow[i][j][k] = atoi(bodyElem->Attribute("show"));
				k++;
				bodyElem = bodyElem->NextSiblingElement("hotspot");
			}

			j++;
			frameElem = frameElem->NextSiblingElement("frame");
		}

		TiXmlElement* actionsRoot = animationElem->FirstChildElement("actions");
		if (actionsRoot) {
			actionsCounts[i] = atoi(actionsRoot->Attribute("count"));
			actions[i] = new CharacterAction*[actionsCounts[i]];
			TiXmlElement* actionElem = actionsRoot->FirstChildElement("action");
			j = 0;
			while (actionElem) {
				actions[i][j] = new CharacterAction(actionElem);
				j++;
				actionElem = actionElem->NextSiblingElement("action");
				printf("action loaded\n");
			}
		} else {
			actionsCounts[i] = 0;
			actions[i] = 0;
		}
		i++;
		animationElem = animationElem->NextSiblingElement("animation");
	}
	animationsCount = i;

	position = origin;
}

void Character::draw(bool schematicMode)
{

	if (schematicMode) {
		game->drawRect(
			game->screenX(position.x),
			game->screenY(position.y),
			halfWidth * game->getFullScale(),
			halfHeight * game->getFullScale(),
			0, 0xFFFFFFFF, 0
		);
	} else {

	}

	int offset = turnedRight ? -1 : 1;
	for (int index = 0; index < bodiesCount; index++) {
		int i = frameAnimLayer[currentAnimation][currentFrame][index];
		if (frameAnimShow[currentAnimation][currentFrame][i]) {
			animations[i]->RenderEx(
				game->screenX(position.x + offset * animatedValue(frameAnimX[currentAnimation][currentFrame][i], frameAnimX[currentAnimation][nextFrame][i])),
				game->screenY(position.y + animatedValue(frameAnimY[currentAnimation][currentFrame][i], frameAnimY[currentAnimation][nextFrame][i])),
				offset * animatedValue(frameAnimAngle[currentAnimation][currentFrame][i], frameAnimAngle[currentAnimation][nextFrame][i]),
				game->getScaleFactor(),
				game->getScaleFactor()
			);
		}
	}
}

void Character::update(float dt)
{

	currentTime += dt;
	float time = 0;
	int frame = 0;
	for (int i = 0; i < framesCounts[currentAnimation]; i++) {
		if (currentTime > time) {
			frame = i;
			frameProgress = (currentTime - time) / framesLengths[currentAnimation][i];
		}
		time += framesLengths[currentAnimation][i];
	}
	currentFrame = frame;
	if (currentFrame > framesCounts[currentAnimation] - 2)
		nextFrame = 0;
	else
		nextFrame = currentFrame + 1;
	if (currentTime > time)
		currentTime = 0;


	int offset = turnedRight ? -1 : 1;
	for (int i = 0; i < hotSpotsCount; i++) {
		if (frameHotSpotShow[currentAnimation][currentFrame][i]) {

		} else {

		}
	}

	for (int i = 0; i < actionsCounts[currentAnimation]; i++) {
		if (actions[currentAnimation][i]->take(game, this)) {
		}
	}

	//onGround = false;

	if (onGround > -1) {
		if (
			position.x + halfWidth > game->getGroundLine(onGround)->getEndPoint().x ||
			position.x - halfWidth < game->getGroundLine(onGround)->getStartPoint().x
		) {
			onGround = -1;
		}
	} else {
		speed += (dt * game->getGravity());
		prevPosition = position;
		position += (dt * speed);

		for (int i = 0; i < game->getGroundLinesCount(); i++) {
			if (!(game->getGroundLine(i)->characterIntersects(this) == b2Vec2_zero)) {
				position.y = game->getGroundLine(i)->yAt(position.x) - halfHeight;
				speed = b2Vec2_zero;
				onGround = i;
			}
		}
	}

	control(dt);
}

void Character::turn()
{
	turnedRight = !turnedRight;
	for (int i = 0; i < bodiesCount; i++) {
		animations[i]->SetFlip(turnedRight, false, true);
	}
}

void Character::run(float speed)
{
	if (onGround > -1) {
		speed = speed * (turnedRight ? 1 : -1);
		float speedX = game->getGroundLine(onGround)->getCosA() * speed;
		float speedY = game->getGroundLine(onGround)->getSinA() * speed;
		position.x += speedX * game->getTimeStep();
		position.y += speedY * game->getTimeStep();
		this->speed.x = speedX;
		this->speed.y = speedY;
	} else {
		speed = speed * (turnedRight ? 1 : -1);
		this->speed.x = speed;
	}
}

void Character::jump(b2Vec2 speed)
{
	onGround = -1;
	speed.x *=  (turnedRight ? 1 : -1);
	this->speed = speed;
	//speed.x = 3 * speed.x * (turnedRight ? 1 : -1);
	//speed.y = 6 * speed.y;
	//position.y -= 2;

}

void Character::setAnim(int anim)
{
	printf("anim %i\n", anim);
	currentAnimation = anim;
	currentFrame = 0;
	nextFrame = 0;
	currentTime = 0;
}

float Character::animatedValue(float prev, float next)
{
	return prev + (next - prev) * frameProgress;
}

b2Vec2 Character::getPosition()
{
	return position;
}
void Character::setPosition(b2Vec2 newPos)
{
	position = newPos;
}

float Character::getX()
{
	return position.x;
}
float Character::getY()
{
	return position.y;
}
float Character::getHalfHeight()
{
	return halfHeight;
}
float Character::getHalfWidth()
{
	return halfWidth;
}

int Character::getOnGround()
{
	return onGround;
}
