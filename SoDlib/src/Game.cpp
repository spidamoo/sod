#include "Game.h"

Game::Game(HGE * hge)
{
	this->world = world;
	this->hge = hge;

	screenWidth = 1600; screenHeight = 900;
	// Set up video mode
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_SCREENWIDTH, screenWidth);
	hge->System_SetState(HGE_SCREENHEIGHT, screenHeight);
	hge->System_SetState(HGE_SCREENBPP, 32);
	hge->System_SetState(HGE_FPS, 60);

	groundLines = new GroundLine*[1000]; groundLinesCount = 0;
	mapAnimations = new MapAnimation*[1000]; mapAnimationsCount = 0;
	characters = new Character*[100];
	charactersCount = 0;

	timeStep = 1.0f / 60.0f;
	velocityIterations = 6;
	positionIterations = 2;

	cameraPos = b2Vec2(0, 0);
	schematicDrawMode = false;

	gui = new hgeGUI();

	pixelsPerMeter = 40.0f;
	scaleFactor = 1.0f;

	gravity = b2Vec2(0, 20);

	mapWidth = 10; mapHeight = 10;

	//debugDraw = new DebugDraw(this);
	//world->SetDebugDraw((b2Draw*)debugDraw);
}

Game::~Game()
{

}

bool Game::preload()
{
    //printf("foo");
    try
    {
    	if(hge->System_Initiate()) {
			hge->Random_Seed(0);

	//		loadConstruction("box.xml", b2Vec2(10, 0));
	//		loadConstruction("box.xml", b2Vec2(11, 3));
	//		loadConstruction("box.xml", b2Vec2(10, 6));
	//
			//loadConstruction("ground.xml", b2Vec2(10, 50), b2_staticBody);

			cursorTex = hge->Texture_Load("cursor.png");
			cursorSprite = new hgeSprite(cursorTex,0,0,32,32);
			//cursorSprite->SetZ(0);

			gui->SetNavMode(HGEGUI_NONAVKEYS);
			gui->SetCursor(cursorSprite);
			gui->SetFocus(1);
			gui->Enter();

			whiteQuad.tex = 0;//bgTex;
			whiteQuad.blend = 2;
			whiteQuad.v[0].z = 1;
			whiteQuad.v[1].z = 1;
			whiteQuad.v[2].z = 1;
			whiteQuad.v[3].z = 1;

			whiteTriple.tex = 0;//bgTex;
			whiteTriple.blend = 2;
			whiteTriple.v[0].z = 1;
			whiteTriple.v[1].z = 1;
			whiteTriple.v[2].z = 1;

			loadAnimationNames("animations.xml");

			return true;
    	} else {
    		char * error = strdup(hge->System_GetErrorMessage());
			Exception * e = new Exception(error);
			throw e;
		}
    }
    catch(Exception *error)
    {
        error->print();
    }
    return false;
}

bool Game::loadAnimationNames(char* fileName)
{
	printf("loading anim list %s ... \n", fileName);
    TiXmlDocument doc(fileName);
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
		TiXmlElement* element = doc.FirstChildElement("animation");
        int i = 0;
        while (element) {
			i++;
            element = element->NextSiblingElement("animation");
        }
        animationNames = new char*[i];
		animationsCount = i;

        element = doc.FirstChildElement("animation");
        i = 0;
        while (element) {
			animationNames[i] = new char[64];
			int c = 0;
			for (c = 0; element->Attribute("file")[c] != '\0'; c++) {
			    animationNames[i][c] = element->Attribute("file")[c];
			}
			animationNames[i][c] = '\0';

			//printf("c %s\n", animationNames[i]);
			i++;
            element = element->NextSiblingElement("animation");

        }
    	return true;
    } else {
    	printf("failed\n");
    	return false;
    }
}

int Game::getAnimationsCount()
{
	return animationsCount;
}

char* Game::getAnimationName(int index)
{
    //printf("g %s\n", animationNames[index]);
	return animationNames[index];
}

