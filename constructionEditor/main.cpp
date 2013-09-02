#include <stdio.h>
#include <hge.h>
#include <hgefont.h>
#include <hgeanim.h>
#include <Box2D/Box2D.h>
#include <SoDlib.h>
#include <windows.h>
#include <list>

Game* game;

const int MODE_DEFAULT     = 0;
const int MODE_SELECT_ANIM = 1;
const int MODE_INSERT_ANIM = 2;
const int MODE_INSERT_BODY = 3;
const int MODE_INSERT_JOINT_B1 = 4;
const int MODE_INSERT_JOINT_B2 = 5;
const int MODE_INSERT_JOINT = 6;
const int MODE_BODY_DRAG = 7;
const int MODE_CAMERA_MOVE = 8;
const int MODE_BODY_ROTATE = 9;

const int MODE_ANIM_EDIT = 100;

int mode = MODE_DEFAULT;

GUIWindow* mainWindow;
GUIWindow* animsWindow;

hgeAnimation* beingInsertedAnim;
char* beingInsertedAnimName;
std::list<b2Vec2> beingInsertedVertices;

int selectedBody = -1;
int selectedBody2 = -1;

float dragOffsetX;
float dragOffsetY;
float dragOffsetAngle;

b2Body** bodies = new b2Body*[256];
hgeAnimation** animations = new hgeAnimation*[256];
b2Joint** joints = new b2Joint*[256];
int bodiesCount = 0;
int jointsCount = 0;
float* animationX = new float[256];
float* animationY = new float[256];
float* animationAngle = new float[256];
char** animationNames = new char*[256];

int framesCount = 1;
int currentFrame = 0;
float** frameAnimX = new float*[256];
float** frameAnimY = new float*[256];
float** frameAnimAngle = new float*[256];

void resetMode()
{
	mode = MODE_DEFAULT;
	selectedBody = -1;
	selectedBody2 = -1;
	mainWindow->Show();
	animsWindow->Hide();
}

int getPointedBody(float x, float y)
{
	b2Vec2 point(x, y);
	for (int i = 0; i < bodiesCount; i++) {
		b2Body* body = bodies[i];
		b2Transform transform(b2Vec2(animationX[i], animationY[i]), b2Rot(animationAngle[i]));
		for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
			b2Shape* shape = f->GetShape();
			bool hit = shape->TestPoint(transform, point);
			if (hit) {
				return i;
			}
		}
	}
	return -1;
}

