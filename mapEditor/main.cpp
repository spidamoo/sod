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

const int MODE_SELECT_PLATFORM   = 11;
const int MODE_NEW_PLATFORM = 111;
const int MODE_EDIT_PLATFORM = 112;
const int MODE_DRAG_PLATFORM_SPOT_TIME = 113;
const int MODE_DRAG_PLATFORM = 114;

const int MODE_MOVING_PLATFORMS = 10;

int mode = MODE_DEFAULT;

float width = 20; float height = 10;
int animationsCount = 0; char** animationNames = new char*[256]; hgeAnimation** animations = new hgeAnimation*[256];
float* animationX = new float[256]; float* animationY = new float[256]; float* animationAngle = new float[256];
int groundLinesCount = 0; GroundLine** groundLines = new GroundLine*[256];

int platformsCount = 0; int* platformGroundLinesCounts = new int[256]; int* platformAnimsCounts = new int[256];
int** platformGroundLines = new int*[256]; int** platformAnims = new int*[256];
int* platformSpotsCounts = new int[256]; float** platformSpotX = new float*[256]; float** platformSpotY = new float*[256]; float** platformSpotAngle = new float*[256];
float** platformSpotsTimes = new float*[256];
int selectedPlatform = -1; int selectedPlatformSpot = -1;

float dragOffsetX; float dragOffsetY; float dragOffsetAngle;
int selectedAnim;
float currentTime = 0;