void Game::loop()
{
	// Let's rock now!
	hge->System_Start();

	delete gui;
	delete cursorSprite;
	hge->Texture_Free(cursorTex);

	// Clean up and shutdown
	hge->System_Shutdown();
	hge->Release();
}

bool Game::update()
{
	return update(true);
}
bool Game::update(bool withPhysics)
{
	// Get the time elapsed since last call of FrameFunc().
	// This will help us to synchronize on different
	// machines and video modes.
	dt = hge->Timer_GetDelta();
	hge->Input_GetMousePos(&mouseX, &mouseY);
	worldMouseX = worldX(mouseX); worldMouseY = worldY(mouseY);
	if (withPhysics) updateWorld(dt);
	updateGui(dt);

	return updateControls();
}

void Game::updateGui(float dt)
{

	gui->Update(dt);
}
void Game::updateWorld(float dt)
{
    timeStep = dt;
	//world->Step(timeStep, velocityIterations, positionIterations);
	for (int i = 0; i < charactersCount; i++) {
		characters[i]->update(dt);
	}
	cameraPos = characters[0]->getPosition() - b2Vec2(0.5 * screenWidth / pixelsPerMeter, 0.5 * screenHeight / pixelsPerMeter);
}
bool Game::updateControls()
{
	// Process keys
	if (hge->Input_GetKeyState(HGEK_ESCAPE)) return true;
	if (hge->Input_KeyDown(HGEK_TAB))        schematicDrawMode = !schematicDrawMode;
	//if (hge->Input_KeyDown(HGEK_SPACE))      loadConstruction("box.xml", b2Vec2(10 + hge->Random_Float(-1, 1), 0));
	return false;
}

void Game::startDraw()
{
	// Begin rendering quads.
	// This function must be called
	// before any actual rendering.
	hge->Gfx_BeginScene();

	// Clear screen with skyblue color
	DWORD color = 0x7fc7ff;
	if (schematicDrawMode) {
		color = 0;
	}
	hge->Gfx_Clear(color);
}

void Game::endDraw()
{

	gui->Render();
	// End rendering and update the screen
	hge->Gfx_EndScene();
}

void Game::drawGame()
{
	for (int i = 0; i < charactersCount; i++) {
		characters[i]->draw(schematicDrawMode);
	}
	for (int i = 0; i < mapAnimationsCount; i++) {
        mapAnimations[i]->draw(schematicDrawMode);
	}
	if (schematicDrawMode) {
		for (int i = 0; i < groundLinesCount; i++) {
			groundLines[i]->debugDraw();
		}
		//world->DrawDebugData();
	}
}

bool Game::draw()
{
	startDraw();
	drawGame();
	endDraw();
	return false;
}

void Game::drawLine(float x1, float y1, float x2, float y2, DWORD color)
{
	getHge()->Gfx_RenderLine(x1, y1, x2, y2, color);
}
void Game::drawLine(b2Vec2 p1, b2Vec2 p2, DWORD color)
{
	this->drawLine(p1.x, p1.y, p2.x, p2.y, color);
}

void Game::drawRect(float left, float top, float right, float bottom, DWORD color, DWORD bgcolor)
{
	whiteQuad.v[0].x = left;  whiteQuad.v[0].y = top;    whiteQuad.v[0].col = bgcolor;
	whiteQuad.v[1].x = right; whiteQuad.v[1].y = top;    whiteQuad.v[1].col = bgcolor;
	whiteQuad.v[2].x = right; whiteQuad.v[2].y = bottom; whiteQuad.v[2].col = bgcolor;
	whiteQuad.v[3].x = left;  whiteQuad.v[3].y = bottom; whiteQuad.v[3].col = bgcolor;
	getHge()->Gfx_RenderQuad(&whiteQuad);

	getHge()->Gfx_RenderLine(left, top, right, top, color);
	getHge()->Gfx_RenderLine(right, top, right, bottom, color);
	getHge()->Gfx_RenderLine(right, bottom, left, bottom, color);
	getHge()->Gfx_RenderLine(left, bottom, left, top, color);
}

