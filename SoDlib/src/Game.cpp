#include "Game.h"

Game::Game(HGE * hge) {
	this->world = world;
	this->hge = hge;

	//groundLines = new GroundLine*[1000];
	groundLinesCount = 0;
	//mapAnimations = new MapAnimation*[1000];
	mapAnimationsCount = 0;
	characters = new Character*[100];
	charactersCount = 0;

    effects = new Effect**[2];
	effects[EFFECT_LIST_NORMAL]     = new Effect*[SETTING_MAX_EFFECTS];
	effects[EFFECT_LIST_BACKGROUND] = new Effect*[SETTING_MAX_EFFECTS];
	effectsCount = new int[2];
	effectsCount[EFFECT_LIST_NORMAL]     = 0;
	effectsCount[EFFECT_LIST_BACKGROUND] = 0;

	effectPrototypesCount = 0;

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
	halfMapWidth = mapWidth * 0.5f; halfMapHeight = mapHeight * 0.5f;
	bgcolor = 0x7fc7ff;

	counters = new float[COUNTERS_COUNT];
	displayedCounters = new float[COUNTERS_COUNT];

	animationNames = new char*[1000];
	animations = new hgeAnimation*[1000];
	animationsCount = 0;

	textureNames = new char*[1000];
	textures = new HTEXTURE[1000];
	texturesCount = 0;

	screenWidth = 1600; screenHeight = 900;
	viewportWidth = screenWidth / (pixelsPerMeter * scaleFactor);
	viewportHeight = screenHeight / (pixelsPerMeter * scaleFactor);
	halfViewportWidth = viewportWidth * 0.5f;
	halfViewportHeight = viewportHeight * 0.5f;
	// Set up video mode
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_SCREENWIDTH, screenWidth);
	hge->System_SetState(HGE_SCREENHEIGHT, screenHeight);
	hge->System_SetState(HGE_SCREENBPP, 32);
	hge->System_SetState(HGE_FPS, 0);
	hge->System_SetState(HGE_TEXTUREFILTER, false);

	//debugDraw = new DebugDraw(this);
	//world->SetDebugDraw((b2Draw*)debugDraw);

	Game::instance = this;
}

Game::~Game() {

}