bool FrameFunc()
{
	float x, y;
	game->getHge()->Input_GetMousePos(&x, &y);
	float worldX = game->worldX(x);
	float worldY = game->worldY(y);

	if (game->getHge()->Input_GetMouseWheel() > 0) {
		game->setScale(game->getScaleFactor() * 2);
	}
	if (game->getHge()->Input_GetMouseWheel() < 0) {
		game->setScale(game->getScaleFactor() * 0.5);
	}

	switch(mode) {
		case MODE_DEFAULT:
			if (game->getHge()->Input_KeyDown(HGEK_LBUTTON)) {
				selectedBody = getPointedBody(worldX, worldY);
				if (selectedBody != -1) {
					dragOffsetX = worldX - animationX[selectedBody];
					dragOffsetY = worldY - animationY[selectedBody];
					mainWindow->Hide();
					mode = MODE_BODY_DRAG;
				}
			}
			if (game->getHge()->Input_KeyDown(HGEK_RBUTTON)) {
				selectedBody = getPointedBody(worldX, worldY);
				if (selectedBody != -1) {
					mainWindow->Hide();
					dragOffsetX = worldX;
					dragOffsetY = worldY;
					mode = MODE_BODY_ROTATE;
				} else {
					dragOffsetX = x;
					dragOffsetY = y;
					mode = MODE_CAMERA_MOVE;
					mainWindow->Hide();
				}
			}
			break;
		case MODE_CAMERA_MOVE:
			game->moveScreen(b2Vec2(dragOffsetX - x, dragOffsetY - y));
			dragOffsetX = x;
			dragOffsetY = y;
			if (game->getHge()->Input_KeyUp(HGEK_RBUTTON)) {
				resetMode();
			}
			break;
		case MODE_BODY_DRAG:
			if (selectedBody != -1) {
				float currentX = worldX - dragOffsetX;
				float currentY = worldY - dragOffsetY;
				animationX[selectedBody] = currentX;
				animationY[selectedBody] = currentY;
				frameAnimX[currentFrame][selectedBody] = currentX;
				frameAnimY[currentFrame][selectedBody] = currentY;
				bodies[selectedBody]->SetTransform(b2Vec2(currentX, currentY), animationAngle[selectedBody]);
			}
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
				resetMode();
			}
            break;
		case MODE_BODY_ROTATE:
			if (selectedBody != -1) {
				if (sqrt(pow(dragOffsetX - worldX, 2) + pow(dragOffsetY - worldY, 2)) < 0.1f) {
					dragOffsetAngle = atan2(worldY - dragOffsetY, worldX - dragOffsetX) - animationAngle[selectedBody];
				} else {
					float angle = atan2(worldY - dragOffsetY, worldX - dragOffsetX) - dragOffsetAngle;
					float offsetAngle = atan2(dragOffsetY - animationY[selectedBody], dragOffsetX - animationX[selectedBody]);
					float len = sqrt(pow(dragOffsetX - animationX[selectedBody], 2) + pow(dragOffsetY - animationY[selectedBody], 2));

					float newOffsetX = animationX[selectedBody] + cos(angle - animationAngle[selectedBody] + offsetAngle) * len;
					float newOffsetY = animationY[selectedBody] + sin(angle - animationAngle[selectedBody] + offsetAngle) * len;

					float transX = newOffsetX - dragOffsetX;
					float transY = newOffsetY - dragOffsetY;
					//printf("%f %f %f\n", len, transX, transY);
					animationAngle[selectedBody] = angle;

					animationX[selectedBody] -= transX;
					animationY[selectedBody] -= transY;

					animationAngle[selectedBody] = angle;

					frameAnimX[currentFrame][selectedBody] -= transX;
					frameAnimY[currentFrame][selectedBody] -= transY;
					frameAnimAngle[currentFrame][selectedBody] = angle;

					bodies[selectedBody]->SetTransform(b2Vec2(animationX[selectedBody], animationY[selectedBody]), angle);
				}
			}
			if (game->getHge()->Input_KeyUp(HGEK_RBUTTON)) {
				resetMode();
			}
			break;
		case MODE_INSERT_ANIM:
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
				animationX[bodiesCount] = game->worldX(x);
				animationY[bodiesCount] = game->worldY(y);
				animations[bodiesCount] = beingInsertedAnim;
				animationNames[bodiesCount] = beingInsertedAnimName;
				b2BodyDef bodyDef;
				bodyDef.type = b2_dynamicBody;
				bodyDef.position.Set(animationX[bodiesCount], animationY[bodiesCount]);
				bodies[bodiesCount] = game->getWorld()->CreateBody(&bodyDef);
				bodiesCount++;
				mode = MODE_INSERT_BODY;
				beingInsertedVertices.clear();
			}
			break;
        case MODE_INSERT_BODY:
            if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
                float x, y;
				game->getHge()->Input_GetMousePos(&x, &y);
				b2Vec2 vertex;
				vertex.Set(game->worldX(x) - animationX[bodiesCount - 1], game->worldY(y) - animationY[bodiesCount - 1]);
				beingInsertedVertices.push_back(vertex);
				printf("v %f %f\n", vertex.x, vertex.y);
            }
            if (game->getHge()->Input_KeyUp(HGEK_RBUTTON)) {
				b2Body* body = bodies[bodiesCount - 1];

				int i = 0;
				for (std::list<b2Vec2>::iterator it = beingInsertedVertices.begin(); it != beingInsertedVertices.end(); it++ ) {
					i++;
				}
				b2Vec2 vertices[i];
				i = 0;

				for (std::list<b2Vec2>::iterator it = beingInsertedVertices.begin(); it != beingInsertedVertices.end(); it++ ) {
					vertices[i].Set((*it).x, (*it).y);
					i++;
				}
				b2PolygonShape polygon;
				polygon.Set(vertices, i);
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &polygon;
				fixtureDef.density = 1.0f;
				fixtureDef.friction = 0.3f;
				body->CreateFixture(&fixtureDef);


                resetMode();
            }
            break;
		case MODE_INSERT_JOINT_B1:
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
				selectedBody = getPointedBody(worldX, worldY);
				if (selectedBody != -1) {
					mode = MODE_INSERT_JOINT_B2;
				}
			}
			break;
		case MODE_INSERT_JOINT_B2:
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
				selectedBody2 = getPointedBody(worldX, worldY);
				if (selectedBody2 != -1 && selectedBody2 != selectedBody) {
					mode = MODE_INSERT_JOINT;
				}
			}
			break;
		case MODE_INSERT_JOINT:
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
				b2RevoluteJointDef jointDef;
				jointDef.Initialize(bodies[selectedBody], bodies[selectedBody2], b2Vec2(worldX, worldY));
				b2RevoluteJoint* joint = (b2RevoluteJoint*)game->getWorld()->CreateJoint(&jointDef);
				joints[jointsCount] = joint;
				jointsCount++;
				resetMode();
			}
			break;
	}

	return game->update(false);
}