void Game::drawRect(float centerX, float centerY, float hw, float hh, float angle, DWORD color, DWORD bgcolor)
{
	float cornerAngle1 = atan2(hh, hw) + angle;
	float cornerAngle2 = atan2(hh, -hw) + angle;
	float length = sqrtf(powf(hw, 2) + powf(hh, 2));
	float width1 = cos(cornerAngle1)  * length;
	float height1 = sin(cornerAngle1) * length;
	float width2 = cos(cornerAngle2)  * length;
	float height2 = sin(cornerAngle2) * length;
	whiteQuad.v[0].x = centerX + width1;  whiteQuad.v[0].y = centerY + height1; whiteQuad.v[0].col = bgcolor;
	whiteQuad.v[1].x = centerX + width2;  whiteQuad.v[1].y = centerY + height2; whiteQuad.v[1].col = bgcolor;
	whiteQuad.v[2].x = centerX - width1;  whiteQuad.v[2].y = centerY - height1; whiteQuad.v[2].col = bgcolor;
	whiteQuad.v[3].x = centerX - width2;  whiteQuad.v[3].y = centerY - height2; whiteQuad.v[3].col = bgcolor;
	getHge()->Gfx_RenderQuad(&whiteQuad);

	getHge()->Gfx_RenderLine(whiteQuad.v[0].x, whiteQuad.v[0].y, whiteQuad.v[1].x, whiteQuad.v[1].y, color);
	getHge()->Gfx_RenderLine(whiteQuad.v[1].x, whiteQuad.v[1].y, whiteQuad.v[2].x, whiteQuad.v[2].y, color);
	getHge()->Gfx_RenderLine(whiteQuad.v[2].x, whiteQuad.v[2].y, whiteQuad.v[3].x, whiteQuad.v[3].y, color);
	getHge()->Gfx_RenderLine(whiteQuad.v[3].x, whiteQuad.v[3].y, whiteQuad.v[0].x, whiteQuad.v[0].y, color);
}

void Game::drawArc(float x, float y, float r, float start, float end, DWORD color, DWORD bgcolor)
{
	int steps = r * (end - start) * 0.1;
	if (steps < 10)
		steps = 10;
	if (steps > 300)
		steps = 300;
	float step = (end - start) / steps;
	float angle = start;
	float prevX = x + r * cos(angle);
	float prevY = y - r * sin(angle);
	float nextX;
	float nextY;
	whiteTriple.v[0].col = bgcolor; whiteTriple.v[1].col = bgcolor; whiteTriple.v[2].col = bgcolor;
	whiteTriple.v[0].x = x;  whiteTriple.v[0].y = y;
	for (int i = 0; i < steps; i++) {
		angle += step;
		nextX = x + r * cos(angle);
		nextY = y - r * sin(angle);

		whiteTriple.v[1].x = prevX;  whiteTriple.v[1].y = prevY;
		whiteTriple.v[2].x = nextX;  whiteTriple.v[2].y = nextY;
		getHge()->Gfx_RenderTriple(&whiteTriple);

		getHge()->Gfx_RenderLine(prevX, prevY, nextX, nextY, color);
		prevX = nextX; prevY = nextY;
	}
}
void Game::drawPoly(b2PolygonShape* poly, b2Transform transform, b2Vec2 origin, float scale, DWORD color, DWORD bgcolor)
{
	b2Vec2 prev = b2Mul(transform, poly->GetVertex(0));
	b2Vec2 first = prev;
	whiteTriple.v[0].col = bgcolor; whiteTriple.v[1].col = bgcolor; whiteTriple.v[2].col = bgcolor;
	whiteTriple.v[0].x = origin.x + prev.x * scale;  whiteTriple.v[0].y = origin.y + prev.y * scale;
	for (int i = 1; i < poly->GetVertexCount(); i++) {
		b2Vec2 current = b2Mul(transform, poly->GetVertex(i));
		if (i > 0) {
			whiteTriple.v[1].x = origin.x + prev.x    * scale;  whiteTriple.v[1].y = origin.y + prev.y    * scale;
			whiteTriple.v[2].x = origin.x + current.x * scale;  whiteTriple.v[2].y = origin.y + current.y * scale;
			getHge()->Gfx_RenderTriple(&whiteTriple);
		}
		getHge()->Gfx_RenderLine(
			origin.x + prev.x * scale,
			origin.y + prev.y * scale,
			origin.x + current.x * scale,
			origin.y + current.y * scale,
			color
		);
		prev = current;
	}
	getHge()->Gfx_RenderLine(
		origin.x + prev.x * scale,
		origin.y + prev.y * scale,
		origin.x + first.x * scale,
		origin.y + first.y * scale,
		color
	);
}

