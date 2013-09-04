#include <stdio.h>
#include <hge.h>
#include <hgefont.h>
#include <hgeanim.h>
#include <Box2D/Box2D.h>
#include <SoDlib.h>
#include <windows.h>
#include <list>

using namespace std;

Game* game;

GUIWindow* mainWindow;
GUIWindow* animsWindow;

hgeFont* fnt;
hgeAnimation* disabledIcon;

hgeAnimation* beingInsertedAnim;
char* beingInsertedAnimName;

const int MODE_DEFAULT     = 0;
const int MODE_SELECT_ANIM = 1;
const int MODE_INSERT_ANIM = 2;
const int MODE_CAMERA_MOVE = 3;
const int MODE_ANIM_DRAG   = 4;
const int MODE_ANIM_ROTATE = 5;
const int MODE_INSERT_GL_STEP1   = 6;
const int MODE_INSERT_GL_STEP2   = 7;

int mode = MODE_DEFAULT;

float width = 10; float height = 10;
int animationsCount = 0; char** animationNames = new char*[256]; hgeAnimation** animations = new hgeAnimation*[256];
float* animationX = new float[256]; float* animationY = new float[256]; float* animationAngle = new float[256];
int groundLinesCount = 0; GroundLine** groundLines = new GroundLine*[256];

float dragOffsetX; float dragOffsetY; float dragOffsetAngle;
int selectedAnim;

void resetMode()
{
	mode = MODE_DEFAULT;
	mainWindow->Show();
	animsWindow->Hide();
	selectedAnim = -1;
}

int getPointedAnim(float x, float y)
{
	int selected = -1;
	for (int i = 0; i < animationsCount; i++) {
        hgeRect* bb = new hgeRect();
        animations[i]->GetBoundingBoxEx(
            game->screenX(animationX[i]),
            game->screenY(animationY[i]),
            animationAngle[i],
            game->getScaleFactor(),
            game->getScaleFactor(),
            bb
        );

        if (bb->TestPoint(x, y))
            selected = i;
	}

	return selected;
}

bool saveMap(char* fn)
{
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );
	TiXmlElement* root = new TiXmlElement( "map" );
	root->SetAttribute("animations", animationsCount);
	root->SetAttribute("ground_lines", groundLinesCount);

	for (int i = 0; i < animationsCount; i++) {
        TiXmlElement* element = new TiXmlElement( "animation" );
        root->LinkEndChild( element );
        element->SetAttribute("file", animationNames[i]);
        element->SetDoubleAttribute("x", animationX[i]);
        element->SetDoubleAttribute("y", animationY[i]);
        element->SetDoubleAttribute("angle", animationAngle[i]);
	}
	for (int i = 0; i < groundLinesCount; i++) {
        TiXmlElement* element = new TiXmlElement( "ground_line" );
        root->LinkEndChild( element );
        element->SetDoubleAttribute("x1", groundLines[i]->getStartPoint().x);
        element->SetDoubleAttribute("y1", groundLines[i]->getStartPoint().y);
        element->SetDoubleAttribute("x2", groundLines[i]->getEndPoint().x);
        element->SetDoubleAttribute("y2", groundLines[i]->getEndPoint().y);
	}

	doc.LinkEndChild( root );
	doc.SaveFile(fn);
}