// This function will be called by HGE when
// the application window should be redrawn.
// Put your rendering code here.
bool RenderFunc()
{

	// RenderFunc should always return false
	game->startDraw();
	for (int i = 0; i < bodiesCount; i++) {
		animations[i]->RenderEx(game->screenX(animationX[i]), game->screenY(animationY[i]), animationAngle[i], game->getScaleFactor(), game->getScaleFactor());
		b2Body* body = bodies[i];
		DWORD color = 0xFFAA0000;
		if (i == selectedBody) {
			color = 0xFFFF0000;
		}
		if (i == selectedBody2) {
			color = 0xFF00FF00;
		}
		for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
			b2Shape* s = f->GetShape();
			if (s->GetType() == b2Shape::e_polygon) {
				b2PolygonShape* ps = (b2PolygonShape*)s;
				b2Vec2 origin(animationX[i], animationY[i]);
				b2Vec2 prev = body->GetWorldPoint(ps->GetVertex(0));
				for (int j = 1; j < ps->GetVertexCount(); j++) {
					b2Vec2 v = body->GetWorldPoint(ps->GetVertex(j));
					game->getHge()->Gfx_RenderLine(game->screenX(prev.x), game->screenY(prev.y), game->screenX(v.x), game->screenY(v.y), color);
					prev = v;
				}
				b2Vec2 v = body->GetWorldPoint(ps->GetVertex(0));
				game->getHge()->Gfx_RenderLine(game->screenX(prev.x), game->screenY(prev.y), game->screenX(v.x), game->screenY(v.y), color);
			}
		}
	}
	for (int i = 0; i < jointsCount; i++) {
		b2Joint* joint = joints[i];
		if (joint->GetType() == e_revoluteJoint) {
			game->getHge()->Gfx_RenderLine(
				game->screenX(joint->GetAnchorA().x),
				game->screenY(joint->GetAnchorA().y),
				game->screenX(joint->GetAnchorB().x),
				game->screenY(joint->GetAnchorB().y),
				0xFF0000FF
			);
		}
	}
	float x, y;
    game->getHge()->Input_GetMousePos(&x, &y);
    float worldX = game->worldX(x);
	float worldY = game->worldY(y);
	switch(mode) {
		case MODE_INSERT_ANIM:
			beingInsertedAnim->RenderEx(x, y, 0, game->getScaleFactor(), game->getScaleFactor());
            break;
        case MODE_INSERT_BODY:
            if (beingInsertedVertices.size() > 0) {
                std::list<b2Vec2>::iterator it = beingInsertedVertices.begin();
                b2Vec2 origin(animationX[bodiesCount - 1], animationY[bodiesCount - 1]);
                b2Vec2 prev = (*it) + origin;
                for (; it != beingInsertedVertices.end(); it++ ) {
                    //printf("r %f %f\n", game->screenX(prev.x), game->screenY(prev.y));
                    b2Vec2 current = (*it) + origin;
                    game->getHge()->Gfx_RenderLine(game->screenX(prev.x), game->screenY(prev.y), game->screenX(current.x), game->screenY(current.y), 0xFFFF0000);
                    prev = (*it) + origin;
                }
                game->getHge()->Gfx_RenderLine(game->screenX(prev.x), game->screenY(prev.y), x, y, 0xFFFF0000);
            }
            break;
		case MODE_BODY_ROTATE:
			game->getHge()->Gfx_RenderLine(game->screenX(dragOffsetX), game->screenY(dragOffsetY), x, y, 0xFFffa500);
			break;
	}

	game->getHge()->Gfx_RenderLine(game->screenX(0) - 10, game->screenY(0) - 10, game->screenX(0) + 10, game->screenY(0) + 10, 0xFF000000);
	game->getHge()->Gfx_RenderLine(game->screenX(0) - 10, game->screenY(0) + 10, game->screenX(0) + 10, game->screenY(0) - 10, 0xFF000000);

	game->endDraw();
	return false;
}

