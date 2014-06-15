#include "Character.h"

Character::Character(TiXmlElement* xml, Game * game, b2Vec2 origin) {
	this->game = game;

	height = 2.0f;
	halfHeight = 1.0f;
	width = 0.5f;
	halfWidth = 0.25f;

	currentMove = 0;
    currentFrame = 0;
    currentTime = 0;
    turnedRight = false;
    onGround = -1;

    speed = b2Vec2_zero;
    prevPosition = position;
    angle = 0.25f * M_PI;

    currentStatus = -1;

	conditions = new Condition*[100];
    conditionsCount = 0;

    params = new CharacterParam*[game->getCharacterParamPrototypesCount()];
	for (int i = 0; i < game->getCharacterParamPrototypesCount(); i++) {
        params[i] = new CharacterParam( this, game->getCharacterParamPrototype(i) );
	}

	resources = new CharacterResource*[game->getCharacterResourcePrototypesCount()];
	for (int i = 0; i < game->getCharacterResourcePrototypesCount(); i++) {
        resources[i] = new CharacterResource( game, this, game->getCharacterResourcePrototype(i) );
	}

	statusActions = new CharacterAction*[game->getCharacterStatusPrototypesCount()];
	for (int i = 0; i < game->getCharacterStatusPrototypesCount(); i++) {
        statusActions[i] = NULL;
	}

	loadFromXml(xml, origin);

	updateStatus();

    printf("character loaded\n");

    setMove(0);
}

Character::~Character() {
    delete animations;
//    delete actions;
}

