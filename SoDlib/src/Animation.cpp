#include "Animation.h"

Animation::Animation()
{
	//ctor
}

Animation::~Animation()
{
	//dtor
}

void Animation::load(char* fn)
{


	printf("loading animation %s ... \n", fn);
    TiXmlDocument doc(fn);
    bool loadOkay = doc.LoadFile();
    if (loadOkay)
    {

		TiXmlElement* root = doc.FirstChildElement("frames");
		framesCount = atoi(root->Attribute("count"));
		bodiesCount = atoi(root->Attribute("bodies"));

		x = new float*[framesCount];
		y = new float*[framesCount];
		angle = new float*[framesCount];
		for (int i = 0; i < framesCount; i++) {
			x[i] = new float[bodiesCount];
			y[i] = new float[bodiesCount];
			angle[i] = new float[bodiesCount];
		}

        TiXmlElement* element = root->FirstChildElement("frame");
        int i = 0;
        while (element) {
			TiXmlElement* bodyElem = element->FirstChildElement("body");
			int j = 0;
			while (bodyElem) {
				x[i][j] = atof(bodyElem->Attribute("x"));
				y[i][j] = atof(bodyElem->Attribute("y"));
				angle[i][j] = atof(bodyElem->Attribute("angle"));

				j++;
				bodyElem = bodyElem->NextSiblingElement("body");
			}
			delete bodyElem;

			i++;
            element = element->NextSiblingElement("frame");
        }
		delete root;
		delete element;
        printf("loaded animation with %i frames ... \n", framesCount);
    }
}