bool insertAnimButtonClick(hgeGUIMenuItem* sender)
{
	mode = MODE_SELECT_ANIM;
	mainWindow->Hide();
	animsWindow->Show();
}

bool insertJointButtonClick(hgeGUIMenuItem* sender)
{
	if (bodiesCount < 2) {
		return false;
	}
	mode = MODE_INSERT_JOINT_B1;
	selectedBody = -1;
	selectedBody2 = -1;
	mainWindow->Hide();
}

bool animButtonClick(hgeGUIMenuItem* sender)
{
	mode = MODE_INSERT_ANIM;
	//mainWindow->Show();
	animsWindow->Hide();
	beingInsertedAnimName = sender->getTitle();
	beingInsertedAnim = game->loadAnimation(beingInsertedAnimName);
}

bool saveConstruction(char* fn)
{
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );
	for (int i = 0; i < bodiesCount; i++) {
		b2Body* body = bodies[i];

        TiXmlElement * element = new TiXmlElement( "body" );
        doc.LinkEndChild( element );
        element->SetAttribute("animation", animationNames[i]);
        element->SetDoubleAttribute("x", animationX[i]);
        element->SetDoubleAttribute("y", animationY[i]);
        element->SetDoubleAttribute("angle", animationAngle[i]);

        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
			b2Shape* s = f->GetShape();
			TiXmlElement * shapeElement = new TiXmlElement( "shape" );
			element->LinkEndChild( shapeElement );
			if (s->GetType() == b2Shape::e_polygon) {
				shapeElement->SetAttribute("type", "polygon");
				b2PolygonShape* ps = (b2PolygonShape*)s;
				for (int j = 0; j < ps->GetVertexCount(); j++) {
					b2Vec2 v = ps->GetVertex(j);

					TiXmlElement * vertexElem;
					vertexElem = new TiXmlElement( "vertex" );
					shapeElement->LinkEndChild( vertexElem );
					vertexElem->SetDoubleAttribute("x", v.x);
					vertexElem->SetDoubleAttribute("y", v.y);
				}
			}
		}
        //cxn->SetDoubleAttribute("timeout", 123.456);
	}
	for (int i = 0; i < jointsCount; i++) {
		b2Joint* joint = joints[i];
		TiXmlElement * element = new TiXmlElement( "joint" );
        doc.LinkEndChild( element );
		if (joint->GetType() == e_revoluteJoint) {
			element->SetAttribute("type", "revolute");
			for (int k = 0; k < bodiesCount; k++) {
				if (bodies[k] == joint->GetBodyA()) {
					element->SetAttribute("bodyA", k);
				}
				if (bodies[k] == joint->GetBodyB()) {
					element->SetAttribute("bodyB", k);
				}
			}
			element->SetDoubleAttribute("x", joint->GetAnchorA().x);
			element->SetDoubleAttribute("y", joint->GetAnchorA().y);
		}
	}
	doc.SaveFile(fn);
}