void 	Game::DrawPolygon (const b2Vec2 *vertices, int32 vertexCount, DWORD color, DWORD bgcolor)
{
	b2Vec2 first = getPixelsPerMeter() * getScaleFactor() * (vertices[0] - cameraPos);
	b2Vec2 prev = first;
	whiteTriple.v[0].col = bgcolor; whiteTriple.v[1].col = bgcolor; whiteTriple.v[2].col = bgcolor;
	whiteTriple.v[0].x = prev.x;  whiteTriple.v[0].y = prev.y;
	for (int i = 1; i < vertexCount; i++) {
		b2Vec2 current = getPixelsPerMeter() * getScaleFactor() * (vertices[i] - cameraPos);
		if (i > 0) {
			whiteTriple.v[1].x = prev.x   ;  whiteTriple.v[1].y = prev.y;
			whiteTriple.v[2].x = current.x;  whiteTriple.v[2].y = current.y;
			getHge()->Gfx_RenderTriple(&whiteTriple);
		}
		getHge()->Gfx_RenderLine(
			prev.x,
			prev.y,
			current.x,
			current.y,
			color
		);
		prev = current;
	}
	getHge()->Gfx_RenderLine(
		prev.x,
		prev.y,
		first.x,
		first.y,
		color
	);
}
void 	Game::DrawPolygonScreen (const b2Vec2 *vertices, int32 vertexCount, DWORD color, DWORD bgcolor)
{
	b2Vec2 first = vertices[0];
	b2Vec2 prev = first;
	whiteTriple.v[0].col = bgcolor; whiteTriple.v[1].col = bgcolor; whiteTriple.v[2].col = bgcolor;
	whiteTriple.v[0].x = prev.x;  whiteTriple.v[0].y = prev.y;
	for (int i = 1; i < vertexCount; i++) {
		b2Vec2 current = vertices[i];
		if (i > 0) {
			whiteTriple.v[1].x = prev.x   ;  whiteTriple.v[1].y = prev.y;
			whiteTriple.v[2].x = current.x;  whiteTriple.v[2].y = current.y;
			getHge()->Gfx_RenderTriple(&whiteTriple);
		}
		getHge()->Gfx_RenderLine(
			prev.x,
			prev.y,
			current.x,
			current.y,
			color
		);
		prev = current;
	}
	getHge()->Gfx_RenderLine(
		prev.x,
		prev.y,
		first.x,
		first.y,
		color
	);
}

void Game::DrawCircle (const b2Vec2 &center, float32 radius, DWORD color, DWORD bgcolor)
{
	b2Vec2 screenCenter = getPixelsPerMeter() * getScaleFactor() * (center - cameraPos);
	drawCircle(screenCenter.x, screenCenter.y, getPixelsPerMeter() * getScaleFactor() * radius, color, bgcolor);
}