bool Character::loadFromXml(TiXmlElement* xml, b2Vec2 origin) {
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

	TiXmlElement* animationElem = xml->FirstChildElement("move");

	movesCount = atoi(xml->Attribute("moves"));
	printf("%i animations\n", movesCount);
	frameAnimAngle = new float**[movesCount];
	frameAnimScaleX = new float**[movesCount];
	frameAnimScaleY = new float**[movesCount];
	frameAnimLayer = new int**[movesCount];
	frameAnimShow = new bool**[movesCount];
	frameAnimX = new float**[movesCount];
	frameAnimY = new float**[movesCount];
	framesCounts = new int[movesCount];
	framesLengths = new float*[movesCount];

	frameHotSpotAngle = new float**[movesCount];
	frameHotSpotShow = new bool**[movesCount];
	frameHotSpotX = new float**[movesCount];
	frameHotSpotY = new float**[movesCount];

	angleAnimAngle = new float**[movesCount];
	angleAnimShow = new bool**[movesCount];
	angleAnimX = new float**[movesCount];
	angleAnimY = new float**[movesCount];
	angleCounts = new int[movesCount];
	angles = new float*[movesCount];

	angleHotSpotAngle = new float**[movesCount];
	angleHotSpotShow = new bool**[movesCount];
	angleHotSpotX = new float**[movesCount];
	angleHotSpotY = new float**[movesCount];

	actionsCounts = new int[movesCount];
	actions = new CharacterAction**[movesCount];

	animRotating = new bool*[movesCount];

	movePriorities = new float[movesCount];
	i = 0;
	while (animationElem) {
		printf("move\n");
		framesCounts[i] = atoi(animationElem->Attribute("frames"));

		frameAnimAngle[i] = new float*[framesCounts[i]];
		frameAnimScaleX[i] = new float*[framesCounts[i]];
		frameAnimScaleY[i] = new float*[framesCounts[i]];
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

		if (animationElem->Attribute("priority")) {
            movePriorities[i] = atof( animationElem->Attribute("priority") );
		}
		else {
            movePriorities[i] = 0.0f;
		}


		TiXmlElement* frameElem = animationElem->FirstChildElement("frame");
		int j = 0;
		while (frameElem) {
            printf("frame %d\n", j);

			frameAnimAngle[i][j] = new float[bodiesCount];
			frameAnimScaleX[i][j] = new float[bodiesCount];
			frameAnimScaleY[i][j] = new float[bodiesCount];
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
				frameAnimScaleX[i][j][k] = atof(bodyElem->Attribute("scale_x"));
				frameAnimScaleY[i][j][k] = atof(bodyElem->Attribute("scale_y"));
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
		animationElem = animationElem->NextSiblingElement("move");
	}
	movesCount = i;

	element = xml->FirstChildElement("resource");
    i = 0;
    while (element) {
        resources[i]->loadBarParamsFromXml(element);

        i++;
        element = element->NextSiblingElement("resource");
    }

	position = origin;
}

void Character::draw(bool schematicMode) {

	if (schematicMode) {
		game->drawRect(
			game->screenX(position.x),
			game->screenY(position.y),
			halfWidth * game->getFullScale(),
			halfHeight * game->getFullScale(),
			0, 0xFFFFFFFF, 0
		);
		for (int i = 0; i < hotSpotsCount; i++) {
            game->drawRect(
                game->screenX(getHotSpotX(i)),
                game->screenY(getHotSpotY(i)),
                2,
                2,
                0, 0xFF00FF00, 0xAA00FF00
            );
		}
		char buffer[16];
		sprintf(buffer, "conditions: %d", conditionsCount);
		game->drawText(game->screenX(position.x), game->screenY(position.y) - 100.0f, buffer);

	} else {

	}

	for (int i = 0; i < game->getCharacterResourcePrototypesCount(); i++) {
        resources[i]->draw(schematicMode, i);
    }

	int offset = turnedRight ? -1 : 1;
	for (int index = 0; index < bodiesCount; index++) {
		int i = frameAnimLayer[currentMove][currentFrame][index];
		if (!animRotating[currentMove][i]) {
			if (frameAnimShow[currentMove][currentFrame][i]) {
                while (frameAnimAngle[currentMove][currentFrame][i] > M_PI) {
                    frameAnimAngle[currentMove][currentFrame][i] -= 2.0f * M_PI;
                }
                while (frameAnimAngle[currentMove][currentFrame][i] < -M_PI) {
                    frameAnimAngle[currentMove][currentFrame][i] += 2.0f * M_PI;
                }
                while (frameAnimAngle[currentMove][nextFrame][i] - frameAnimAngle[currentMove][currentFrame][i] > M_PI) {
                    frameAnimAngle[currentMove][nextFrame][i] -= 2.0f * M_PI;
                }
                while (frameAnimAngle[currentMove][nextFrame][i] - frameAnimAngle[currentMove][currentFrame][i] < -M_PI) {
                    frameAnimAngle[currentMove][nextFrame][i] += 2.0f * M_PI;
                }
				animations[i]->SetFlip(turnedRight, false, true);
				animations[i]->RenderEx(
					game->screenX(position.x + offset * animatedValue(frameAnimX[currentMove][currentFrame][i], frameAnimX[currentMove][nextFrame][i])),
					game->screenY(position.y + animatedValue(frameAnimY[currentMove][currentFrame][i], frameAnimY[currentMove][nextFrame][i])),
					offset * animatedValue(frameAnimAngle[currentMove][currentFrame][i], frameAnimAngle[currentMove][nextFrame][i]),
					game->getScaleFactor() * animatedValue(frameAnimScaleX[currentMove][currentFrame][i], frameAnimScaleX[currentMove][nextFrame][i]),
					game->getScaleFactor() * animatedValue(frameAnimScaleY[currentMove][currentFrame][i], frameAnimScaleY[currentMove][nextFrame][i])
				);
			}
		} else {
			if (angleAnimShow[currentMove][currentFrame][i]) {
				animations[i]->RenderEx(
					game->screenX(position.x + midanglePosition(angleAnimX[currentMove][prevAngle][i], angleAnimX[currentMove][nextAngle][i])),
					game->screenY(position.y + midanglePosition(angleAnimY[currentMove][prevAngle][i], angleAnimY[currentMove][nextAngle][i])),
					midangleAngle(angleAnimAngle[currentMove][prevAngle][i], angleAnimAngle[currentMove][nextAngle][i]),
					game->getScaleFactor(),
					game->getScaleFactor()
				);
			}
		}
	}

    /*
	game->drawRect(
        game->screenX(position.x) - 25,
        game->screenY(position.y - halfHeight) - 10,
        game->screenX(position.x) + 25,
        game->screenY(position.y - halfHeight) - 5,
        0xFFFFFFFF, 0xFF000000
    );
    if ( currentHitPoints > 0 ) {
        game->drawRect(
            game->screenX(position.x) - 24,
            game->screenY(position.y - halfHeight) - 9,
            game->screenX(position.x) - 24 + 48 * currentHitPoints / maxHitPoints,
            game->screenY(position.y - halfHeight) - 6,
            0xFFFF0000, 0xFFFF0000
        );
    }
    */
}

void Character::update(float dt) {
	currentTime += dt;
	float time = 0;
	int frame = 0;
	for (int i = 0; i < framesCounts[currentMove]; i++) {
		if (currentTime > time) {
			frame = i;
			frameProgress = (currentTime - time) / framesLengths[currentMove][i];
		}
		time += framesLengths[currentMove][i];
	}
	currentFrame = frame;
	if (currentFrame > framesCounts[currentMove] - 2)
		nextFrame = 0;
	else
		nextFrame = currentFrame + 1;

    if (turnedRight) {
        angle = atan2(game->getWorldMouseY() - position.y, ( game->getWorldMouseX() - position.x ) );
    }
    else {
        angle = atan2(game->getWorldMouseY() - position.y, ( position.x - game->getWorldMouseX() ) );
    }
	prevAngle = 0; nextAngle = 0;
	float prevDiff = - M_PI * 2;
	float nextDiff = M_PI * 2;
	for (int i = 0; i < angleCounts[currentMove]; i++) {
		float nAngle = angles[currentMove][i];
		float pAngle = angles[currentMove][i];
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
		if (frameHotSpotShow[currentMove][currentFrame][i]) {

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

    for (int i = 0; i < actionsCounts[currentMove]; i++) {
		if (actions[currentMove][i]->take(game, this)) {
		}
	}

	if (currentTime > time)
		currentTime = 0;

	float h = position.y;
	for (int i = 0; i < game->getGroundLinesCount(); i++) {///Обработаем все стены
		if (game->getGroundLine(i)->getType() == GROUND_LINE_TYPE_WALL) {

                if (
                    position.y - halfHeight + 0.1f < game->getGroundLine(i)->getBottom()///Верхняя точка героя выше нижней точки линии
                    && position.y + halfHeight - 0.1f > game->getGroundLine(i)->getTop()///Или нижняя героя ниже верхней линии
                ) {
                    ///Если середина была слева от линии, а правый край теперь справа
                    if (prevPosition.x < game->getGroundLine(i)->xAt(position.y) && position.x + halfWidth > game->getGroundLine(i)->xAt(position.y)) {
                        ///Ставим к стенке (слева)
                        if (onGround == -1) {
                            position.x = game->getGroundLine(i)->xAt(position.y) - halfWidth;
                        } else {
                            position = prevPosition;
                        }
                        speed.x = 0;///Останавливаем
                    } else if (prevPosition.x > game->getGroundLine(i)->xAt(position.y) && position.x - halfWidth < game->getGroundLine(i)->xAt(position.y)) {
                        ///То же самое направо от стенки
                        if (onGround == -1) {
                            position.x = game->getGroundLine(i)->xAt(position.y) + halfWidth;
                        } else {
                            position = prevPosition;
                        }
                        speed.x = 0;
                    }
                }

		}
	}
	for (int i = 0; i < game->getGroundLinesCount(); i++) {///Теперь все полы/потолки
		if (game->getGroundLine(i)->getType() == GROUND_LINE_TYPE_FLOOR) {
		    if (
                position.x + halfWidth > game->getGroundLine(i)->getStartPoint().x && position.x - halfWidth < game->getGroundLine(i)->getEndPoint().x
            ) {
                float highY, lowY;
                if (game->getGroundLine(i)->getK() > 0) {
                    highY = game->getGroundLine(i)->yAt(position.x - halfWidth);
                    lowY = game->getGroundLine(i)->yAt(position.x + halfWidth);
                }
                else {
                    highY = game->getGroundLine(i)->yAt(position.x + halfWidth);
                    lowY = game->getGroundLine(i)->yAt(position.x - halfWidth);
                }

                if (prevPosition.y + halfHeight < highY && position.y + halfHeight > highY && speed.y >= 0) {
                    onGround = i;
                    position.y = highY - halfHeight;
                }
                else if (prevPosition.y > lowY && position.y - halfHeight < lowY) {
//                    position.y = lowY + halfHeight;
//                    speed.y = 0;
                }
			}
		}
	}

	if (position.x > game->getHalfMapWidth()) {
        position.x = game->getHalfMapWidth();
        speed.x = 0;
	}
	if (position.x < -game->getHalfMapWidth()) {
        position.x = -game->getHalfMapWidth();
        speed.x = 0;
	}
	if (position.y > game->getHalfMapHeight()) {
        position.y = game->getHalfMapHeight();
        speed.y = 0;
	}
//	if (position.y < 0) {
//        position.y = 0;
//        speed.y = 0;
//	}

    if (onGround > -1) {
		if (
			position.x - halfWidth > game->getGroundLine(onGround)->getEndPoint().x ||
			position.x + halfWidth < game->getGroundLine(onGround)->getStartPoint().x
		) {
			onGround = -1;
		}
	}

    int conditionsToRemove[conditionsCount];
	int conditionsToRemoveCount = 0;
	for (int i = 0; i < conditionsCount; i++) {
        conditions[i]->update(dt);
        if ( conditions[i]->getTime() < 0 ) {
            conditionsToRemove[conditionsToRemoveCount] = i;
            conditionsToRemoveCount++;
        }
	}
	for (int i = conditionsToRemoveCount - 1; i >= 0 ; i--) {
	    removeCondition( conditionsToRemove[i] );
	}

	if (currentStatus > -1 && statusPriority > movePriorities[currentMove]) {
        statusActions[currentStatus]->perform(game, this);
	}

	for (int i =0; i < game->getCharacterResourcePrototypesCount(); i++) {
        resources[i]->regen(dt);
	}

}

void Character::updateParams(Condition* condition) {
    switch ( condition->getPrototype()->getType() ) {
        case CONDITION_TYPE_ADD_RESOURCE:
        case CONDITION_TYPE_ADD_FULL_RESOURCE:
        case CONDITION_TYPE_BOOST_RESOURCE:
        case CONDITION_TYPE_REGEN_RESOURCE:
            resources[condition->getPrototype()->getParam()]->update();
            break;
        case CONDITION_TYPE_ADD_PARAM:
            params[condition->getPrototype()->getParam()]->update();
            break;
        case CONDITION_TYPE_ADD_STATUS:
        case CONDITION_TYPE_REMOVE_STATUS:
            updateStatus();
            break;
    }
}
void Character::updateStatus() {

    bool removeStatuses[ game->getCharacterStatusPrototypesCount() ];
    for (int i = 0; i < game->getCharacterStatusPrototypesCount(); i++) {
        removeStatuses[i] = false;
    }
    for (int i = 0; i < conditionsCount; i++) {
        if (conditions[i]->getPrototype()->getType() == CONDITION_TYPE_REMOVE_STATUS) {
            removeStatuses[ conditions[i]->getPrototype()->getParam() ] = true;
        }
    }

    currentStatus = -1;
    statusPriority = -1.0f;
    for (int i = 0; i < conditionsCount; i++) {
        if (conditions[i]->getPrototype()->getType() == CONDITION_TYPE_ADD_STATUS) {
            if ( removeStatuses[ conditions[i]->getPrototype()->getParam() ] ) {
                conditions[i]->setTime(-1.0f);
            }
            else if ( currentStatus == -1 || conditions[i]->getValue() > statusPriority ) {
                currentStatus = conditions[i]->getPrototype()->getParam();
                statusPriority = conditions[i]->getValue();
            }

        }
    }
}

void Character::turn() {
	turnedRight = !turnedRight;
//	for (int i = 0; i < bodiesCount; i++) {
////		animations[i]->SetFlip(turnedRight, false, true);
//	}
}

void Character::run(float speed) {
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
		position.x += game->getTimeStep() * speed;
		//this->speed.x = speed;
	}
}

void Character::jump(b2Vec2 _speed) {
	onGround = -1;
	_speed.x *=  (turnedRight ? 1 : -1);
	speed.x = _speed.x;
	if (_speed.y != 0.0f)
        speed.y = _speed.y;
	//speed.x = 3 * speed.x * (turnedRight ? 1 : -1);
	//speed.y = 6 * speed.y;
	//position.y -= 2;

}

void Character::move(float dx, float dy) {
    position.x += dx; position.y += dy;
}

void Character::setMove(int move) {
	currentMove = move;
	currentFrame = 0;
	nextFrame = 0;
	currentTime = 0.0f;

//	printf("move set to #%d\n", move);

	for (int i = 0; i< actionsCounts[currentMove]; i++) {
        actions[currentMove][i]->prepare(this);
	}
}

float Character::animatedValue(float prev, float next) {
	return prev + (next - prev) * frameProgress;
}
float Character::midanglePosition(float prev, float next) {
	return prev + (next - prev) * angleProgress;
}
float Character::midangleAngle(float prev, float next) {
	if (next < prev) {
		next += 2 * M_PI;
	}
	return prev + (next - prev) * angleProgress;
}

b2Vec2 Character::getPosition() {
	return position;
}
void Character::setPosition(b2Vec2 newPos) {
	position = newPos;
}

float Character::getX() {
	return position.x;
}
float Character::getY() {
	return position.y;
}
float Character::getHalfHeight() {
	return halfHeight;
}
float Character::getHalfWidth() {
	return halfWidth;
}

int Character::getOnGround() {
	return onGround;
}
int Character::getDirection() {
    return turnedRight ? 1 : -1;
}
float Character::getAngle() {
    return angle;
}

float Character::getHotSpotX(int index) {
    if (index < 0 || index > hotSpotsCount) {
        return position.x;
    }
    int offset = turnedRight ? -1 : 1;
    return position.x + offset * animatedValue( frameHotSpotX[currentMove][currentFrame][index], frameHotSpotX[currentMove][nextFrame][index] );
}
float Character::getHotSpotY(int index) {
    if (index < 0 || index > hotSpotsCount) {
        return position.y;
    }
    return position.y + animatedValue( frameHotSpotY[currentMove][currentFrame][index], frameHotSpotY[currentMove][nextFrame][index] );
}
float Character::getHotSpotAngle(int index) {
    if (index < 0 || index > hotSpotsCount) {
        return position.y;
    }
    return turnedRight ? -animatedValue( frameHotSpotAngle[currentMove][currentFrame][index], frameHotSpotAngle[currentMove][nextFrame][index] ) : animatedValue( frameHotSpotAngle[currentMove][currentFrame][index], frameHotSpotAngle[currentMove][nextFrame][index] );
}
int Character::getHotSpotIndex(char* name) {
    for (int i = 0; i < hotSpotsCount; i++) {
        if ( compareStrings( name, hotSpots[i]->getName() ) ) {
            return i;
        }
    }
    return -1;
}

float Character::getAnimTime() {
    return currentTime;
}

int Character::getConditionsCount() {
    return conditionsCount;
}
Condition* Character::getCondition(int index) {
    return conditions[index];
}
void Character::addCondition(Condition* condition) {
    conditions[conditionsCount] = condition;
    conditionsCount++;

    updateParams(condition);
}
void Character::removeCondition(int index) {
    if (index == conditionsCount - 1) {
        Condition* deleted = conditions[index];
        conditions[index] = NULL;
        conditionsCount--;
        updateParams(deleted);
        delete deleted;
    } else {
        Condition* deleted = conditions[index];
        conditions[index] = conditions[conditionsCount - 1];
        conditions[conditionsCount - 1] = NULL;
        conditionsCount--;
        updateParams(deleted);
        delete deleted;
    }
}

CharacterParam* Character::getParam(int index) {
    return params[index];
}
CharacterResource* Character::getResource(int index) {
    return resources[index];
}

void Character::setStatusAction(int status, CharacterAction* action) {
    statusActions[status] = action;
}
void Character::setResourceLessAction(int resource, float value, CharacterAction* action) {
    printf("setting action %d %f\n", resource, value);
    resources[resource]->setLessEvent(value, action);
}