bool loadConstruction(char* fn)
{
	bodies = new b2Body*[256];
	animations = new hgeAnimation*[256];
	joints = new b2Joint*[256];
	bodiesCount = 0;
	jointsCount = 0;
	animationX = new float[256];
	animationY = new float[256];
	animationAngle = new float[256];
	animationNames = new char*[256];

	printf("loading construction %s ... \n", fn);
    TiXmlDocument doc(fn);
    bool loadOkay = doc.LoadFile();
    if (loadOkay)
    {
    	b2Vec2 origin(0, 0);
        TiXmlElement* element = doc.FirstChildElement("body");
        int i = 0;
        while (element) {
        	printf("loading body...\n");
        	float x = atof(element->Attribute("x"));
			float y = atof(element->Attribute("y"));
			float angle = atof(element->Attribute("angle"));
			b2BodyDef bodyDef;
			bodyDef.type = b2_dynamicBody;
			bodyDef.position.Set(origin.x + x, origin.y + y);
			bodyDef.angle = angle;
			printf("set it at %.2f %.2f\n", origin.x, origin.y);
			b2Body* body = game->getWorld()->CreateBody(&bodyDef);

			TiXmlElement* shapeElem = element->FirstChildElement("shape");
			int j = 0;
			while (shapeElem) {
				printf("  %s shape ", shapeElem->Attribute("type"));
				if (strcmp(shapeElem->Attribute("type"), "polygon") == 0) {
					TiXmlElement* vertexElem = shapeElem->FirstChildElement("vertex");
					int k = 0;
					while (vertexElem) {
						k++;
						vertexElem = vertexElem->NextSiblingElement("vertex");
					}
					b2Vec2 vertices[k];
					vertexElem = shapeElem->FirstChildElement("vertex");
					k = 0;
					while (vertexElem) {

						float x = atof(vertexElem->Attribute("x"));
						float y = atof(vertexElem->Attribute("y"));
						printf("vertex %.2f %.2f ", x, y);
						vertices[k].Set(x, y);
						k++;
						vertexElem = vertexElem->NextSiblingElement("vertex");
					}
					b2PolygonShape polygon;
					polygon.Set(vertices, k);
					b2FixtureDef fixtureDef;
					fixtureDef.shape = &polygon;
					fixtureDef.density = 1.0f;
					fixtureDef.friction = 0.3f;
					body->CreateFixture(&fixtureDef);
				}
				if (strcmp(shapeElem->Attribute("type"), "box") == 0) {
					float hx = atof(shapeElem->Attribute("hx"));
					float hy = atof(shapeElem->Attribute("hy"));
					printf("with width %.2f and height %.2f ", hx, hy);
					b2PolygonShape dynamicBox;
					dynamicBox.SetAsBox(hx, hy);
					b2FixtureDef fixtureDef;
					fixtureDef.shape = &dynamicBox;
					fixtureDef.density = 1.0f;
					fixtureDef.friction = 0.3f;
					body->CreateFixture(&fixtureDef);
					printf("added\n");
				}

				j++;
				shapeElem = shapeElem->NextSiblingElement("shape");
			}
			delete shapeElem;
			printf("-\n");
			char* animationName = (char*)element->Attribute("animation");
			printf("-\n");
			hgeAnimation* animation = game->loadAnimation(animationName);
			printf("-\n");
			//animation->SetHotSpot(-game->getPixelsPerMeter() * atof(element->Attribute("x")), -game->getPixelsPerMeter() * atof(element->Attribute("y")));

			bodies[i]     = body;
			animations[i] = animation;
			animationX[i] = x;
			animationY[i] = y;
			animationAngle[i] = angle;

			frameAnimX[0][i] = x;
			frameAnimY[0][i] = y;
			frameAnimAngle[0][i] = angle;

			animationNames[i] = new char[64];
			int c = 0;
			for (c = 0; animationName[c] != '\0'; c++) {
			    animationNames[i][c] = animationName[c];
			}
			animationNames[i][c] = '\0';

			i++;
            element = element->NextSiblingElement("body");
        }
        bodiesCount = i;
        i = 0;
        element = doc.FirstChildElement("joint");
        while (element) {
			b2RevoluteJointDef jointDef;
			float x = atof(element->Attribute("x"));
			float y = atof(element->Attribute("y"));
			int bodyA = atoi(element->Attribute("bodyA"));
			int bodyB = atoi(element->Attribute("bodyB"));
			jointDef.Initialize(bodies[bodyA], bodies[bodyB], b2Vec2(x, y) + origin);
			//b2RevoluteJoint* joint = (b2RevoluteJoint*)
			joints[i] = game->getWorld()->CreateJoint(&jointDef);
			i++;
			element = element->NextSiblingElement("joint");
        }
        jointsCount = i;
        delete element;
    } else {
        char * error = strdup("Can't load construction: ");
        strcat(error, fn);
        Exception * e = new Exception(error);
        throw e;
    }
    printf("construction loaded\n");
}

