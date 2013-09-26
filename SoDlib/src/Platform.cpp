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
}

Platform::~Platform()
{
    //dtor
}