void 	Game::DrawSegment (const b2Vec2 &p1, const b2Vec2 &p2, DWORD color)
{
	b2Vec2 screenP1 = getPixelsPerMeter() * getScaleFactor() * (p1 - cameraPos);
	b2Vec2 screenP2 = getPixelsPerMeter() * getScaleFactor() * (p2 - cameraPos);
	hge->Gfx_RenderLine(screenP1.x, screenP1.y, screenP2.x, screenP2.y, color);
}
void 	Game::DrawTransform (const b2Transform &xf)
{
	b2Vec2 p1 = xf.p, p2;
	b2Vec2 screenP1 = getPixelsPerMeter() * getScaleFactor() * (p1 - cameraPos);
	const float32 k_axisScale = 0.4f;
	//glBegin(GL_LINES);

	//glColor3f(1.0f, 0.0f, 0.0f);
	//glVertex2f(p1.x, p1.y);
	p2 = p1 + k_axisScale * xf.q.GetXAxis();
	b2Vec2 screenP2 = getPixelsPerMeter() * getScaleFactor() * (p2 - cameraPos);
	//glVertex2f(p2.x, p2.y);
	hge->Gfx_RenderLine(screenP1.x, screenP1.y, screenP2.x, screenP2.y, 0xFFFF0000);

	//glColor3f(0.0f, 1.0f, 0.0f);
	//glVertex2f(p1.x, p1.y);
	p2 = p1 + k_axisScale * xf.q.GetYAxis();
	screenP2 = getPixelsPerMeter() * getScaleFactor() * (p2 - cameraPos);
	//glVertex2f(p2.x, p2.y);
	hge->Gfx_RenderLine(screenP1.x, screenP1.y, screenP2.x, screenP2.y, 0xFF00FF00);

	//glEnd();
}

void Game::drawCircle(float x, float y, float r, DWORD color)
{
	drawArc(x, y, r, 0, M_PI * 2, color, 0);
}
void Game::drawCircle(float x, float y, float r, DWORD color, DWORD bgcolor)
{
	drawArc(x, y, r, 0, M_PI * 2, color, bgcolor);
}


hgeAnimation* Game::loadAnimation(char * fn)
{
	printf("loading animation %s ...\n", fn);
	TiXmlDocument doc(fn);
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
    	printf("load okay... ");
    	TiXmlElement* element = doc.FirstChildElement("animation");
		HTEXTURE tex = hge->Texture_Load(element->Attribute("texture"));
		printf("texture %s loaded... ", element->Attribute("texture"));
		int nframes = atoi(element->Attribute("nframes"));
		float fps = atof(element->Attribute("fps"));
		float x = atof(element->Attribute("x"));
		float y = atof(element->Attribute("y"));
		float w = atof(element->Attribute("w"));
		float h = atof(element->Attribute("h"));
		printf("creating animation... ");
		hgeAnimation* newObject = new hgeAnimation(tex, nframes, fps, x, y, w, h);
		newObject->SetHotSpot(w / 2, h / 2);\
		//newObject->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_ZWRITE);
		//newObject->SetZ(0.5f);
		printf("done\n");
		printf("animation loaded\n");
		return newObject;
    } else {
        char * error = strdup("Can't load animation: ");
        strcat(error, fn);
        Exception * e = new Exception(error);
        throw e;
    }
}

Character * Game::loadPlayerCharacter(char * fn, b2Vec2 origin)
{
	printf("loading player character %s ... \n", fn);
    TiXmlDocument doc(fn);
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
		TiXmlElement* characterElem = doc.FirstChildElement("character");
		PlayerCharacter* newObject = new PlayerCharacter(characterElem, this, origin);
		//newObject->setPosition(origin);

		characters[charactersCount] = newObject;
		//printf("character loaded\n");
		charactersCount++;

		return newObject;
    }
}

Character * Game::loadNonPlayerCharacter(char * fn, b2Vec2 origin)
{
	printf("loading player character %s ... \n", fn);
    TiXmlDocument doc(fn);
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
		TiXmlElement* characterElem = doc.FirstChildElement("character");
		NonPlayerCharacter* newObject = new NonPlayerCharacter(characterElem, this, origin);

		characters[charactersCount] = newObject;
		//printf("character loaded\n");
		charactersCount++;

		return newObject;
    }
}