bool loadMap(char* fn)
{
	animations = new hgeAnimation*[256];

	animationsCount = 0;

	animationX = new float[256];
	animationY = new float[256];
	animationAngle = new float[256];
	animationNames = new char*[256];


	printf("loading map %s ... \n", fn);
    TiXmlDocument doc(fn);
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
    	TiXmlElement* root = doc.FirstChildElement("map");

        if (root->Attribute("width")) {
			width = atof(root->Attribute("width"));
		}
    	if (root->Attribute("height")) {
			height = atof(root->Attribute("height"));
		}

        TiXmlElement* element = root->FirstChildElement("animation");
        int i = 0;
        while (element) {
        	printf("loading animation...\n");
        	float x = atof(element->Attribute("x"));
			float y = atof(element->Attribute("y"));
			float angle = atof(element->Attribute("angle"));

			char* animationName = (char*)element->Attribute("file");
			hgeAnimation* animation = game->loadAnimation(animationName);
			//animation->SetHotSpot(-game->getPixelsPerMeter() * atof(element->Attribute("x")), -game->getPixelsPerMeter() * atof(element->Attribute("y")));

			animations[i] = animation;
			animationX[i] = x;
			animationY[i] = y;
			animationAngle[i] = angle;

			animationNames[i] = new char[64];
			int c = 0;
			for (c = 0; animationName[c] != '\0'; c++) {
			    animationNames[i][c] = animationName[c];
			}
			animationNames[i][c] = '\0';

			i++;
            element = element->NextSiblingElement("animation");
        }
        animationsCount = i;

        i = 0;
        element = root->FirstChildElement("ground_line");
        while (element) {
        	printf("loading ground line...\n");
			groundLines[i] = new GroundLine(game, atof(element->Attribute("x1")), atof(element->Attribute("y1")), atof(element->Attribute("x2")), atof(element->Attribute("y2")));

			i++;
            element = element->NextSiblingElement("ground_line");
        }
        groundLinesCount = i;
    } else {
        printf("failed\n");
    }

}


bool saveMapButtonClick(hgeGUIMenuItem* sender)
{
	saveMap("map.xml");
}

bool loadMapButtonClick(hgeGUIMenuItem* sender)
{
	loadMap("map.xml");
}



bool insertAnimButtonClick(hgeGUIMenuItem* sender)
{
	mode = MODE_SELECT_ANIM;
	//mainWindow->Hide();
	animsWindow->Show();
}
bool animButtonClick(hgeGUIMenuItem* sender)
{
	mode = MODE_INSERT_ANIM;
	//mainWindow->Show();
	animsWindow->Hide();
	beingInsertedAnimName = sender->getTitle();
	beingInsertedAnim = game->loadAnimation(beingInsertedAnimName);
	//game->getHge()->Input_SetMousePos(1450, 600);
}
bool insertGroundLineButtonClick(hgeGUIMenuItem* sender)
{
	mode = MODE_INSERT_GL_STEP1;
}