bool saveConstructionButtonClick(hgeGUIMenuItem* sender)
{
	saveConstruction("test.xml");
}

bool loadConstructionButtonClick(hgeGUIMenuItem* sender)
{
	loadConstruction("test.xml");
}

bool insertFrameButtonClick(hgeGUIMenuItem* sender)
{
	currentFrame = framesCount;
	framesCount += 1;

	for (int i = 0; i < bodiesCount; i++) {
		frameAnimX[currentFrame][i] = animationX[i];
		frameAnimY[currentFrame][i] = animationY[i];
		frameAnimAngle[currentFrame][i] = animationAngle[i];
	}
}

void setFrame(int frame)
{
	currentFrame = frame;
	printf("frame %i\n", frame);
	for (int i = 0; i < bodiesCount; i++) {
		animationX[i] = frameAnimX[currentFrame][i];
		animationY[i] = frameAnimY[currentFrame][i];
		animationAngle[i] = frameAnimAngle[currentFrame][i];
		bodies[i]->SetTransform(b2Vec2(animationX[i], animationY[i]), animationAngle[i]);
	}
}

bool nextFrameButtonClick(hgeGUIMenuItem* sender)
{
	currentFrame += 1;
	if (currentFrame >= framesCount) {
		currentFrame = 0;
	}

	setFrame(currentFrame);
}

bool prevFrameButtonClick(hgeGUIMenuItem* sender)
{
	currentFrame -= 1;
	if (currentFrame < 0) {
		currentFrame = framesCount - 1;
	}

	setFrame(currentFrame);
}



bool saveAnimation(char* fn)
{
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );
	TiXmlElement * root = new TiXmlElement( "frames" );
	doc.LinkEndChild( root );
	root->SetAttribute("count", framesCount);
	root->SetAttribute("bodies", bodiesCount);
	for (int i = 0; i < framesCount; i++) {
        TiXmlElement * element = new TiXmlElement( "frame" );
        root->LinkEndChild( element );

        for (int j = 0; j < bodiesCount; j++) {
			TiXmlElement * bodyElement = new TiXmlElement( "body" );
			element->LinkEndChild( bodyElement );

			bodyElement->SetDoubleAttribute("x", frameAnimX[i][j]);
			bodyElement->SetDoubleAttribute("y", frameAnimY[i][j]);
			bodyElement->SetDoubleAttribute("angle", frameAnimAngle[i][j]);
        }
	}
	doc.SaveFile(fn);
}