b2Fixture* Game::loadFixture(b2Body* body, TiXmlElement* elem)
{
	printf("  %s shape ", elem->Attribute("type"));
	if (strcmp(elem->Attribute("type"), "polygon") == 0) {
		TiXmlElement* vertexElem = elem->FirstChildElement("vertex");
		int k = 0;
		while (vertexElem) {
			k++;
			vertexElem = vertexElem->NextSiblingElement("vertex");
		}
		b2Vec2 vertices[k];
		vertexElem = elem->FirstChildElement("vertex");
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
		fixtureDef.friction = 100.0f;
		return body->CreateFixture(&fixtureDef);
	}
	if (strcmp(elem->Attribute("type"), "box") == 0) {
		float hx = atof(elem->Attribute("hx"));
		float hy = atof(elem->Attribute("hy"));
		printf("with width %.2f and height %.2f ", hx, hy);
		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(hx, hy);
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 1.0f;
		printf("added\n");
		return body->CreateFixture(&fixtureDef);
	}
}

void Game::addGroundLine(GroundLine* newGL)
{
	groundLines[groundLinesCount] = newGL;
	groundLinesCount++;
}
void Game::addMapAnimation(MapAnimation* newMA)
{
	mapAnimations[mapAnimationsCount] = newMA;
	mapAnimationsCount++;
}

int Game::getCharactersCount()
{
	return charactersCount;
}
Character* Game::getCharacter(int index)
{
	return characters[index];
}

HGE* Game::getHge()
{
	return hge;
}
b2World* Game::getWorld()
{
	return world;
}

hgeGUI* Game::getGUI()
{
	return gui;
}

int Game::getGroundLinesCount()
{
	return groundLinesCount;
}
GroundLine* Game::getGroundLine(int index)
{
	if (index < groundLinesCount) {
		return groundLines[index];
	} else {
		return NULL;
	}
}

float Game::worldX(float screenX)
{
    return (screenX / (pixelsPerMeter * scaleFactor)) + cameraPos.x;
}
float Game::worldY(float screenY)
{
    return (screenY / (pixelsPerMeter * scaleFactor)) + cameraPos.y;
}

float Game::screenX(float worldX)
{
    return (worldX - cameraPos.x) * (pixelsPerMeter * scaleFactor);
}
float Game::screenY(float worldY)
{
    return (worldY - cameraPos.y) * (pixelsPerMeter * scaleFactor);
}
b2Vec2 Game::screenPos(b2Vec2 worldPos)
{
    return b2Vec2(this->screenX(worldPos.x), this->screenY(worldPos.y));
}

float Game::getScaleFactor()
{
    return scaleFactor;
}
float Game::getPixelsPerMeter()
{
    return pixelsPerMeter;
}
float Game::getFullScale()
{
    return scaleFactor * pixelsPerMeter;
}

void Game::setScale(float scale)
{
	scaleFactor = scale;
}
void Game::moveCamera(b2Vec2 diff)
{
	cameraPos += diff;
}
void Game::setCamera(b2Vec2 pos)
{
	cameraPos = pos;
}
b2Vec2 Game::getCameraPos()
{
	return cameraPos;
}
float Game::getWorldMouseX()
{
	return worldMouseX;
}
float Game::getWorldMouseY()
{
	return worldMouseY;
}
void Game::moveScreen(b2Vec2 diff)
{
	float scale = 1 / (scaleFactor * pixelsPerMeter);
	diff *= scale;
	cameraPos += diff;
}

b2Vec2 Game::getGravity()
{
	return gravity;
}

float Game::getTimeStep()
{
	return timeStep;
}

int Game::getScreenWidth()
{
	return screenWidth;
}
int Game::getScreenHeight()
{
	return screenHeight;
}
float Game::getMapWidth()
{
	return mapWidth;
}
float Game::getMapHeight()
{
	return mapHeight;
}