void resetMode()
{
	mode = MODE_DEFAULT;
	mainWindow->Show();
	animsWindow->Hide();
	selectedAnim = -1;
	selectedPlatform = -1;
	selectedPlatformSpot = -1;
	currentTime = 0;
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
b2Vec2 getPlatformShift(int platform) {

	///Считаем общее время движений платформы
	float totalTime = 0;
	for (int l = 0; l < platformSpotsCounts[platform]; l++) {
		totalTime += platformSpotsTimes[platform][l];
	}
	float currentPlatformTime = currentTime;///Найдем текущее время для этой платформы
	while (currentPlatformTime > totalTime) {
		currentPlatformTime -= totalTime;
	}
	float t = 0;
	int l = -1;
	float prevT = 0;
	while (t < currentPlatformTime) {///Находим текущий spot
		prevT = t;
		l++;
		t += platformSpotsTimes[platform][l];
	}
	///Теперь l - индекс следующего спота. t - время следующего спота
	float progress = (currentPlatformTime - prevT) / platformSpotsTimes[platform][l];
	b2Vec2 shift = b2Vec2_zero;
	if (l > 0) {
		shift.x = platformSpotX[platform][l - 1];
		shift.y = platformSpotY[platform][l - 1];
	}
	shift.x += (platformSpotX[platform][l] - shift.x) * progress;
	shift.y += (platformSpotY[platform][l] - shift.y) * progress;
	return shift;
}
float* getPlatformBounds(int platform) {
	float left = 1605; float right = -5;///Края установим для начала за 5 пикселей за экраном
	float top = 905; float bottom = -5;
	float shiftX = 0; float shiftY = 0;///Сдвиг на слуяай, если эта платформа редактируется
	if (mode >= MODE_EDIT_PLATFORM && selectedPlatform == platform && selectedPlatformSpot > -1) {
		shiftX = platformSpotX[selectedPlatform][selectedPlatformSpot];
		shiftY = platformSpotY[selectedPlatform][selectedPlatformSpot];
	}
	if (mode == MODE_MOVING_PLATFORMS) {
		b2Vec2 shift = getPlatformShift(platform);
		shiftX = shift.x; shiftY = shift.y;
	}
	///Перебор анимаций
	for (int j = 0; j < platformAnimsCounts[platform]; j++) {
		hgeRect* bb = new hgeRect();
		animations[platformAnims[platform][j]]->GetBoundingBoxEx(
			game->screenX(animationX[platformAnims[platform][j]] + shiftX),
			game->screenY(animationY[platformAnims[platform][j]] + shiftY),
			animationAngle[platformAnims[platform][j]],
			game->getScaleFactor(),
			game->getScaleFactor(),
			bb
		);
		///Если края крайнее, выберем их
		if (bb->x1 < left) {
			left = bb->x1;
		}
		if (bb->x2 > right) {
			right = bb->x2;
		}
		if (bb->y1 < top) {
			top = bb->y1;
		}
		if (bb->y2 > bottom) {
			bottom = bb->y2;
		}
	}
	///Все то же самое для линий
	for (int j = 0; j < platformGroundLinesCounts[platform]; j++) {
		if (game->screenX(groundLines[platformGroundLines[platform][j]]->getLeft() + shiftX) < left) {
			left = game->screenX(groundLines[platformGroundLines[platform][j]]->getLeft() + shiftX);
		}
		if (game->screenX(groundLines[platformGroundLines[platform][j]]->getRight() + shiftX) > right) {
			right = game->screenX(groundLines[platformGroundLines[platform][j]]->getRight() + shiftX);
		}
		if (game->screenY(groundLines[platformGroundLines[platform][j]]->getTop() + shiftY) < top) {
			top = game->screenY(groundLines[platformGroundLines[platform][j]]->getTop() + shiftY);
		}
		if (game->screenY(groundLines[platformGroundLines[platform][j]]->getBottom() + shiftY) > bottom) {
			bottom = game->screenY(groundLines[platformGroundLines[platform][j]]->getBottom() + shiftY);
		}
	}

	float* bounds = new float[4];
	bounds[0] = left; bounds[1] = top; bounds[2] = right; bounds[3] = bottom;
	return bounds;
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
	for (int i = 0; i < platformsCount; i++) {
        TiXmlElement* element = new TiXmlElement( "platform" );
        root->LinkEndChild( element );
        element->SetAttribute("ground_lines", platformGroundLinesCounts[i]);
        element->SetAttribute("animations", platformAnimsCounts[i]);
        element->SetAttribute("spots", platformSpotsCounts[i]);
        for (int j = 0; j < platformGroundLinesCounts[i]; j++) {
            TiXmlElement* child = new TiXmlElement( "ground_line" );
            element->LinkEndChild( child );
            child->SetAttribute("index", platformGroundLines[i][j]);
        }
        for (int j = 0; j < platformAnimsCounts[i]; j++) {
            TiXmlElement* child = new TiXmlElement( "animation" );
            element->LinkEndChild( child );
            child->SetAttribute("index", platformAnims[i][j]);
        }
        for (int j = 0; j < platformSpotsCounts[i]; j++) {
            TiXmlElement* child = new TiXmlElement( "spot" );
            element->LinkEndChild( child );
            child->SetDoubleAttribute("time", platformSpotsTimes[i][j]);
            child->SetDoubleAttribute("x", platformSpotX[i][j]);
            child->SetDoubleAttribute("y", platformSpotY[i][j]);
            child->SetDoubleAttribute("angle", platformSpotAngle[i][j]);
        }
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

        i = 0;
        element = root->FirstChildElement("platform");
        while (element) {
            int j =0;
            TiXmlElement* child = element->FirstChildElement("ground_line");
            while (child) {
                platformGroundLines[i][j] = atoi(child->Attribute("index"));
                j++;
                child = child->NextSiblingElement("ground_line");
            }
            platformGroundLinesCounts[i] = j;

            j = 0;
            child = element->FirstChildElement("animation");
            while (child) {
                platformAnims[i][j] = atoi(child->Attribute("index"));
                j++;
                child = child->NextSiblingElement("animation");
            }
            platformAnimsCounts[i] = j;

            j = 0;
            child = element->FirstChildElement("spot");
            while (child) {
                platformSpotsTimes[i][j] = atof(child->Attribute("time"));
                platformSpotX[i][j] = atof(child->Attribute("x"));
                platformSpotY[i][j] = atof(child->Attribute("y"));
                platformSpotAngle[i][j] = atof(child->Attribute("angle"));
                j++;
                child = child->NextSiblingElement("spot");
            }
            platformSpotsCounts[i] = j;

            i++;
            element = element->NextSiblingElement("platform");
        }
        platformsCount = i;

        //delete root;
    } else {
        printf("failed\n");
    }
	resetMode();
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
bool spotModeButtonClick(hgeGUIMenuItem* sender)
{
	mode = MODE_SELECT_PLATFORM;
}

bool FrameFunc()
{
    float x, y;
	game->getHge()->Input_GetMousePos(&x, &y);
	float worldX = game->worldX(x);
	float worldY = game->worldY(y);
	float dt = game->getHge()->Timer_GetDelta();

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
			}
			if (game->getHge()->Input_KeyUp(HGEK_RBUTTON)) {
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
		case MODE_SELECT_PLATFORM:
			selectedPlatform = -1;
		    if (x < 1300 && game->getHge()->Input_KeyDown(HGEK_LBUTTON)) {
				/// Выбираем платформу, в которую ткнули
				for (int i = 0; i < platformsCount; i++) {
					float* bounds = getPlatformBounds(i);
					if (x > bounds[0] && y > bounds[1] && x < bounds[2] && y < bounds[3]) {
						selectedPlatform = i;
					}
				}
				if (selectedPlatform == -1) {///Если ни в какую, создаем новую
					dragOffsetX = worldX;
					dragOffsetY = worldY;
					mode = MODE_NEW_PLATFORM;
				} else {/// Если да, редактируем ее
					mode = MODE_EDIT_PLATFORM;
					selectedPlatformSpot = -1;
				}
		    }
			break;
        case MODE_NEW_PLATFORM:
        	{///Скобки нужны чтобы иметь блок, а в нем объявлять переменные
            int c1 = 0;
            for (int i = 0; i < animationsCount; i++) {///Перебираем все картинки
                bool occupied = false;///Проверяем, используется ли картинка уже в какой-то платформе
                for (int j = 0; j < platformsCount; j++) {
                    for (int k = 0; k < platformAnimsCounts[j]; k++) {
                        if (platformAnims[j][k] == i) {
                            occupied = true;
                        }
                    }
                }
                if (occupied) {///Если да, пропускаем ее
                    continue;
                }
                hgeRect* bb = new hgeRect();
                animations[i]->GetBoundingBoxEx(
                    game->screenX(animationX[i]),
                    game->screenY(animationY[i]),
                    animationAngle[i],
                    game->getScaleFactor(),
                    game->getScaleFactor(),
                    bb
                );
                if (///Проверяем, умещается ли bb картинки целиком в выделенную область
                    (
                        (bb->x1 > game->screenX(dragOffsetX) && bb->x2 < x)
                        ||
                        (bb->x2 < game->screenX(dragOffsetX) && bb->x1 > x)
                    ) && (
                        (bb->y1 > game->screenY(dragOffsetY) && bb->y2 < y)
                        ||
                        (bb->y2 < game->screenY(dragOffsetY) && bb->y1 > y)
                    )
                ) {///Если да, то запишем ее в платформу, следующую после крайней (еще не созданную)
                    platformAnims[platformsCount][c1] = i;
                    c1++;
                }
            }
            platformAnimsCounts[platformsCount] = c1;///Записываем количество пойманных картинок

			///Все то же самое для линий
            int c2 = 0;
            for (int i = 0; i < groundLinesCount; i++) {
                bool occupied = false;
                for (int j = 0; j < platformsCount; j++) {
                    for (int k = 0; k < platformGroundLinesCounts[j]; k++) {
                        if (platformGroundLines[j][k] == i) {
                            occupied = true;
                        }
                    }
                }
                if (occupied) {
                    continue;
                }
                if (
                    (
                        (groundLines[i]->getLeft()   > dragOffsetX && groundLines[i]->getRight()  < worldX)
                        ||
                        (groundLines[i]->getRight()  < dragOffsetX && groundLines[i]->getLeft()   > worldX)
                    ) && (
                        (groundLines[i]->getTop()    > dragOffsetY && groundLines[i]->getBottom() < worldY)
                        ||
                        (groundLines[i]->getBottom() < dragOffsetY && groundLines[i]->getTop()    > worldY)
                    )
                ) {
                    platformGroundLines[platformsCount][c2] = i;
                    c2++;
                }
            }
            platformGroundLinesCounts[platformsCount] = c2;

            if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {///Отпустили кнопку - перестали выделять
                if (c1 > 0 || c2 > 0) {///Если была выделена хоть одна картинка или линия, создаем новую платформу
					selectedPlatform = platformsCount;
                    platformsCount++;
					mode = MODE_EDIT_PLATFORM;
					selectedPlatformSpot = -1;
                } else {/// иначе сбрасывамся - считаем это отменой
                    platformAnimsCounts[platformsCount] = 0;
                    platformGroundLinesCounts[platformsCount] = 0;
                    resetMode();
                }
            }
        	}
            break;
		case MODE_EDIT_PLATFORM:
			///Редактирование платформы: добавление и перемещение положений, время перемещений между ними
			if (game->getHge()->Input_KeyDown(HGEK_LBUTTON)) {///Выбираем...
				if (y < 50 && selectedPlatform > -1) {///... верхний интерфейс - временнЫе узлы
					selectedPlatformSpot = -1;
					///Переберем все положения
					float t = 0;///Время начала - 0
					for (int i = 0; i < platformSpotsCounts[selectedPlatform]; i++) {
						if (x > (t + platformSpotsTimes[selectedPlatform][i]) * 50 + (i + 1) * 50 ///Попадаем в квадрат 50х50
							&& x < 50 + (t + platformSpotsTimes[selectedPlatform][i]) * 50 + (i + 1) * 50
						) {
							selectedPlatformSpot = i;
							mode = MODE_DRAG_PLATFORM_SPOT_TIME;
							dragOffsetX = x;///Запоминаем откуда начали
						}
						t += platformSpotsTimes[selectedPlatform][i];///Прибавляем время этого положения
					}
					if (x < 50) {
						selectedPlatformSpot = -1;
					}
					if (x > 50 + t * 50 + platformSpotsCounts[selectedPlatform] * 50
							&& x < 100 + t * 50 + platformSpotsCounts[selectedPlatform] * 50) {///Попали в большой +
						platformSpotAngle[selectedPlatform][platformSpotsCounts[selectedPlatform]] = 0;
						platformSpotX[selectedPlatform][platformSpotsCounts[selectedPlatform]] = 0;
						platformSpotY[selectedPlatform][platformSpotsCounts[selectedPlatform]] = 0;
						platformSpotsTimes[selectedPlatform][platformSpotsCounts[selectedPlatform]] = 0;
						platformSpotsCounts[selectedPlatform]++;
					}
				} else if (x < 1300) {/// ... платформу, в которую ткнули
					for (int i = 0; i < platformsCount; i++) {
						float* bounds = getPlatformBounds(i);
						if (x > bounds[0] && y > bounds[1] && x < bounds[2] && y < bounds[3]) {
							selectedPlatform = i;
							if (selectedPlatformSpot > -1) {
								mode = MODE_DRAG_PLATFORM;///Начинаем тащить
								dragOffsetX = worldX;///Запоминаем откуда начали
								dragOffsetY = worldY;
							}
						}
						delete bounds;
					}
				}
			}
			if (game->getHge()->Input_KeyDown(HGEK_SPACE)) {
				//resetMode();
				mode = MODE_MOVING_PLATFORMS;
			}
			break;
		case MODE_DRAG_PLATFORM_SPOT_TIME:
			platformSpotsTimes[selectedPlatform][selectedPlatformSpot] += (x - dragOffsetX) * 0.02;///Прибавляем сдвиг с последнего раза
			if (platformSpotsTimes[selectedPlatform][selectedPlatformSpot] < 0) {///Время не может быть отрицательным
				platformSpotsTimes[selectedPlatform][selectedPlatformSpot] = 0;
			} else {
				dragOffsetX = x;///И запоминаем текущее положение
			}
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
				///Возвращаемся назад
				mode = MODE_EDIT_PLATFORM;
			}
			break;
		case MODE_DRAG_PLATFORM:
			platformSpotX[selectedPlatform][selectedPlatformSpot] += (worldX - dragOffsetX);///Прибавляем сдвиг с последнего раза
			platformSpotY[selectedPlatform][selectedPlatformSpot] += (worldY - dragOffsetY);
			dragOffsetX = worldX;///И запоминаем текущее положение
			dragOffsetY = worldY;
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
				///Возвращаемся назад
				mode = MODE_EDIT_PLATFORM;
			}
			break;
		case MODE_MOVING_PLATFORMS:
			currentTime += dt;
			if (game->getHge()->Input_KeyDown(HGEK_SPACE)) {
				//resetMode();
				mode = MODE_EDIT_PLATFORM;
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
		DWORD color = 0xFFFFFFFF;
		DWORD bbColor = 0xFFAA0000;
		float shiftX = 0;///Сдвиг на случай, если мы редактируем платформу
		float shiftY = 0;
		if (mode >= MODE_SELECT_PLATFORM) {
			color = 0xAAFFFFFF;bbColor = 0xAAAA0000;
			for (int j = 0; j < platformsCount; j++) {
				for (int k = 0; k < platformAnimsCounts[j]; k++) {
					if (platformAnims[j][k] == i) {
						color = 0xAAAAAAAA;///Анимация относится к какой-то левой платформе
						bbColor = 0xAA550000;
					}
				}
			}
			for (int k = 0; k < platformAnimsCounts[platformsCount]; k++) {
                if (platformAnims[platformsCount][k] == i) {
                    color = 0xFFFFFFFF;///Анимация относится к создаваемой платформе
                    bbColor = 0xFFAA0000;

                }
            }
            if (selectedPlatform > -1) {
				for (int k = 0; k < platformAnimsCounts[selectedPlatform]; k++) {
					if (platformAnims[selectedPlatform][k] == i) {
						color = 0xFFFFFFFF;///Анимация относится к редактируемой платформе
						bbColor = 0xFFAA0000;
						if (selectedPlatformSpot > -1) {
							shiftX = platformSpotX[selectedPlatform][selectedPlatformSpot];
							shiftY = platformSpotY[selectedPlatform][selectedPlatformSpot];
						}
					}
				}
            }
		}
		if (mode == MODE_MOVING_PLATFORMS) {
			for (int j = 0; j < platformsCount; j++) {
				for (int k = 0; k < platformAnimsCounts[j]; k++) {
					if (platformAnims[j][k] == i) {
						///Нашли платформу, к которой относится эта анимация, получаем ее сдвиг
						b2Vec2 shift = getPlatformShift(j);
						shiftX = shift.x; shiftY = shift.y;
					}
				}
			}
		}
		animations[i]->SetColor(color);
        animations[i]->RenderEx(
            game->screenX(animationX[i] + shiftX),
            game->screenY(animationY[i] + shiftY),
            animationAngle[i],
            game->getScaleFactor(),
            game->getScaleFactor()
        );

        hgeRect* bb = new hgeRect();
        animations[i]->GetBoundingBoxEx(
            game->screenX(animationX[i] + shiftX),
            game->screenY(animationY[i] + shiftY),
            animationAngle[i],
            game->getScaleFactor(),
            game->getScaleFactor(),
            bb
        );

        if (i == selectedAnim) {
            bbColor = 0xFFFF0000;
        }
        game->drawRect(bb->x1, bb->y1, bb->x2, bb->y2, bbColor, 0);

	}
	for (int i = 0; i < groundLinesCount; i++) {
        DWORD alpha = 0xFF000000;
        DWORD rgb = 0x00FFFF00;
        if (groundLines[i]->getType() == GROUND_LINE_TYPE_WALL) {
            rgb = 0x0000FF00;
        }
        b2Vec2 shift = b2Vec2_zero;///Сдвиг на случай, если мы редактируем платформу
        if (mode >= MODE_SELECT_PLATFORM) {
			alpha = 0xAA000000;
			for (int j = 0; j < platformsCount; j++) {
				for (int k = 0; k < platformGroundLinesCounts[j]; k++) {
					if (platformGroundLines[j][k] == i) {
						alpha = 0x55000000;///Линия относится к какой-то левой платформе
					}
				}
			}
			for (int k = 0; k < platformGroundLinesCounts[platformsCount]; k++) {
                if (platformGroundLines[platformsCount][k] == i) {
                    alpha = 0xFF000000;///Линия относится к создаваемой платформе
                }
            }
            if (selectedPlatform > -1) {
				for (int k = 0; k < platformGroundLinesCounts[selectedPlatform]; k++) {
					if (platformGroundLines[selectedPlatform][k] == i) {
						alpha = 0xFF000000;///Линия относится к редактируемой платформе
						if (selectedPlatformSpot > -1) {
							shift.x = platformSpotX[selectedPlatform][selectedPlatformSpot];
							shift.y = platformSpotY[selectedPlatform][selectedPlatformSpot];
						}
					}
				}
            }
		}
		if (mode == MODE_MOVING_PLATFORMS) {
			for (int j = 0; j < platformsCount; j++) {
				for (int k = 0; k < platformGroundLinesCounts[j]; k++) {
					if (platformGroundLines[j][k] == i) {
						///Нашли платформу, к которой относится эта линия, получаем ее сдвиг
						shift = getPlatformShift(j);
					}
				}
			}
		}
        DWORD color = alpha + rgb;
        game->drawLine(game->screenPos(groundLines[i]->getStartPoint() + shift), game->screenPos(groundLines[i]->getEndPoint() + shift), color);
	}
	///Нарисуем рамочки вокруг платформ
	for (int i = 0; i < platformsCount; i++) {
		float* bounds = getPlatformBounds(i);
        DWORD color = 0xFF00AAFF;
        if (i == selectedPlatform) {
			color = 0xFF0000FF;
        }
        game->drawRect(bounds[0], bounds[1], bounds[2], bounds[3], color, 0);
        delete bounds;
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
            if (worldX <= 0 || worldX >= width || worldY <= 0 || worldY >= height) {
                disabledIcon->Render(x, y);
            } else {
                DWORD color = 0xFFFFFFAA;
                if (abs(game->screenY(dragOffsetY) - y) > abs(game->screenX(dragOffsetX) - x)) {
                    color = 0xFF00FFAA;
                }
                game->getHge()->Gfx_RenderLine(game->screenX(dragOffsetX), game->screenY(dragOffsetY), x, y, color);
            }
			break;
        case MODE_NEW_PLATFORM:
            game->drawRect(game->screenX(dragOffsetX), game->screenY(dragOffsetY), x, y, 0xFF0000AA, 0);
            break;
	}

	if (mode >= MODE_EDIT_PLATFORM && selectedPlatform > -1) {
		///Нарисуем синие кругали
		///Стартовый кругаль
		DWORD color = 0xFF0000FF;
		if (-1 == selectedPlatformSpot) {///Текущий кругаль подсвечен
			color = 0xFF00AAFF;
		}
		game->drawCircle(25, 25, 25, color, color);
		float t = 0;///Начинаем с времени 0
		fnt->SetColor(0xFFFFFFFF);
		for (int i = 0; i < platformSpotsCounts[selectedPlatform]; i++) {
			DWORD color = 0xFF0000FF;
			if (i == selectedPlatformSpot) {///Текущий кругаль подсвечен
				color = 0xFF00AAFF;
			}
			///1 секунда=50 пикселей, каждый кругаль еще +50
			///Рисуем перемычки между кругалями...
			game->drawRect(t * 50 + (i + 1) * 50, 20, (t + platformSpotsTimes[selectedPlatform][i]) * 50 + (i + 1) * 50, 30, 0xFF0000FF, 0xFF0000FF);
			///Подпишем длину
			float width = (platformSpotsTimes[selectedPlatform][i]) * 50;
			float y = 20;
			if (width < 50) {
				y = 50;
			}
			if (width < 50) {
				width = 50;
			}
			fnt->printfb(t * 50 + (i + 1) * 50, y, width, 10, HGETEXT_CENTER | HGETEXT_MIDDLE, "%.2f", platformSpotsTimes[selectedPlatform][i]);
			///И сами кругали
			game->drawCircle(25 + (t + platformSpotsTimes[selectedPlatform][i]) * 50 + (i + 1) * 50, 25, 25, color, color);
			t += platformSpotsTimes[selectedPlatform][i];
		}
		///Большой зеленый плюс в рамочке
		game->drawRect(50 + t * 50 + platformSpotsCounts[selectedPlatform] * 50, 0, 100 + 50 * t + platformSpotsCounts[selectedPlatform] * 50, 50, 0xFF00AA00, 0xAAFFFFFF);
		game->drawRect(56 + t * 50 + platformSpotsCounts[selectedPlatform] * 50, 23, 94 + t * 50 + platformSpotsCounts[selectedPlatform] * 50, 27, 0xFF00AA00, 0xFF00AA00);
		game->drawRect(73 + t * 50 + platformSpotsCounts[selectedPlatform] * 50, 6, 77 + t * 50 + platformSpotsCounts[selectedPlatform] * 50, 44, 0xFF00AA00, 0xFF00AA00);
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

	for (int i = 0; i < 256; i++) {
		platformGroundLines[i] = new int[256];
		platformAnims[i] = new int[256];
		platformSpotX[i] = new float[256];
		platformSpotY[i] = new float[256];
		platformSpotAngle[i] = new float[256];
		platformSpotsTimes[i] = new float[256];

		platformAnimsCounts[i] = 0;
		platformGroundLinesCounts[i] = 0;
		platformSpotsCounts[i] = 0;
	}

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
		mainWindow->AddCtrl(new hgeGUIMenuItem(6, fnt, 150, 120, 0.0f, "spot mode", spotModeButtonClick));

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