bool loadAnimation(char* fn)
{
	framesCount = 1;
	currentFrame = 0;
	frameAnimX = new float*[256];
	frameAnimY = new float*[256];
	frameAnimAngle = new float*[256];
	for (int i = 0; i < 256; i++) {
		frameAnimX[i] = new float[256];
		frameAnimY[i] = new float[256];
		frameAnimAngle[i] = new float[256];
	}


	printf("loading animation %s ... \n", fn);
    TiXmlDocument doc(fn);
    bool loadOkay = doc.LoadFile();
    if (loadOkay)
    {
    	TiXmlElement* root = doc.FirstChildElement("frames");
        TiXmlElement* element = root->FirstChildElement("frame");
        int i = 0;
        while (element) {
			TiXmlElement* bodyElem = element->FirstChildElement("body");
			int j = 0;
			while (bodyElem) {
				frameAnimX[i][j] = atof(bodyElem->Attribute("x"));
				frameAnimY[i][j] = atof(bodyElem->Attribute("y"));
				frameAnimAngle[i][j] = atof(bodyElem->Attribute("angle"));

				j++;
				bodyElem = bodyElem->NextSiblingElement("body");
			}
			delete bodyElem;

			i++;
            element = element->NextSiblingElement("frame");
        }
        framesCount = i;
        printf("loaded animation with %i frames ... \n", framesCount);

        setFrame(0);
    }
}


bool saveAnimButtonClick(hgeGUIMenuItem* sender)
{
	saveAnimation("test_anim.xml");
}

bool loadAnimButtonClick(hgeGUIMenuItem* sender)
{
	loadAnimation("test_anim.xml");
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// Get HGE interface
	HGE * hge = hgeCreate(HGE_VERSION);
	hge->System_SetState(HGE_USESOUND, false);

	// Set up log file, frame function, render function and window title
	hge->System_SetState(HGE_LOGFILE, "construction_editor.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "SoD construction editor");

	// Set up video mode
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_SCREENWIDTH, 800);
	hge->System_SetState(HGE_SCREENHEIGHT, 600);
	hge->System_SetState(HGE_SCREENBPP, 32);
	hge->System_SetState(HGE_FPS, 60);


	b2Vec2 gravity(0.0f, 0.0f);
	b2World* world = new b2World(gravity);

	game = new Game(world, hge);

	for (int i = 0; i < 256; i++) {
		frameAnimX[i] = new float[256];
		frameAnimY[i] = new float[256];
		frameAnimAngle[i] = new float[256];
	}

	if (game->preload()) {
		hgeFont* fnt;

		fnt = new hgeFont("font1.fnt");

		mainWindow = new GUIWindow(game->getGUI(), 1, 600, 0, 200, 600);
		game->getGUI()->AddCtrl(mainWindow);

		mainWindow->AddCtrl(new hgeGUIMenuItem(2, fnt, 50, 20, 0.0f, "insert anim", insertAnimButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(3, fnt, 50, 40, 0.0f, "insert joint", insertJointButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(4, fnt, 50, 60, 0.0f, "save", saveConstructionButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(5, fnt, 50, 80, 0.0f, "load", loadConstructionButtonClick));

		mainWindow->AddCtrl(new hgeGUIMenuItem(6, fnt, 50, 120, 0.0f, "insert frame", insertFrameButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(7, fnt, 50, 140, 0.0f, "<<", prevFrameButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(8, fnt, 50, 160, 0.0f, ">>", nextFrameButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(9, fnt, 50, 180, 0.0f, "save anim", saveAnimButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(10, fnt, 50, 200, 0.0f, "load anim", loadAnimButtonClick));

		animsWindow = new GUIWindow(game->getGUI(), 20, 0, 0, 800, 600);
		game->getGUI()->AddCtrl(animsWindow);
		for (int i = 0; i < game->getAnimationsCount(); i++) {
			animsWindow->AddCtrl(new hgeGUIMenuItem(21 + i, fnt, 50, 10 + i * 20, 0.0f, game->getAnimationName(i), animButtonClick));
			//printf("foo %s\n", game->getAnimationName(i));
		}
		animsWindow->Hide();

		game->moveCamera(b2Vec2(-4, -3));

		game->loop();

		//delete fnt;

	}

	return 0;
}