bool Game::preload() {
    //printf("foo");
    try
    {
    	if(hge->System_Initiate()) {
			hge->Random_Seed();

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

			defaultFont = new hgeFont("font1.fnt");

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

void Game::loop() {
	// Let's rock now!
	hge->System_Start();

	delete gui;
	delete cursorSprite;
	hge->Texture_Free(cursorTex);

	// Clean up and shutdown
	hge->System_Shutdown();
	hge->Release();
}

bool Game::update() {
	return update(true);
}
bool Game::update(bool withPhysics) {
    clock_t startT = clock();
	// Get the time elapsed since last call of FrameFunc().
	// This will help us to synchronize on different
	// machines and video modes.
	dt = hge->Timer_GetDelta();
	hge->Input_GetMousePos(&mouseX, &mouseY);
	worldMouseX = worldX(mouseX); worldMouseY = worldY(mouseY);
	if (withPhysics) updateWorld(dt);
	updateGui(dt);

    clock_t endT = clock();
    counters[COUNTER_UPDATE] += (float)(endT - startT);
    counters[COUNTER_TOTAL] = (counters[COUNTER_UPDATE] + counters[COUNTER_DRAW]);
	return updateControls();
}

void Game::updateGui(float dt) {
	gui->Update(dt);
}
void Game::updateWorld(float dt) {
    timeStep = dt;
	//world->Step(timeStep, velocityIterations, positionIterations);
	for (int i = 0; i < charactersCount; i++) {
		characters[i]->update(dt);
	}
	for (int i = 0; i < platformsCount; i++) {
        platforms[i]->update(dt);
	}

	clock_t eus = clock();
	for (int i = 0; i < 2; i++) {
        int effectsToRemove[ effectsCount[i] ];
        int effectsToRemoveCount = 0;
        for (int j = 0; j < effectsCount[i]; j++) {
            effects[i][j]->update(dt);
            if ( effects[i][j]->getTime() < 0 ) {
                effectsToRemove[effectsToRemoveCount] = j;
                effectsToRemoveCount++;
            }
        }
        for (int j = effectsToRemoveCount - 1; j >= 0 ; j--) {
            removeEffect( effectsToRemove[j], i );
        }
	}
	clock_t eue = clock();
	counters[COUNTER_EFFECTS_UPDATE] += (float)(eue - eus);

	if (charactersCount > 0) {
        cameraPos = characters[0]->getPosition();
        if (cameraPos.x - halfViewportWidth < -halfMapWidth) {
            cameraPos.x = -halfMapWidth + halfViewportWidth;
        }
        if (cameraPos.x + halfViewportWidth > halfMapWidth) {
            cameraPos.x = halfMapWidth - halfViewportWidth;
        }
        if (cameraPos.y - halfViewportHeight < -halfMapHeight) {
            cameraPos.y = -halfMapHeight + halfViewportHeight;
        }
        if (cameraPos.y + halfViewportHeight > halfMapHeight) {
            cameraPos.y = halfMapHeight - halfViewportHeight;
        }
	}
}
bool Game::updateControls() {
	/// Process keys
	if (hge->Input_GetKeyState(HGEK_ESCAPE)) return true;
	if (hge->Input_KeyDown(HGEK_TAB)) {
        schematicDrawMode = !schematicDrawMode;
	}
	if (hge->Input_KeyDown(HGEK_P)) {
        drawPerformanceInfo = (drawPerformanceInfo + 1) % 3;
        for (int i = 0; i < COUNTERS_COUNT; i++) {
            counters[i] = 0;
        }
        timer = hge->Timer_GetTime();
	}
	//if (hge->Input_KeyDown(HGEK_SPACE))      loadConstruction("box.xml", b2Vec2(10 + hge->Random_Float(-1, 1), 0));
	return false;
}

void Game::startDraw() {
	hge->Gfx_BeginScene();

	DWORD color = bgcolor;
	if (schematicDrawMode) {
		color = 0;
	}
	hge->Gfx_Clear(color);
}
void Game::endDraw() {

	gui->Render();
	/// End rendering and update the screen
	hge->Gfx_EndScene();
}

void Game::drawGame() {
    clock_t st = clock();
    if (schematicDrawMode) {
		for (int i = 0; i < groundLinesCount; i++) {
			groundLines[i]->debugDraw();
		}
		//world->DrawDebugData();
	}
	clock_t et = clock();
	updateCounter(COUNTER_DRAW_GL, et - st);

	st = clock();

	for (int i = 0; i < backLayersCount; i++) {
        for (int j = 0; j < backLayersMapAnimationsCounts[i]; j++) {
            mapAnimations[ backLayersMapAnimations[i][j] ]->draw(backLayerRatios[i], schematicDrawMode);
        }
	}

	et = clock();
	float map_t = et - st;

    st = clock();
	for (int i = 0; i < effectsCount[EFFECT_LIST_BACKGROUND]; i++) {
        effects[EFFECT_LIST_BACKGROUND][i]->draw(schematicDrawMode);
	}
	et = clock();
	float effects_t = et - st;

	st = clock();
	for (int i = 0; i < charactersCount; i++) {
		characters[i]->draw(schematicDrawMode);
	}
	et = clock();


	updateCounter(COUNTER_DRAW_CHARACTERS, et - st);

	st = clock();
	for (int i = 0; i < effectsCount[EFFECT_LIST_NORMAL]; i++) {
        effects[EFFECT_LIST_NORMAL][i]->draw(schematicDrawMode);
	}
	et = clock();
	updateCounter(COUNTER_DRAW_EFFECTS, effects_t + et - st);


	st = clock();

	for (int i = 0; i < frontLayersCount; i++) {
        for (int j = 0; j < frontLayersMapAnimationsCounts[i]; j++) {
            mapAnimations[ frontLayersMapAnimations[i][j] ]->draw(frontLayerRatios[i], schematicDrawMode);
        }
	}

	if (schematicDrawMode) {
        for (int i = 0; i < groundLinesCount; i++) {
            groundLines[i]->debugDraw();
        }
	}

	et = clock();
	updateCounter(COUNTER_DRAW_MAP, map_t + et - st);

    st = clock();
	if (drawPerformanceInfo) {
        int i = 0;
        float totalTime = (displayedCounters[COUNTER_TOTAL]) / CLOCKS_PER_SEC;
        float secondLength = 200 / totalTime;
        int currentLevel = 0;
        float* offsets = new float[5];
        float currentOffset = 0;
        float prevOffset = 0;
        for (i = 1; i < COUNTERS_COUNT; i++) {
            int level = countSpaces(COUNTER_NAMES[i]);
            if (level > currentLevel) {
                offsets[currentLevel] = currentOffset;
                currentOffset = prevOffset;
            }
            if (level < currentLevel) {
                currentOffset = offsets[level];
            }
            currentLevel = level;

            float time = displayedCounters[i] / CLOCKS_PER_SEC;

            drawRect( (currentOffset) * secondLength, 0, (currentOffset + time) * secondLength, 50 - 10 * currentLevel, 0xFF333333, COUNTER_COLORS[i] );
            prevOffset = currentOffset;
            currentOffset += time;
            defaultFont->SetColor(COUNTER_COLORS[i]);
            defaultFont->printf(0, 60 + i * 20, HGETEXT_LEFT, "%s: %f", COUNTER_NAMES[i], time);
        }
        delete offsets;
        defaultFont->SetColor(0xFFFFFFFF);
        float fps = 1.0f / dt;
        if (fps < 50) {
            defaultFont->SetColor(0xFFFF0000);
        }
        float fullTime = 1.0f;
        if (drawPerformanceInfo == 1) {
            fullTime = ( hge->Timer_GetTime() - timer );
        }
        defaultFont->printf(200, 0, HGETEXT_LEFT, "dt: %f fps: %f busy/total time: %f/%f=%02.0f%%", dt, fps, displayedCounters[COUNTER_TOTAL] / CLOCKS_PER_SEC, fullTime, 100 * (displayedCounters[COUNTER_TOTAL] / CLOCKS_PER_SEC) / fullTime );
        defaultFont->SetColor(0xFFFFFFFF);
        i++;
        if (effectsCount[0] + effectsCount[1] < 200) {
            defaultFont->SetColor(0xFFFFFFFF);
        } else if (effectsCount[0] + effectsCount[1] < 500) {
            defaultFont->SetColor(0xFF00FF00 + ( ( ( effectsCount[0] + effectsCount[1] - 200 ) * 255 / 300 ) << 16 ) );
        } else if (effectsCount[0] + effectsCount[1] < 800) {
            defaultFont->SetColor(0xFFFF0000 + ( ( ( 800 - effectsCount[0] + effectsCount[1] ) * 255 / 300 ) << 8 ) );
        } else {
            defaultFont->SetColor(0xFFFF0000);
        }
        defaultFont->printf(200, 20, HGETEXT_LEFT, "effects: %d", effectsCount[0] + effectsCount[1]);
	}
	et = clock();
	updateCounter(COUNTER_DRAW_PERFORMANCE_INFO, et - st);

    if (drawPerformanceInfo == 1) {
        for (int i = 0; i < COUNTERS_COUNT; i++) {
            displayedCounters[i] = counters[i];
        }
    }
	if (drawPerformanceInfo == 2) {
        if ( (hge->Timer_GetTime() - timer) > 1 ) {
            for (int i = 0; i < COUNTERS_COUNT; i++) {
                displayedCounters[i] = counters[i];
                counters[i] = 0;
            }
            timer = hge->Timer_GetTime();
        }
	}
}
bool Game::draw() {
    clock_t startT = clock();
	startDraw();
	//updateCounter(COUNTER_DRAW_START, clock() - startT);
	clock_t mt = clock();
	drawGame();
	updateCounter(COUNTER_DRAW_GAME, clock() - mt);
	mt = clock();
	endDraw();
	clock_t endT = clock();
	counters[COUNTER_DRAW_END] += (float)(endT - mt);
	counters[COUNTER_DRAW] += (float)(endT - startT);
	return false;
}

void Game::drawLine(float x1, float y1, float x2, float y2, DWORD color) {
	getHge()->Gfx_RenderLine(x1, y1, x2, y2, color);
}
void Game::drawLine(b2Vec2 p1, b2Vec2 p2, DWORD color) {
	this->drawLine(p1.x, p1.y, p2.x, p2.y, color);
}
void Game::drawRect(float left, float top, float right, float bottom, DWORD color, DWORD bgcolor) {
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
void Game::drawRect(float centerX, float centerY, float hw, float hh, float angle, DWORD color, DWORD bgcolor) {
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
void Game::drawArc(float x, float y, float r, float start, float end, DWORD color, DWORD bgcolor) {
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
void Game::drawPoly(b2PolygonShape* poly, b2Transform transform, b2Vec2 origin, float scale, DWORD color, DWORD bgcolor) {
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
void Game::drawText(float x, float y, char* text) {
    defaultFont->Render(x, y, HGETEXT_CENTER, text);
}

void Game::DrawPolygon (const b2Vec2 *vertices, int32 vertexCount, DWORD color, DWORD bgcolor) {
	b2Vec2 first( screenX(vertices[0].x), screenY(vertices[0].y) );
	b2Vec2 prev = first;
	whiteTriple.v[0].col = bgcolor; whiteTriple.v[1].col = bgcolor; whiteTriple.v[2].col = bgcolor;
	whiteTriple.v[0].x = prev.x;  whiteTriple.v[0].y = prev.y;
	for (int i = 1; i < vertexCount; i++) {
		b2Vec2 current( screenX(vertices[i].x), screenY(vertices[i].y) );
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
void Game::DrawPolygonScreen (const b2Vec2 *vertices, int32 vertexCount, DWORD color, DWORD bgcolor) {
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
void Game::DrawCircle (const b2Vec2 &center, float32 radius, DWORD color, DWORD bgcolor) {
	b2Vec2 screenCenter( screenX(center.x), screenY(center.y) );
	drawCircle(screenCenter.x, screenCenter.y, getPixelsPerMeter() * getScaleFactor() * radius, color, bgcolor);
}
void Game::DrawSegment (const b2Vec2 &p1, const b2Vec2 &p2, DWORD color) {
	b2Vec2 screenP1 = getPixelsPerMeter() * getScaleFactor() * (p1 - cameraPos);
	b2Vec2 screenP2 = getPixelsPerMeter() * getScaleFactor() * (p2 - cameraPos);
	hge->Gfx_RenderLine(screenP1.x, screenP1.y, screenP2.x, screenP2.y, color);
}
void Game::DrawTransform (const b2Transform &xf) {
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
void Game::drawCircle(float x, float y, float r, DWORD color) {
	drawArc(x, y, r, 0, M_PI * 2, color, 0);
}
void Game::drawCircle(float x, float y, float r, DWORD color, DWORD bgcolor) {
	drawArc(x, y, r, 0, M_PI * 2, color, bgcolor);
}


hgeAnimation* Game::loadAnimation(char * fn) {

    for ( int i = 0; i < animationsCount; i++ ) {
        if ( compareStrings(fn, animationNames[i]) ) {
//            printf("cloning anim #%d\n", i);
            return new hgeAnimation(*animations[i]);
        }
    }
    printf("loading animation %s ...\n", fn);


	TiXmlDocument doc(fn);
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
    	printf("load okay... ");
    	TiXmlElement* element = doc.FirstChildElement("animation");
    	printf("texture %s ", element->Attribute("texture"));
		HTEXTURE tex = loadTexture((char*)element->Attribute("texture"));
		int nframes = atoi(element->Attribute("nframes"));
		float fps = atof(element->Attribute("fps"));
		float x = atof(element->Attribute("x"));
		float y = atof(element->Attribute("y"));
		float w = atof(element->Attribute("w"));
		float h = atof(element->Attribute("h"));
		printf("creating animation... ");
		hgeAnimation* anim = new hgeAnimation(tex, nframes, fps, x, y, w, h);
		anim->SetHotSpot(w / 2, h / 2);
		//newObject->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_ZWRITE);
		//newObject->SetZ(0.5f);
		printf("done\n");
		animationNames[animationsCount] = copyString(fn);
		animations[animationsCount] = anim;
		animationsCount++;

		hgeAnimation* newAnim = new hgeAnimation(*anim);
        return newAnim;
    } else {
        char * error = strdup("Can't load animation: ");
        strcat(error, fn);
        Exception * e = new Exception(error);
        throw e;
    }
}
int Game::getAnimationsCount() {
    return animationsCount;
}
hgeAnimation* Game::getAnimation(int index) {
    return animations[index];
}
char* Game::getAnimationName(int index) {
    return animationNames[index];
}

HTEXTURE Game::loadTexture(char* fn) {
    for ( int i = 0; i < texturesCount; i++ ) {
        if ( compareStrings(fn, textureNames[i]) ) {
            printf("cloned\n");
            return textures[i];
        }
    }
    printf("loaded\n");
    HTEXTURE newTexture = hge->Texture_Load(fn);
    textureNames[texturesCount] = copyString(fn);
    textures[texturesCount] = newTexture;
    texturesCount++;
    return newTexture;
}

hgeAnimation* Game::cloneAnimation(hgeAnimation* source) {
    float x, y, w, h;
    source->GetTextureRect(&x, &y, &w, &h);
    hgeAnimation* newObject = new hgeAnimation( source->GetTexture(), source->GetFrames(), source->GetSpeed(), x, y, w, h );
    newObject->SetHotSpot(w / 2, h / 2);
//    printf("animation cloned\n");
    return newObject;
}

Character * Game::loadPlayerCharacter(char * fn, b2Vec2 origin) {
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
Character * Game::loadNonPlayerCharacter(char * fn, b2Vec2 origin) {
	printf("loading non player character %s ... \n", fn);
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

//b2Fixture* Game::loadFixture(b2Body* body, TiXmlElement* elem) {
//	printf("  %s shape ", elem->Attribute("type"));
//	if (strcmp(elem->Attribute("type"), "polygon") == 0) {
//		TiXmlElement* vertexElem = elem->FirstChildElement("vertex");
//		int k = 0;
//		while (vertexElem) {
//			k++;
//			vertexElem = vertexElem->NextSiblingElement("vertex");
//		}
//		b2Vec2 vertices[k];
//		vertexElem = elem->FirstChildElement("vertex");
//		k = 0;
//		while (vertexElem) {
//
//			float x = atof(vertexElem->Attribute("x"));
//			float y = atof(vertexElem->Attribute("y"));
//			printf("vertex %.2f %.2f ", x, y);
//			vertices[k].Set(x, y);
//			k++;
//			vertexElem = vertexElem->NextSiblingElement("vertex");
//		}
//		b2PolygonShape polygon;
//		polygon.Set(vertices, k);
//		b2FixtureDef fixtureDef;
//		fixtureDef.shape = &polygon;
//		fixtureDef.density = 1.0f;
//		fixtureDef.friction = 100.0f;
//		return body->CreateFixture(&fixtureDef);
//	}
//	if (strcmp(elem->Attribute("type"), "box") == 0) {
//		float hx = atof(elem->Attribute("hx"));
//		float hy = atof(elem->Attribute("hy"));
//		printf("with width %.2f and height %.2f ", hx, hy);
//		b2PolygonShape dynamicBox;
//		dynamicBox.SetAsBox(hx, hy);
//		b2FixtureDef fixtureDef;
//		fixtureDef.shape = &dynamicBox;
//		fixtureDef.density = 1.0f;
//		fixtureDef.friction = 1.0f;
//		printf("added\n");
//		return body->CreateFixture(&fixtureDef);
//	}
//}

void Game::addGroundLine(GroundLine* newGL) {
	groundLines[groundLinesCount] = newGL;
	groundLinesCount++;
}
void Game::addMapAnimation(MapAnimation* newMA) {
	mapAnimations[mapAnimationsCount] = newMA;
	mapAnimationsCount++;
}
void Game::addPlatform(Platform* newPlatform) {
	platforms[platformsCount] = newPlatform;
	platformsCount++;
}
void Game::addEffect(Effect* newEffect, int listIndex) {
    newEffect->initialize();
	effects[listIndex][ effectsCount[listIndex] ] = newEffect;
	effectsCount[listIndex]++;
}

void Game::removeEffect(int index, int listIndex) {
    if (index == effectsCount[listIndex] - 1) {
        delete effects[listIndex][index];
        //effects[index] = NULL;
        effectsCount[listIndex]--;
    } else {
        delete effects[listIndex][index];
        effects[listIndex][index] = effects[listIndex][effectsCount[listIndex] - 1];
        //effects[effectsCount - 1] = NULL;
        effectsCount[listIndex]--;
    }
}

int Game::getCharactersCount() {
	return charactersCount;
}
Character* Game::getCharacter(int index) {
    if (index > -1 && index < charactersCount) {
        return characters[index];
    }
    else {
		return NULL;
	}
}

HGE* Game::getHge() {
	return hge;
}
b2World* Game::getWorld() {
	return world;
}

hgeGUI* Game::getGUI() {
	return gui;
}

int Game::getGroundLinesCount() {
	return groundLinesCount;
}
GroundLine* Game::getGroundLine(int index) {
	if (index > -1 && index < groundLinesCount) {
		return groundLines[index];
	} else {
		return NULL;
	}
}

int Game::getMapAnimationsCount() {
	return mapAnimationsCount;
}
MapAnimation* Game::getMapAnimation(int index) {
	if (index > -1 && index < mapAnimationsCount) {
		return mapAnimations[index];
	} else {
		return NULL;
	}
}

bool Game::loadEffectPrototypes(char* fileName) {
	printf("loading effect prototypes %s ... \n", fileName);
    TiXmlDocument doc(fileName);
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
    	TiXmlElement* root = doc.FirstChildElement("effect_prototypes");

		effectPrototypesCount = atoi(root->Attribute("count"));
		effectPrototypes = new EffectPrototype*[effectPrototypesCount];

        TiXmlElement* element = root->FirstChildElement("prototype");
        int i = 0;
        while (element) {
			EffectPrototype* newObject = new EffectPrototype(this);
            newObject->loadFromXml(element);
            effectPrototypes[i] = newObject;

			i++;
            element = element->NextSiblingElement("prototype");
        }
        effectPrototypesCount = i;
		return true;
    } else {
        printf("failed\n");
        return false;
    }

}
int Game::getEffectPrototypesCount() {
	return effectPrototypesCount;
}
EffectPrototype* Game::getEffectPrototype(int index) {
	if (index > -1 && index < effectPrototypesCount) {
		return effectPrototypes[index];
	} else {
		return NULL;
	}
}
void Game::addEffectPrototype() {
    EffectPrototype** _effectPrototypes = new EffectPrototype*[effectPrototypesCount + 1];
    for (int i = 0; i < effectPrototypesCount; i++) {
        _effectPrototypes[i] = effectPrototypes[i];
    }
    _effectPrototypes[effectPrototypesCount] = new EffectPrototype(this);

    delete effectPrototypes;
    effectPrototypes = _effectPrototypes;
    effectPrototypesCount++;
}
void Game::removeEffectPrototype(int index) {
    if (index > -1 && index < effectPrototypesCount) {
        delete effectPrototypes[index];
        for (int i = index; i < effectPrototypesCount - 1; i++) {
            effectPrototypes[i] = effectPrototypes[i + 1];
        }
        effectPrototypesCount--;
    }
}

/// condition prototypes
//{
bool Game::loadConditionPrototypes(char* fileName) {
	printf("loading condition prototypes %s ... \n", fileName);
    TiXmlDocument doc(fileName);
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
    	TiXmlElement* root = doc.FirstChildElement("condition_prototypes");

		conditionPrototypesCount = atoi(root->Attribute("count"));
		conditionPrototypes = new ConditionPrototype*[conditionPrototypesCount];

        TiXmlElement* element = root->FirstChildElement("prototype");
        int i = 0;
        while (element) {
			ConditionPrototype* newObject = new ConditionPrototype(this);
            newObject->loadFromXml(element);
            conditionPrototypes[i] = newObject;

			i++;
            element = element->NextSiblingElement("prototype");
        }
        conditionPrototypesCount = i;
		return true;
    } else {
        printf("failed\n");
        return false;
    }
}
int Game::getConditionPrototypesCount() {
	return conditionPrototypesCount;
}
ConditionPrototype* Game::getConditionPrototype(int index) {
	if (index > -1 && index < conditionPrototypesCount) {
		return conditionPrototypes[index];
	} else {
		return NULL;
	}
}
void Game::addConditionPrototype() {
    ConditionPrototype** _conditionPrototypes = new ConditionPrototype*[conditionPrototypesCount + 1];
    for (int i = 0; i < conditionPrototypesCount; i++) {
        _conditionPrototypes[i] = conditionPrototypes[i];
    }
    _conditionPrototypes[conditionPrototypesCount] = new ConditionPrototype(this);

    delete conditionPrototypes;
    conditionPrototypes = _conditionPrototypes;
    conditionPrototypesCount++;
}
void Game::removeConditionPrototype(int index) {
    if (index > -1 && index < conditionPrototypesCount) {
        delete conditionPrototypes[index];
        for (int i = index; i < conditionPrototypesCount - 1; i++) {
            conditionPrototypes[i] = conditionPrototypes[i + 1];
        }
        conditionPrototypesCount--;
    }
}
//}

/// character param prototypes
//{
bool Game::loadCharacterParamPrototypes(char* fileName) {
	printf("loading character param prototypes %s ... \n", fileName);
    TiXmlDocument doc(fileName);
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
    	TiXmlElement* root = doc.FirstChildElement("character_param_prototypes");

		characterParamPrototypesCount = atoi(root->Attribute("count"));
		characterParamPrototypes = new CharacterParamPrototype*[characterParamPrototypesCount];

        TiXmlElement* element = root->FirstChildElement("prototype");
        int i = 0;
        while (element) {
			CharacterParamPrototype* newObject = new CharacterParamPrototype();
            newObject->loadFromXml(element);
            characterParamPrototypes[i] = newObject;

			i++;
            element = element->NextSiblingElement("prototype");
        }
        characterParamPrototypesCount = i;
		return true;
    } else {
        printf("failed\n");
        return false;
    }
}
int Game::getCharacterParamPrototypesCount() {
	return characterParamPrototypesCount;
}
CharacterParamPrototype* Game::getCharacterParamPrototype(int index) {
	if (index > -1 && index < characterParamPrototypesCount) {
		return characterParamPrototypes[index];
	} else {
		return NULL;
	}
}
void Game::addCharacterParamPrototype() {
    CharacterParamPrototype** _characterParamPrototypes = new CharacterParamPrototype*[characterParamPrototypesCount + 1];
    for (int i = 0; i < characterParamPrototypesCount; i++) {
        _characterParamPrototypes[i] = characterParamPrototypes[i];
    }
    _characterParamPrototypes[characterParamPrototypesCount] = new CharacterParamPrototype();

    delete characterParamPrototypes;
    characterParamPrototypes = _characterParamPrototypes;
    characterParamPrototypesCount++;
}
void Game::removeCharacterParamPrototype(int index) {
    if (index > -1 && index < characterParamPrototypesCount) {
        delete characterParamPrototypes[index];
        for (int i = index; i < characterParamPrototypesCount - 1; i++) {
            characterParamPrototypes[i] = characterParamPrototypes[i + 1];
        }
        characterParamPrototypesCount--;
    }
}

//}
/// character resource prototypes
//{
bool Game::loadCharacterResourcePrototypes(char* fileName) {
	printf("loading character resource prototypes %s ... \n", fileName);
    TiXmlDocument doc(fileName);
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
    	TiXmlElement* root = doc.FirstChildElement("character_resource_prototypes");

		characterResourcePrototypesCount = atoi(root->Attribute("count"));
		characterResourcePrototypes = new CharacterResourcePrototype*[characterResourcePrototypesCount];

        TiXmlElement* element = root->FirstChildElement("prototype");
        int i = 0;
        while (element) {
			CharacterResourcePrototype* newObject = new CharacterResourcePrototype();
            newObject->loadFromXml(element);
            characterResourcePrototypes[i] = newObject;

			i++;
            element = element->NextSiblingElement("prototype");
        }
        characterResourcePrototypesCount = i;
		return true;
    } else {
        printf("failed\n");
        return false;
    }
}
int Game::getCharacterResourcePrototypesCount() {
	return characterResourcePrototypesCount;
}
CharacterResourcePrototype* Game::getCharacterResourcePrototype(int index) {
	if (index > -1 && index < characterResourcePrototypesCount) {
		return characterResourcePrototypes[index];
	} else {
		return NULL;
	}
}
void Game::addCharacterResourcePrototype() {
    CharacterResourcePrototype** _characterResourcePrototypes = new CharacterResourcePrototype*[characterResourcePrototypesCount + 1];
    for (int i = 0; i < characterResourcePrototypesCount; i++) {
        _characterResourcePrototypes[i] = characterResourcePrototypes[i];
    }
    _characterResourcePrototypes[characterResourcePrototypesCount] = new CharacterResourcePrototype();

    delete characterResourcePrototypes;
    characterResourcePrototypes = _characterResourcePrototypes;
    characterResourcePrototypesCount++;
}
void Game::removeCharacterResourcePrototype(int index) {
    if (index > -1 && index < characterResourcePrototypesCount) {
        delete characterResourcePrototypes[index];
        for (int i = index; i < characterResourcePrototypesCount - 1; i++) {
            characterResourcePrototypes[i] = characterResourcePrototypes[i + 1];
        }
        characterResourcePrototypesCount--;
    }
}

//}
/// character status prototypes
//{
bool Game::loadCharacterStatusPrototypes(char* fileName) {
	printf("loading character status prototypes %s ... ", fileName);
    TiXmlDocument doc(fileName);
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
    	TiXmlElement* root = doc.FirstChildElement("character_status_prototypes");

		characterStatusPrototypesCount = atoi(root->Attribute("count"));
		characterStatusPrototypes = new CharacterStatusPrototype*[characterStatusPrototypesCount];

        TiXmlElement* element = root->FirstChildElement("prototype");
        int i = 0;
        while (element) {
			CharacterStatusPrototype* newObject = new CharacterStatusPrototype();
            newObject->loadFromXml(element);
            characterStatusPrototypes[i] = newObject;

			i++;
            element = element->NextSiblingElement("prototype");
        }
        characterStatusPrototypesCount = i;
        printf("%i of them\n", characterStatusPrototypesCount);
		return true;
    } else {
        printf("failed\n");
        return false;
    }
}
int Game::getCharacterStatusPrototypesCount() {
	return characterStatusPrototypesCount;
}
CharacterStatusPrototype* Game::getCharacterStatusPrototype(int index) {
	if (index > -1 && index < characterStatusPrototypesCount) {
		return characterStatusPrototypes[index];
	} else {
		return NULL;
	}
}
void Game::addCharacterStatusPrototype() {
    CharacterStatusPrototype** _characterStatusPrototypes = new CharacterStatusPrototype*[characterStatusPrototypesCount + 1];
    for (int i = 0; i < characterStatusPrototypesCount; i++) {
        _characterStatusPrototypes[i] = characterStatusPrototypes[i];
    }
    _characterStatusPrototypes[characterStatusPrototypesCount] = new CharacterStatusPrototype();

    delete characterStatusPrototypes;
    characterStatusPrototypes = _characterStatusPrototypes;
    characterStatusPrototypesCount++;
}
void Game::removeCharacterStatusPrototype(int index) {
    if (index > -1 && index < characterStatusPrototypesCount) {
        delete characterStatusPrototypes[index];
        for (int i = index; i < characterStatusPrototypesCount - 1; i++) {
            characterStatusPrototypes[i] = characterStatusPrototypes[i + 1];
        }
        characterStatusPrototypesCount--;
    }
}

//}
/// character move types
//{
bool Game::loadCharacterMoveTypes(char* fileName) {
	printf("loading character move types %s ... \n", fileName);
    TiXmlDocument doc(fileName);
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
    	TiXmlElement* root = doc.FirstChildElement("character_move_types");

		characterMoveTypesCount = atoi(root->Attribute("count"));
		characterMoveTypes = new CharacterMoveType*[characterMoveTypesCount];

        TiXmlElement* element = root->FirstChildElement("prototype");
        int i = 0;
        while (element) {
			CharacterMoveType* newObject = new CharacterMoveType();
            newObject->loadFromXml(element);
            characterMoveTypes[i] = newObject;

			i++;
            element = element->NextSiblingElement("prototype");
        }
        characterMoveTypesCount = i;
		return true;
    } else {
        printf("failed\n");
        return false;
    }
}
int Game::getCharacterMoveTypesCount() {
	return characterMoveTypesCount;
}
CharacterMoveType* Game::getCharacterMoveType(int index) {
	if (index > -1 && index < characterMoveTypesCount) {
		return characterMoveTypes[index];
	} else {
		return NULL;
	}
}
void Game::addCharacterMoveType() {
    CharacterMoveType** _characterMoveTypes = new CharacterMoveType*[characterMoveTypesCount + 1];
    for (int i = 0; i < characterMoveTypesCount; i++) {
        _characterMoveTypes[i] = characterMoveTypes[i];
    }
    _characterMoveTypes[characterMoveTypesCount] = new CharacterMoveType();

    delete characterMoveTypes;
    characterMoveTypes = _characterMoveTypes;
    characterMoveTypesCount++;
}
void Game::removeCharacterMoveType(int index) {
    if (index > -1 && index < characterMoveTypesCount) {
        delete characterMoveTypes[index];
        for (int i = index; i < characterMoveTypesCount - 1; i++) {
            characterMoveTypes[i] = characterMoveTypes[i + 1];
        }
        characterMoveTypesCount--;
    }
}

//}

float Game::worldX(float screenX, float ratio) {
    return (screenX - getScreenWidth() * 0.5f) / (pixelsPerMeter * scaleFactor) + cameraPos.x * ratio;
}
float Game::worldY(float screenY, float ratio) {
    return (screenY - getScreenHeight() * 0.5f) / (pixelsPerMeter * scaleFactor) + cameraPos.y * ratio;
}

float Game::worldX(float screenX) {
    return (screenX - getScreenWidth() * 0.5f) / (pixelsPerMeter * scaleFactor) + cameraPos.x;
}
float Game::worldY(float screenY) {
    return (screenY - getScreenHeight() * 0.5f) / (pixelsPerMeter * scaleFactor) + cameraPos.y;
}

float Game::screenX(float worldX) {
    return floorf( (worldX - cameraPos.x) * (pixelsPerMeter * scaleFactor) + getScreenWidth() * 0.5f );
}
float Game::screenY(float worldY) {
    return floorf( (worldY - cameraPos.y) * (pixelsPerMeter * scaleFactor) + getScreenHeight() * 0.5f );
}

float Game::screenX(float worldX, float ratio) {
    return floorf( (worldX - cameraPos.x * ratio) * (pixelsPerMeter * scaleFactor) + getScreenWidth() * 0.5f );
}
float Game::screenY(float worldY, float ratio) {
    return floorf( (worldY - cameraPos.y * ratio) * (pixelsPerMeter * scaleFactor) + getScreenHeight() * 0.5f );
}
//b2Vec2 Game::screenPos(b2Vec2 worldPos) {
//    return b2Vec2(this->screenX(worldPos.x), this->screenY(worldPos.y));
//}

float Game::getScaleFactor() {
    return scaleFactor;
}
float Game::getPixelsPerMeter() {
    return pixelsPerMeter;
}
float Game::getFullScale() {
    return scaleFactor * pixelsPerMeter;
}

void Game::setScale(float scale) {
	scaleFactor = scale;
	viewportWidth = screenWidth / (pixelsPerMeter * scaleFactor);
	viewportHeight = screenHeight / (pixelsPerMeter * scaleFactor);
	halfViewportWidth = viewportWidth * 0.5f;
	halfViewportHeight = viewportHeight * 0.5f;
}
void Game::moveCamera(b2Vec2 diff) {
	cameraPos += diff;
}
void Game::setCamera(b2Vec2 pos) {
	cameraPos = pos;
}
b2Vec2 Game::getCameraPos() {
	return cameraPos;
}
float Game::getWorldMouseX() {
	return worldMouseX;
}
float Game::getWorldMouseY() {
	return worldMouseY;
}
void Game::moveScreen(b2Vec2 diff) {
	float scale = 1 / (scaleFactor * pixelsPerMeter);
	diff *= scale;
	cameraPos += diff;
}

b2Vec2 Game::getGravity() {
	return gravity;
}

float Game::getTimeStep() {
	return timeStep;
}

int Game::getScreenWidth() {
	return screenWidth;
}
int Game::getScreenHeight() {
	return screenHeight;
}
float Game::getViewportWidth() {
    return viewportWidth;
}
float Game::getViewportHeight() {
    return viewportHeight;
}
float Game::getHalfViewportWidth() {
    return halfViewportWidth;
}
float Game::getHalfViewportHeight() {
    return halfViewportHeight;
}

float Game::getMapWidth() {
	return mapWidth;
}
float Game::getMapHeight() {
	return mapHeight;
}
float Game::getHalfMapWidth() {
	return halfMapWidth;
}
float Game::getHalfMapHeight() {
	return halfMapHeight;
}


void Game::loadMap(char* fn) {
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
		halfMapWidth = mapWidth * 0.5f; halfMapHeight = mapHeight * 0.5f;
		if (root->Attribute("bgcolor")) {
			bgcolor = atoi(root->Attribute("bgcolor"));
		}

		mapAnimationsCount = atoi(root->Attribute("animations"));
		mapAnimations = new MapAnimation*[mapAnimationsCount];
		groundLinesCount = atoi(root->Attribute("ground_lines"));
		groundLines = new GroundLine*[groundLinesCount];
		platformsCount = atoi(root->Attribute("platforms"));
		platforms = new Platform*[platformsCount];


        int layersCount = atoi( root->Attribute("layers") );
        int orders[layersCount];
        float ratios[layersCount];

        int layer = 0;
        TiXmlElement* element = root->FirstChildElement("layer");
        while (element) {
            orders[layer] = atoi( element->Attribute("order") );
            ratios[layer] = atof( element->Attribute("ratio") );
            layer++;

            element = element->NextSiblingElement("layer");
        }
        layersCount = layer;

        int minOrder = -1;
        int maxOrder = -100;
        for (int i = 0; i < layersCount; i++) {
            if ( orders[i] < minOrder )
                minOrder = orders[i];
            if ( orders[i] > maxOrder && orders[i] <= 0 )
                maxOrder = orders[i];
        }
        backLayersCount = 0;
        int backLayerIndices[layersCount];
        backLayerRatios = new float[layersCount];
        for (int order = minOrder; order <= maxOrder; order++) {
            for (int i = 0; i < layersCount; i++) {
                if (order == orders[i]) {
                    backLayerRatios[backLayersCount] = ratios[i];
                    backLayerIndices[backLayersCount] = i;
                    backLayersCount++;
                }
            }
        }

        minOrder = 100;
        maxOrder = 0;
        for (int i = 0; i < layersCount; i++) {
            if ( orders[i] < minOrder && orders[i] > 0 )
                minOrder = orders[i];
            if ( orders[i] > maxOrder )
                maxOrder = orders[i];
        }
        frontLayersCount = 0;
        int frontLayerIndices[layersCount];
        frontLayerRatios = new float[layersCount];
        for (int order = minOrder; order <= maxOrder; order++) {
            for (int i = 0; i < layersCount; i++) {
                if (order == orders[i]) {
                    frontLayerRatios[frontLayersCount] = ratios[i];
                    frontLayerIndices[frontLayersCount] = i;
                    frontLayersCount++;
                }
            }
        }

        backLayersMapAnimationsCounts = new int[backLayersCount];
        frontLayersMapAnimationsCounts = new int[frontLayersCount];
        backLayersMapAnimations = new int*[backLayersCount];
        frontLayersMapAnimations = new int*[frontLayersCount];
        for (int i = 0; i < backLayersCount; i++) {
            backLayersMapAnimations[i] = new int[mapAnimationsCount];
            backLayersMapAnimationsCounts[i] = 0;
        }
        for (int i = 0; i < frontLayersCount; i++) {
            frontLayersMapAnimations[i] = new int[mapAnimationsCount];
            frontLayersMapAnimationsCounts[i] = 0;
        }


        element = root->FirstChildElement("animation");
        int i = 0;
        while (element) {
        	printf("loading animation...\n");

            mapAnimations[i] = new MapAnimation(this);
            mapAnimations[i]->loadFromXml(element);

            int layer = atoi(element->Attribute("layer"));

            for (int j = 0; j < backLayersCount; j++) {
                if (backLayerIndices[j] == layer) {
                    backLayersMapAnimations[j][ backLayersMapAnimationsCounts[j] ] = i;
                    backLayersMapAnimationsCounts[j]++;
                }
            }
            for (int j = 0; j < frontLayersCount; j++) {
                if (frontLayerIndices[j] == layer) {
                    frontLayersMapAnimations[j][ frontLayersMapAnimationsCounts[j] ] = i;
                    frontLayersMapAnimationsCounts[j]++;
                }
            }

			i++;
            element = element->NextSiblingElement("animation");
        }
        mapAnimationsCount = i;


        i = 0;
        element = root->FirstChildElement("ground_line");
        while (element) {
        	printf("loading ground line...\n");
			groundLines[i] = new GroundLine(this, atof(element->Attribute("x1")), atof(element->Attribute("y1")), atof(element->Attribute("x2")), atof(element->Attribute("y2")));

			i++;
            element = element->NextSiblingElement("ground_line");
        }
        groundLinesCount = i;

        i = 0;
        element = root->FirstChildElement("platform");
        while (element) {
        	printf("loading platform...\n");
        	platforms[i] = new Platform(this, element);

			i++;
            element = element->NextSiblingElement("platform");
        }
        platformsCount = i;
    } else {
        printf("failed\n");
    }

}





void Game::updateCounter(int index, float value) {
    counters[index] += value;
}

Game* Game::instance = NULL;
Game* Game::getInstance() {
    return Game::instance;
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

float distanceToSegment(float x1, float y1, float x2, float y2, float pointX, float pointY) {
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

float frand(float from, float to) {
    float r = Game::getInstance()->getHge()->Random_Float(from, to);
//    printf("%.3f - %.3f: %.3f\n", from, to, r);
    return r;
    return from + (rand()) /( static_cast <float> (RAND_MAX / (to-from)));
}

char* copyString(const char* original) {
    char* newString = new char[256];
    int i = 0;
    while (i < 255 && original[i] != '\0') {
        newString[i] = original[i];
        i++;
    }
    if (original[i - 1] == ' ') {
        i--;
    }
    newString[i] = '\0';

    return newString;
}

bool compareStrings(char* first, char* second) {
    int i = 0;
    while (i < 256) {
        if ( first[i] != second[i] )
            return false;
        if ( first[i] == '\0' )
            return true;
        i++;
    }
    return true;
}

int countSpaces(const char* str) {
    int i = 0;
    while (i < 256) {
        if (str[i] != ' ')
            return i;
        i++;
    }
    return i;
}

char* getFileName(const char* path) {
    char* fileName = new char[256];
    int i = 0;
    int start = 0; int finish = 255;
    while (i < 255 && path[i] != '\0') {
        if (path[i] == '\\' || path[i] == '/') {
            start = i + 1;
        }
        if (path[i] == '.') {
            finish = i;
        }
        i++;
    }

    for (i = 0; i < finish - start; i++) {
        fileName[i] = path[start + i];
    }
    fileName[i] = '\0';

    return fileName;
}

