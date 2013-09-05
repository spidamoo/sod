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

	angleAnimAngle = new float**[animationsCount];
	angleAnimShow = new bool**[animationsCount];
	angleAnimX = new float**[animationsCount];
	angleAnimY = new float**[animationsCount];
	angleCounts = new int[animationsCount];
	angles = new float*[animationsCount];

	angleHotSpotAngle = new float**[animationsCount];
	angleHotSpotShow = new bool**[animationsCount];
	angleHotSpotX = new float**[animationsCount];
	angleHotSpotY = new float**[animationsCount];

	actionsCounts = new int[animationsCount];
	actions = new CharacterAction**[animationsCount];

	animRotating = new bool*[animationsCount];
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

		angleCounts[i] = atoi(animationElem->Attribute("angles"));

		angleAnimAngle[i] = new float*[angleCounts[i]];
		angleAnimShow[i] = new bool*[angleCounts[i]];
		angleAnimX[i] = new float*[angleCounts[i]];
		angleAnimY[i] = new float*[angleCounts[i]];
		angles[i] = new float[angleCounts[i]];

		angleHotSpotAngle[i] = new float*[angleCounts[i]];
		angleHotSpotShow[i] = new bool*[angleCounts[i]];
		angleHotSpotX[i] = new float*[angleCounts[i]];
		angleHotSpotY[i] = new float*[angleCounts[i]];

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

		frameElem = animationElem->FirstChildElement("angle");
		j = 0;
		while (frameElem) {
			angleAnimAngle[i][j] = new float[bodiesCount];
			angleAnimShow[i][j] = new bool[bodiesCount];
			angleAnimX[i][j] = new float[bodiesCount];
			angleAnimY[i][j] = new float[bodiesCount];

			angleHotSpotAngle[i][j] = new float[hotSpotsCount];
			angleHotSpotShow[i][j] = new bool[hotSpotsCount];
			angleHotSpotX[i][j] = new float[hotSpotsCount];
			angleHotSpotY[i][j] = new float[hotSpotsCount];

			angles[i][j] = atof(frameElem->Attribute("value"));
			printf("angle %i is %f\n", j, angles[i][j]);

			TiXmlElement* bodyElem = frameElem->FirstChildElement("body");
			int k = 0;
			while (bodyElem) {
				angleAnimX[i][j][k] = atof(bodyElem->Attribute("x"));
				angleAnimY[i][j][k] = atof(bodyElem->Attribute("y"));
				angleAnimAngle[i][j][k] = atof(bodyElem->Attribute("angle"));
				angleAnimShow[i][j][k] = atoi(bodyElem->Attribute("show"));
				k++;
				bodyElem = bodyElem->NextSiblingElement("body");
			}

			bodyElem = frameElem->FirstChildElement("hotspot");
			k = 0;
			while (bodyElem) {
				angleHotSpotX[i][j][k] = atof(bodyElem->Attribute("x"));
				angleHotSpotY[i][j][k] = atof(bodyElem->Attribute("y"));
				angleHotSpotAngle[i][j][k] = atof(bodyElem->Attribute("angle"));
				angleHotSpotShow[i][j][k] = atoi(bodyElem->Attribute("show"));
				k++;
				bodyElem = bodyElem->NextSiblingElement("hotspot");
			}

			j++;
			frameElem = frameElem->NextSiblingElement("angle");
		}

		animRotating[i] = new bool[bodiesCount];
		TiXmlElement* bodyElem = animationElem->FirstChildElement("body");
		j = 0;
		while (bodyElem) {
			animRotating[i][j] = atoi(bodyElem->Attribute("rotating"));
			j++;
			bodyElem = bodyElem->NextSiblingElement("body");
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
		if (!animRotating[currentAnimation][i]) {
			if (frameAnimShow[currentAnimation][currentFrame][i]) {
				animations[i]->SetFlip(turnedRight, false, true);
				animations[i]->RenderEx(
					game->screenX(position.x + offset * animatedValue(frameAnimX[currentAnimation][currentFrame][i], frameAnimX[currentAnimation][nextFrame][i])),
					game->screenY(position.y + animatedValue(frameAnimY[currentAnimation][currentFrame][i], frameAnimY[currentAnimation][nextFrame][i])),
					offset * animatedValue(frameAnimAngle[currentAnimation][currentFrame][i], frameAnimAngle[currentAnimation][nextFrame][i]),
					game->getScaleFactor(),
					game->getScaleFactor()
				);
			}
		} else {
			if (angleAnimShow[currentAnimation][currentFrame][i]) {
				animations[i]->RenderEx(
					game->screenX(position.x + midanglePosition(angleAnimX[currentAnimation][prevAngle][i], angleAnimX[currentAnimation][nextAngle][i])),
					game->screenY(position.y + midanglePosition(angleAnimY[currentAnimation][prevAngle][i], angleAnimY[currentAnimation][nextAngle][i])),
					midangleAngle(angleAnimAngle[currentAnimation][prevAngle][i], angleAnimAngle[currentAnimation][nextAngle][i]),
					game->getScaleFactor(),
					game->getScaleFactor()
				);
			}
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

	angle = atan2(game->getWorldMouseY() - position.y, game->getWorldMouseX() - position.x);
	prevAngle = 0; nextAngle = 0;
	float prevDiff = - M_PI * 2;
	float nextDiff = M_PI * 2;
	for (int i = 0; i < angleCounts[currentAnimation]; i++) {
		float nAngle = angles[currentAnimation][i];
		float pAngle = angles[currentAnimation][i];
		while (nAngle < angle) {
			nAngle += M_PI * 2;
		}
		while (pAngle > angle) {
			pAngle -= M_PI * 2;
		}
		if (nAngle - angle < nextDiff) {
			nextDiff = nAngle - angle;
			nextAngle = i;
		}
		if (pAngle - angle > prevDiff) {
			prevDiff = pAngle - angle;
			prevAngle = i;
		}
	}

	angleProgress = (prevDiff / (prevDiff - nextDiff));

	int offset = turnedRight ? -1 : 1;
	for (int i = 0; i < hotSpotsCount; i++) {
		if (frameHotSpotShow[currentAnimation][currentFrame][i]) {

		} else {

		}
	}

	prevPosition = position;

	if (onGround != -1) {
		speed = b2Vec2_zero;
	} else {
		speed += (dt * game->getGravity());
		position += (dt * speed);
	}

	float h = position.y;
	for (int i = 0; i < game->getGroundLinesCount(); i++) {
		if (game->getGroundLine(i)->getType() == GROUND_LINE_TYPE_FLOOR) {
			if (
                position.x > game->getGroundLine(i)->getStartPoint().x && position.x < game->getGroundLine(i)->getEndPoint().x
            ) {
                if (prevPosition.y < game->getGroundLine(i)->yAt(position.x) && position.y + halfHeight > game->getGroundLine(i)->yAt(position.x)) {
                    onGround = i;
                    position.y = game->getGroundLine(i)->yAt(position.x) - halfHeight;
                } else if (prevPosition.y > game->getGroundLine(i)->yAt(position.x) && position.y - halfHeight < game->getGroundLine(i)->yAt(position.x)) {
                    position.y = game->getGroundLine(i)->yAt(position.x) + halfHeight;
                    speed.y = 0;
                }
			}
		} else {
		    if (game->getGroundLine(i)->getK() > 0) {
                if (
                    position.y - halfHeight > game->getGroundLine(i)->getStartPoint().y
                    && position.y + halfHeight < game->getGroundLine(i)->getEndPoint().y
                ) {
                    if (prevPosition.x < game->getGroundLine(i)->xAt(position.y) && position.x + halfWidth > game->getGroundLine(i)->xAt(position.y)) {
                        position.x = game->getGroundLine(i)->xAt(position.y) - halfWidth;
                        speed.x = 0;
                    } else if (prevPosition.x > game->getGroundLine(i)->xAt(position.y) && position.x - halfWidth < game->getGroundLine(i)->xAt(position.y)) {
                        position.x = game->getGroundLine(i)->xAt(position.y) + halfWidth;
                        speed.x = 0;
                    }
                } else (
                    position.y - halfHeight > game->getGroundLine(i)->getEndPoint().y
                    && position.y + halfHeight < game->getGroundLine(i)->getStartPoint().y
                ) {
                    if (prevPosition.x < game->getGroundLine(i)->xAt(position.y) && position.x + halfWidth > game->getGroundLine(i)->xAt(position.y)) {
                        position.x = game->getGroundLine(i)->xAt(position.y) - halfWidth;
                        speed.x = 0;
                    } else if (prevPosition.x > game->getGroundLine(i)->xAt(position.y) && position.x - halfWidth < game->getGroundLine(i)->xAt(position.y)) {
                        position.x = game->getGroundLine(i)->xAt(position.y) + halfWidth;
                        speed.x = 0;
                    }
                }
			}
		}
	}

	if (position.x > game->getMapWidth()) {
        position.x = game->getMapWidth();
        speed.x = 0;
	}
	if (position.x < 0) {
        position.x = 0;
        speed.x = 0;
	}
	if (position.y > game->getMapHeight()) {
        position.y = game->getMapHeight();
        speed.y = 0;
	}
	if (position.y < 0) {
        position.y = 0;
        speed.y = 0;
	}


	for (int i = 0; i < actionsCounts[currentAnimation]; i++) {
		if (actions[currentAnimation][i]->take(game, this)) {
		}
	}

    if (onGround > -1) {
		if (
			position.x > game->getGroundLine(onGround)->getEndPoint().x ||
			position.x < game->getGroundLine(onGround)->getStartPoint().x
		) {
			onGround = -1;
		}
	}

	control(dt);
}

void Character::turn()
{
	turnedRight = !turnedRight;
//	for (int i = 0; i < bodiesCount; i++) {
////		animations[i]->SetFlip(turnedRight, false, true);
//	}
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
	currentAnimation = anim;
	currentFrame = 0;
	nextFrame = 0;
	currentTime = 0;
}

float Character::animatedValue(float prev, float next)
{
	return prev + (next - prev) * frameProgress;
}
float Character::midanglePosition(float prev, float next)
{
	return prev + (next - prev) * angleProgress;
}
float Character::midangleAngle(float prev, float next)
{
	if (next < prev) {
		next += 2 * M_PI;
	}
	return prev + (next - prev) * angleProgress;
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