bool FrameFunc()
{
    float x, y;
	game->getHge()->Input_GetMousePos(&x, &y);
	float worldX = game->worldX(x);
	float worldY = game->worldY(y);

	if (game->getHge()->Input_GetMouseWheel() > 0) {
		game->setScale(game->getScaleFactor() * 2);
		game->setCamera(b2Vec2(worldX - game->getWorldScreenWidth() * 0.5f, worldY - game->getWorldScreenHeight() * 0.5f));
	}
	if (game->getHge()->Input_GetMouseWheel() < 0) {
		game->setScale(game->getScaleFactor() * 0.5);
		game->setCamera(b2Vec2(worldX - game->getWorldScreenWidth() * 0.5f, worldY - game->getWorldScreenHeight() * 0.5f));
	}

	if (game->getHge()->Input_GetKeyState(HGEK_RIGHT)) {
        game->moveScreen(b2Vec2(10, 0));
	}
	if (game->getHge()->Input_GetKeyState(HGEK_LEFT)) {
        game->moveScreen(b2Vec2(-10, 0));
	}
	if (game->getHge()->Input_GetKeyState(HGEK_DOWN)) {
        game->moveScreen(b2Vec2(0, 10));
	}
	if (game->getHge()->Input_GetKeyState(HGEK_UP)) {
        game->moveScreen(b2Vec2(0, -10));
	}

	switch(mode) {
		case MODE_DEFAULT:
		    if (y < 1300 && game->getHge()->Input_KeyDown(HGEK_LBUTTON)) {
                selectedAnim = getPointedAnim(x, y);
                if (selectedAnim != -1) {
                    dragOffsetX = worldX - animationX[selectedAnim];
                    dragOffsetY = worldY - animationY[selectedAnim];
                    mode = MODE_ANIM_DRAG;
                }
		    }
		    if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && game->getHge()->Input_GetKeyState(HGEK_SHIFT)) {
				selectedAnim = getPointedAnim(x, y);
				if (selectedAnim != -1) {
					//mainWindow->Hide();
					dragOffsetX = worldX;
					dragOffsetY = worldY;
//					selectedBodyX = game->screenX(animX(selectedBody));
//					selectedBodyY = game->screenY(animY(selectedBody));
					mode = MODE_ANIM_ROTATE;
				}
			}
			if (game->getHge()->Input_KeyDown(HGEK_RBUTTON)) {
                dragOffsetX = x;
                dragOffsetY = y;
                mode = MODE_CAMERA_MOVE;
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
		case MODE_INSERT_ANIM:
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)
                && worldX > 0 && worldX < width && worldY > 0 && worldY < height) {
				animationX[animationsCount] = worldX;
				animationY[animationsCount] = worldY;
				animationAngle[animationsCount] = 0;

				animationNames[animationsCount] = beingInsertedAnimName;
				animations[animationsCount] = beingInsertedAnim;

				animationsCount++;
				resetMode();
			}
			break;
        case MODE_ANIM_DRAG:
			if (selectedAnim != -1) {
				float currentX = worldX - dragOffsetX;
				float currentY = worldY - dragOffsetY;
				if (currentX > 0 && currentX < width && currentY > 0 && currentY < height) {
                    animationX[selectedAnim] = currentX;
                    animationY[selectedAnim] = currentY;
				}

				if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
					resetMode();
				}
			}
            break;
        case MODE_ANIM_ROTATE:
			if (selectedAnim != -1) {
				if (sqrt(pow(dragOffsetX - worldX, 2) + pow(dragOffsetY - worldY, 2)) < 0.1f) {
					dragOffsetAngle = atan2(worldY - dragOffsetY, worldX - dragOffsetX) - animationAngle[selectedAnim];
				} else {
					float angle = atan2(worldY - dragOffsetY, worldX - dragOffsetX) - dragOffsetAngle;
					float offsetAngle = atan2(
						dragOffsetY - animationY[selectedAnim],
						dragOffsetX - animationX[selectedAnim]
					);
					float len = sqrt(
						pow(dragOffsetX - animationX[selectedAnim], 2) +
						pow(dragOffsetY - animationY[selectedAnim], 2)
					);

					float newOffsetX = animationX[selectedAnim] + cos(angle - animationAngle[selectedAnim] + offsetAngle) * len;
					float newOffsetY = animationY[selectedAnim] + sin(angle - animationAngle[selectedAnim] + offsetAngle) * len;

					float transX = newOffsetX - dragOffsetX;
					float transY = newOffsetY - dragOffsetY;
					//printf("%f %f %f\n", len, transX, transY);

                    animationX[selectedAnim] = animationX[selectedAnim] - transX;
                    animationY[selectedAnim] = animationY[selectedAnim] - transY;
					animationAngle[selectedAnim] = angle;
				}
			}
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON) || game->getHge()->Input_KeyUp(HGEK_SHIFT)) {
				resetMode();
			}
			break;
        case MODE_INSERT_GL_STEP1:
            if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)
                && worldX > 0 && worldX < width && worldY > 0 && worldY < height) {
				dragOffsetX = worldX;
				dragOffsetY = worldY;
				mode = MODE_INSERT_GL_STEP2;
			}
            break;
        case MODE_INSERT_GL_STEP2:
            if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)
                && worldX > 0 && worldX < width && worldY > 0 && worldY < height) {
				groundLines[groundLinesCount] = new GroundLine(game, dragOffsetX, dragOffsetY, worldX, worldY);
				groundLinesCount++;
				dragOffsetX = worldX;
				dragOffsetY = worldY;
			}
			if (game->getHge()->Input_KeyUp(HGEK_RBUTTON)
                && worldX > 0 && worldX < width && worldY > 0 && worldY < height) {
//				groundLines[groundLinesCount] = new GroundLine(game, dragOffsetX, dragOffsetY, worldX, worldY);
//				groundLinesCount++;
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
    float x, y;
    game->getHge()->Input_GetMousePos(&x, &y);
    float worldX = game->worldX(x);
	float worldY = game->worldY(y);
	float insertX = x; float insertY = y;
	if (insertX > 1300) insertX = 1300;

	// RenderFunc should always return false
	game->startDraw();

	for (int i = 0; i < animationsCount; i++) {
        animations[i]->RenderEx(
            game->screenX(animationX[i]),
            game->screenY(animationY[i]),
            animationAngle[i],
            game->getScaleFactor(),
            game->getScaleFactor()
        );

        hgeRect* bb = new hgeRect();
        animations[i]->GetBoundingBoxEx(
            game->screenX(animationX[i]),
            game->screenY(animationY[i]),
            animationAngle[i],
            game->getScaleFactor(),
            game->getScaleFactor(),
            bb
        );

        DWORD color = 0xFFAA0000;
        if (i == selectedAnim) {
            color = 0xFFFF0000;
        }
        game->drawRect(bb->x1, bb->y1, bb->x2, bb->y2, color, 0);

	}
	for (int i = 0; i < groundLinesCount; i++) {
        DWORD color = 0xFFFFFF00;
        if (groundLines[i]->getType() == GROUND_LINE_TYPE_WALL) {
            color = 0xFF00FF00;
        }
        groundLines[i]->debugDraw(color);
	}

	game->drawRect(game->screenX(0), game->screenY(0), game->screenX(width), game->screenY(height), 0xFFAAAAAA, 0);

    switch (mode) {
		case MODE_INSERT_ANIM:
			beingInsertedAnim->RenderEx(insertX, insertY, 0, game->getScaleFactor(), game->getScaleFactor());
            break;
        case MODE_ANIM_ROTATE:
			game->getHge()->Gfx_RenderLine(game->screenX(dragOffsetX), game->screenY(dragOffsetY), x, y, 0xFFffa500);
			break;
        case MODE_INSERT_GL_STEP1:
            if (worldX <= 0 || worldX >= width || worldY <= 0 || worldY >= height) {
                disabledIcon->Render(x, y);
            }
			break;
        case MODE_INSERT_GL_STEP2:
            DWORD color = 0xFFFFFFAA;
            if (abs(game->screenY(dragOffsetY) - y) > abs(game->screenX(dragOffsetX) - x)) {
                color = 0xFF00FFAA;
            }
            game->getHge()->Gfx_RenderLine(game->screenX(dragOffsetX), game->screenY(dragOffsetY), x, y, color);
			break;
	}

	game->endDraw();
	return false;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// Get HGE interface
	HGE * hge = hgeCreate(HGE_VERSION);
	hge->System_SetState(HGE_USESOUND, false);
	//hge->System_SetState(HGE_ZBUFFER, true);

	// Set up log file, frame function, render function and window title
	hge->System_SetState(HGE_LOGFILE, "map_editor.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "SoD map editor");

	game = new Game(hge);

	if (game->preload()) {
		//bgTex = game->getHge()->Texture_Load("box.png");

		fnt = new hgeFont("font1.fnt");
        disabledIcon = game->loadAnimation("disabled_icon.xml");

		mainWindow = new GUIWindow(game, 1, 1300, 1, 300, 420);
		game->getGUI()->AddCtrl(mainWindow);
		mainWindow->AddCtrl(new hgeGUIMenuItem(2, fnt, 120, 20, 0.0f, "save", saveMapButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(3, fnt, 180, 20, 0.0f, "load", loadMapButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(4, fnt, 150, 60, 0.0f, "insert anim", insertAnimButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(5, fnt, 150, 90, 0.0f, "insert line", insertGroundLineButtonClick));

		animsWindow = new GUIWindow(game, 100, 1000, 120, 600, 600);
		game->getGUI()->AddCtrl(animsWindow);
		for (int i = 0; i < game->getAnimationsCount(); i++) {
			animsWindow->AddCtrl(new hgeGUIMenuItem(101 + i, fnt, 250, 10 + i * 20, 0.0f, game->getAnimationName(i), animButtonClick));
		}
		animsWindow->Hide();

		game->moveCamera(b2Vec2(-6.5, -4.5));

		game->loop();

		//delete fnt;

	}

	return 0;
}