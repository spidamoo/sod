#include "Platform.h"

Platform::Platform(Game* game, TiXmlElement* element)
{
    this->game = game;

    groundLinesCount = atoi(element->Attribute("ground_lines"));
    animationsCount  = atoi(element->Attribute("animations"));
    groundLines = new int[groundLinesCount];
    animations  = new int[animationsCount];

    spotsCount = atoi(element->Attribute("spots"));
    spotX     = new float[spotsCount];
    spotY     = new float[spotsCount];
    spotAngle = new float[spotsCount];
    spotTime  = new float[spotsCount];

    int j = 0;
    TiXmlElement* child = element->FirstChildElement("ground_line");
    while (child) {
        groundLines[j] = atoi(child->Attribute("index"));
        j++;
        child = child->NextSiblingElement("ground_line");
    }

    j = 0;
    child = element->FirstChildElement("animation");
    while (child) {
        animations[j] = atoi(child->Attribute("index"));
        j++;
        child = child->NextSiblingElement("animation");
    }

    j = 0;
    child = element->FirstChildElement("spot");
    while (child) {
        spotTime[j] = atof(child->Attribute("time"));
        spotX[j] = atof(child->Attribute("x"));
        spotY[j] = atof(child->Attribute("y"));
        spotAngle[j] = atof(child->Attribute("angle"));
        j++;
        child = child->NextSiblingElement("spot");
    }

    currentSpot = 0;
    currentTime = 0.0f;
    currentSpotStart = 0.0f;
    currentX = 0;
    currentY = 0;
}

Platform::~Platform()
{
    //dtor
}

void Platform::update(float dt)
{
    currentTime += dt;
    if (currentTime > currentSpotStart + spotTime[currentSpot]) {
        ///Переходим к следующему положению
        if (currentSpot == spotsCount - 1) {
            currentTime -= (currentSpotStart + spotTime[currentSpot]);
            currentSpotStart = 0;
            currentSpot = 0;
        } else {
            currentSpotStart += spotTime[currentSpot];
            currentSpot++;
        }
    }
    float prevX = 0; float prevY = 0;
    if (currentSpot > 0) {
        prevX = spotX[currentSpot - 1];
        prevY = spotY[currentSpot - 1];
    }
    float progress = (currentTime - currentSpotStart) / spotTime[currentSpot];
    float x = prevX + (spotX[currentSpot] - prevX) * progress;
    float y = prevY + (spotY[currentSpot] - prevY) * progress;
    float dx = x - currentX; float dy = y - currentY;

    for (int i = 0; i < game->getCharactersCount(); i++) {
        if (characterOn(i)) {
            game->getCharacter(i)->move(dx, dy);
        }
    }
    for (int i = 0; i < groundLinesCount; i++) {
        game->getGroundLine(groundLines[i])->move(dx, dy);
    }
    for (int i = 0; i < animationsCount; i++) {
        game->getMapAnimation(animations[i])->move(dx, dy);
    }
    currentX = x; currentY = y;
}

bool Platform::characterOn(int charIndex)
{
    Character* character = game->getCharacter(charIndex);
    for (int i = 0; i < groundLinesCount; i++) {
        if (character->getOnGround() == groundLines[i])
            return true;
    }
    return false;
}
