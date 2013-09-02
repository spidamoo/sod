#include "Construction.h"


Construction::Construction()
{
	//ctor
}
Construction::~Construction()
{
	//dtor
}

Construction::Construction(char * fileName, Game * game, b2Vec2 origin, b2BodyType type)
{
	printf("loading construction %s ... \n", fileName);
    TiXmlDocument doc(fileName);
    bool loadOkay = doc.LoadFile();
    if (loadOkay)
    {
        TiXmlElement* element = doc.FirstChildElement("body");
        int i = 0;
        while (element)
        {
            i++;
            element = element->NextSiblingElement("body");
        }
        printf("found %i bodies\n", i);
        bodies = new b2Body*[i];
        animations = new hgeAnimation*[i];
        bodiesCount = i;
        element = doc.FirstChildElement("body");
        i = 0;
        while (element) {
        	printf("loading body...\n");
        	float x = atof(element->Attribute("x"));
			float y = atof(element->Attribute("y"));
			float angle = atof(element->Attribute("angle"));

			printf("set it at %.2f %.2f\n", origin.x, origin.y);

			hgeAnimation* animation = game->loadAnimation((char*)element->Attribute("animation"));
			//animation->SetHotSpot(-game->getPixelsPerMeter() * atof(element->Attribute("x")), -game->getPixelsPerMeter() * atof(element->Attribute("y")));

			//bodies[i]     = body;
			animations[i] = animation;

			i++;
            element = element->NextSiblingElement("body");
        }

    } else {
        char * error = strdup("Can't load construction: ");
        strcat(error,fileName);
        Exception * e = new Exception(error);
        throw e;
    }
    printf("construction loaded\n");
}

void Construction::draw(Game* game, bool schematicMode)
{
	for (int i = 0; i < bodiesCount; i++) {
		if (schematicMode) {

		} else {

		}
		animations[i]->RenderEx(game->screenX(0), game->screenY(0), bodies[i]->GetAngle(), game->getScaleFactor(), game->getScaleFactor());
	}
}