float Game::getWorldScreenWidth()
{
	return screenWidth / (scaleFactor * pixelsPerMeter);
}
float Game::getWorldScreenHeight()
{
	return screenHeight / (scaleFactor * pixelsPerMeter);
}

void Game::loadMap(char* fn)
{
	//mapAnimations = new hgeAnimation*[256];

	//animationsCount = 0;

	//animationX = new float[256];
	//animationY = new float[256];
	//animationAngle = new float[256];
	//animationNames = new char*[256];


	printf("loading map %s ... \n", fn);
    TiXmlDocument doc(fn);
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
    	TiXmlElement* root = doc.FirstChildElement("map");

        if (root->Attribute("width")) {
			mapWidth = atof(root->Attribute("width"));
		}
    	if (root->Attribute("height")) {
			mapHeight = atof(root->Attribute("height"));
		}

        TiXmlElement* element = root->FirstChildElement("animation");
        int i = 0;
        while (element) {
        	printf("loading animation...\n");
        	float x = atof(element->Attribute("x"));
			float y = atof(element->Attribute("y"));
			float angle = atof(element->Attribute("angle"));

			char* animationName = (char*)element->Attribute("file");
			hgeAnimation* animation = loadAnimation(animationName);
			delete animationName;

			addMapAnimation(new MapAnimation(this, animation, x, y, angle));

			i++;
            element = element->NextSiblingElement("animation");
        }
        mapAnimationsCount = i;

        i = 0;
        element = root->FirstChildElement("ground_line");
        while (element) {
        	printf("loading ground line...\n");
			addGroundLine(new GroundLine(this, atof(element->Attribute("x1")), atof(element->Attribute("y1")), atof(element->Attribute("x2")), atof(element->Attribute("y2"))));

			i++;
            element = element->NextSiblingElement("ground_line");
        }
        groundLinesCount = i;
    } else {
        printf("failed\n");
    }

}

b2Vec2 intersection(b2Vec2 p1, b2Vec2 p2, b2Vec2 p3, b2Vec2 p4) {
	// Store the values for fast access and easy
	// equations-to-code conversion
	float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
	float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;

	float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	// If d is zero, there is no intersection
	if (d == 0) return b2Vec2_zero;

	// Get the x and y
	float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
	float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
	float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

	// Check if the x and y coordinates are within both lines
	if ( x < std::min(x1, x2) || x > std::max(x1, x2) ||
	x < std::min(x3, x4) || x > std::max(x3, x4) ) return b2Vec2_zero;
	if ( y < std::min(y1, y2) || y > std::max(y1, y2) ||
	y < std::min(y3, y4) || y > std::max(y3, y4) ) return b2Vec2_zero;

	// Return the point of intersection
	b2Vec2 ret(x, y);
	return ret;
}

float distanceToSegment(float x1, float y1, float x2, float y2, float pointX, float pointY)
{
    float diffX = x2 - x1;
    float diffY = y2 - y1;
    if ((diffX == 0) && (diffY == 0)) {
        diffX = pointX - x1;
        diffY = pointY - y1;
        return sqrt(diffX * diffX + diffY * diffY);
    }

    float t = ((pointX - x1) * diffX + (pointY - y1) * diffY) / (diffX * diffX + diffY * diffY);

    if (t < 0) {
        ///point is nearest to the first point i.e x1 and y1
        diffX = pointX - x1;
        diffY = pointY - y1;
    } else if (t > 1) {
        ///point is nearest to the end point i.e x2 and y2
        diffX = pointX - x2;
        diffY = pointY - y2;
    } else {
        ///if perpendicular line intersect the line segment.
        diffX = pointX - (x1 + t * diffX);
        diffY = pointY - (y1 + t * diffY);
    }

    ///returning shortest distance
    return sqrt(diffX * diffX + diffY * diffY);
}
