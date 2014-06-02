#include "MapAnimation.h"

MapAnimation::MapAnimation(Game* _game) {
    game = _game;
    animation = NULL;
    x = 0.0f;
    y = 0.0f;
    angle = 0.0f;
    scaleX = 1.0f;
    scaleY = 1.0f;
}

MapAnimation::~MapAnimation()
{
    //dtor
}

void MapAnimation::loadFromXml(TiXmlElement* xml) {
    x = atof(xml->Attribute("x"));
    y = atof(xml->Attribute("y"));
    angle = atof(xml->Attribute("angle"));

    if ( xml->Attribute("scale_x") ) {
        scaleX = atof( xml->Attribute("scale_x") );
    }
    if ( xml->Attribute("scale_y") ) {
        scaleY = atof( xml->Attribute("scale_y") );
    }

    animation = game->loadAnimation( (char*)xml->Attribute("file") );
}

void MapAnimation::draw(float ratio, bool schematicMode)
{
    animation->RenderEx(
        game->screenX(x, ratio),
        game->screenY(y, ratio),
        angle,
        game->getScaleFactor() * scaleX,
        game->getScaleFactor() * scaleY
    );
}

void MapAnimation::move(float dx, float dy)
{
    x += dx; y += dy;
}
