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
const int MODE_ANIM_DRAG   = 7;
const int MODE_CAMERA_MOVE = 8;
const int MODE_ANIM_ROTATE = 9;
const int MODE_LAYER_DRAG  = 10;
const int MODE_ANIM_CONTEXT_MENU = 11;
const int MODE_RESIZE_CHARACTER = 12;

const int MODE_PLAYING     = 20;

const int MODE_ADD_HOTSPOT  = 31;
const int MODE_ADD_HOTSPOT_BOX_STEP1  = 32;
const int MODE_ADD_HOTSPOT_BOX_STEP2  = 33;
const int MODE_HOTSPOT_DRAG  = 34;
const int MODE_HOTSPOT_ROTATE  = 35;

const int MODE_ANGLE_DRAG  = 40;

const int MODE_ANIM_EDIT   = 100;

const int EDIT_MODE_FRAMES   = 0;
const int EDIT_MODE_ANGLES   = 1;

int mode = MODE_DEFAULT;
int editMode = EDIT_MODE_FRAMES;

GUIWindow* mainWindow;
GUIWindow* animsWindow;
GUIWindow* hotspotTypesWindow;
GUIWindow* animContextMenuWindow;

hgeAnimation* beingInsertedAnim;
char* beingInsertedAnimName;

int selectedBody = -1;
int selectedLayer = -1;
int selectedHotSpot = -1;

float dragOffsetX;
float dragOffsetY;
float dragOffsetAngle;

float selectedBodyX;
float selectedBodyY;
float selectedBodyAngle;

float selectedHotSpotX;
float selectedHotSpotY;
float selectedHotSpotAngle;

hgeAnimation** animations = new hgeAnimation*[256];
CharacterAction*** actions = new CharacterAction**[256];
CharacterHotSpot** hotSpots = new CharacterHotSpot*[256];
float* hotSpotWidths = new float[256];
float* hotSpotHeights = new float[256];
int bodiesCount = 0;
int hotSpotsCount = 0;

float characterHeight = 2; float characterWidth = 0.5;

float* animationX = new float[256];
float* animationY = new float[256];
float* animationAngle = new float[256];
char** animationNames = new char*[256];

float* hotSpotX = new float[256];
float* hotSpotY = new float[256];
float* hotSpotAngle = new float[256];

int* framesCounts = new int[256];
float** framesLengths = new float*[256];
int* actionsCounts = new int[256];
int currentFrame = 0;
int animationsCount = 1;
int currentAnimation = 0;
float*** frameAnimX = new float**[256];
float*** frameAnimY = new float**[256];
float*** frameAnimAngle = new float**[256];
int*** frameAnimLayer = new int**[256];
bool*** frameAnimShow = new bool**[256];
bool*** frameAnimHideLayer = new bool**[256];
float*** angleAnimX = new float**[256]; float*** angleAnimY = new float**[256]; float*** angleAnimAngle = new float**[256];
float*** angleHotSpotX = new float**[256]; float*** angleHotSpotY = new float**[256]; float*** angleHotSpotAngle = new float**[256];
bool*** angleHotSpotShow = new bool**[256];
bool*** angleAnimShow = new bool**[256];
int* angleCounts = new int[256]; float** angles = new float*[256];
bool** animRotating = new bool*[256]; bool** hotSpotRotating = new bool*[256];
float*** frameHotSpotX = new float**[256];
float*** frameHotSpotY = new float**[256];
float*** frameHotSpotAngle = new float**[256];
bool*** frameHotSpotShow = new bool**[256];

int currentTab = 0;
float currentTime = 0;
float frameProgress = 0;
bool drawBoxes = true;

HTEXTURE bgTex;
hgeFont* fnt;

hgeAnimation* disabledIcon; hgeAnimation* resizeIcon;

hgeQuad grayQuad;

void resetMode()
{
	mode = MODE_DEFAULT;
	selectedBody = -1;
	selectedLayer = -1;
	selectedHotSpot = -1;
	mainWindow->Show();
	animsWindow->Hide();
	animContextMenuWindow->Hide();
}

float animX(int anim)
{
	if (!animRotating[currentAnimation][anim]) {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return frameAnimX[currentAnimation][currentFrame][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return frameAnimX[currentAnimation][0][anim];
			}
		}
		int nextFrame;
		if (currentFrame >= framesCounts[currentAnimation] - 1) {
			nextFrame = 0;
		} else {
			nextFrame = currentFrame + 1;
		}
		return frameAnimX[currentAnimation][currentFrame][anim]
			+ (frameAnimX[currentAnimation][nextFrame][anim] - frameAnimX[currentAnimation][currentFrame][anim]) * frameProgress;
	} else {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return angleAnimX[currentAnimation][0][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return angleAnimX[currentAnimation][currentFrame][anim];
			}
		} else {
			float x, y;
			game->getHge()->Input_GetMousePos(&x, &y);
			float worldX = game->worldX(x);
			float worldY = game->worldY(y);
			float angle = atan2(worldY, worldX);
			int prevAngle = 0; int nextAngle = 0;
			float prevDiff = - M_PI * 2;
			float nextDiff = M_PI * 2;
			for (int i = 0; i < angleCounts[currentAnimation]; i++) {
				float nAngle = angles[currentAnimation][i];
				float pAngle = angles[currentAnimation][i];
				while (nAngle < angle) {
					nAngle += M_PI * 2;
				}
				while (pAngle > angle) {
					pAngle -= M_PI * 2;
				}
				if (nAngle - angle < nextDiff) {
					nextDiff = nAngle - angle;
					nextAngle = i;
				}
				if (pAngle - angle > prevDiff) {
					prevDiff = pAngle - angle;
					prevAngle = i;
				}
			}

			float progress = (prevDiff / (prevDiff - nextDiff));
			return angleAnimX[currentAnimation][prevAngle][anim]
				+ (angleAnimX[currentAnimation][nextAngle][anim] - angleAnimX[currentAnimation][prevAngle][anim]) * progress;
		}
	}
}

float animY(int anim)
{
	if (!animRotating[currentAnimation][anim]) {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return frameAnimY[currentAnimation][currentFrame][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return frameAnimY[currentAnimation][0][anim];
			}
		}
		int nextFrame;
		if (currentFrame >= framesCounts[currentAnimation] - 1) {
			nextFrame = 0;
		} else {
			nextFrame = currentFrame + 1;
		}
		return frameAnimY[currentAnimation][currentFrame][anim]
			+ (frameAnimY[currentAnimation][nextFrame][anim] - frameAnimY[currentAnimation][currentFrame][anim]) * frameProgress;
	} else {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return angleAnimY[currentAnimation][0][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return angleAnimY[currentAnimation][currentFrame][anim];
			}
		} else {
			float x, y;
			game->getHge()->Input_GetMousePos(&x, &y);
			float worldX = game->worldX(x);
			float worldY = game->worldY(y);
			float angle = atan2(worldY, worldX);
			int prevAngle = 0; int nextAngle = 0;
			float prevDiff = - M_PI * 2;
			float nextDiff = M_PI * 2;
			for (int i = 0; i < angleCounts[currentAnimation]; i++) {
				float nAngle = angles[currentAnimation][i];
				float pAngle = angles[currentAnimation][i];
				while (nAngle < angle) {
					nAngle += M_PI * 2;
				}
				while (pAngle > angle) {
					pAngle -= M_PI * 2;
				}
				if (nAngle - angle < nextDiff) {
					nextDiff = nAngle - angle;
					nextAngle = i;
				}
				if (pAngle - angle > prevDiff) {
					prevDiff = pAngle - angle;
					prevAngle = i;
				}
			}

			float progress = (prevDiff / (prevDiff - nextDiff));
			return angleAnimY[currentAnimation][prevAngle][anim]
				+ (angleAnimY[currentAnimation][nextAngle][anim] - angleAnimY[currentAnimation][prevAngle][anim]) * progress;
		}
	}
}

float animAngle(int anim)
{
	if (!animRotating[currentAnimation][anim]) {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return frameAnimAngle[currentAnimation][currentFrame][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return frameAnimAngle[currentAnimation][0][anim];
			}
		}
		int nextFrame;
		if (currentFrame >= framesCounts[currentAnimation] - 1) {
			nextFrame = 0;
		} else {
			nextFrame = currentFrame + 1;
		}
		return frameAnimAngle[currentAnimation][currentFrame][anim]
			+ (frameAnimAngle[currentAnimation][nextFrame][anim] - frameAnimAngle[currentAnimation][currentFrame][anim]) * frameProgress;
	} else {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return angleAnimAngle[currentAnimation][0][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return angleAnimAngle[currentAnimation][currentFrame][anim];
			}
		} else {
			float x, y;
			game->getHge()->Input_GetMousePos(&x, &y);
			float worldX = game->worldX(x);
			float worldY = game->worldY(y);
			float angle = atan2(worldY, worldX);
			int prevAngle = 0; int nextAngle = 0;
			float prevDiff = - M_PI * 2;
			float nextDiff = M_PI * 2;
			for (int i = 0; i < angleCounts[currentAnimation]; i++) {
				float nAngle = angles[currentAnimation][i];
				float pAngle = angles[currentAnimation][i];
				while (nAngle < angle) {
					nAngle += M_PI * 2;
				}
				while (pAngle > angle) {
					pAngle -= M_PI * 2;
				}
				if (nAngle - angle < nextDiff) {
					nextDiff = nAngle - angle;
					nextAngle = i;
				}
				if (pAngle - angle > prevDiff) {
					prevDiff = pAngle - angle;
					prevAngle = i;
				}
			}

			float progress = (prevDiff / (prevDiff - nextDiff));
			float pAngle = angleAnimAngle[currentAnimation][prevAngle][anim];
			float nAngle = angleAnimAngle[currentAnimation][nextAngle][anim];
			if (nAngle < pAngle) {
                nAngle += 2 * M_PI;
			}
			return pAngle
				+ (nAngle - pAngle) * progress;
		}
	}
}
float animShow(int anim) {
	if (!animRotating[currentAnimation][anim]) {
		if (editMode == EDIT_MODE_FRAMES) {
			return frameAnimShow[currentAnimation][currentFrame][anim];
		} else if (editMode == EDIT_MODE_ANGLES) {
			return frameAnimShow[currentAnimation][0][anim];
		}
	} else {
		if (editMode == EDIT_MODE_FRAMES) {
			return angleAnimShow[currentAnimation][0][anim];
		} else if (editMode == EDIT_MODE_ANGLES) {
			return angleAnimShow[currentAnimation][currentFrame][anim];
		}
	}
}
bool animHideLayer(int anim)
{
	return frameAnimHideLayer[currentAnimation][currentFrame][anim];
}

int animLayer(int anim)
{
	if (editMode == EDIT_MODE_FRAMES) {
		return frameAnimLayer[currentAnimation][currentFrame][anim];
	} else if (editMode == EDIT_MODE_ANGLES) {
		return frameAnimLayer[currentAnimation][0][anim];
	}
}

float hsX(int hs)
{
    if (!hotSpotRotating[currentAnimation][hs]) {
        if (mode != MODE_PLAYING) {
            if (editMode == EDIT_MODE_FRAMES) {
                return frameHotSpotX[currentAnimation][currentFrame][hs];
            } else if (editMode == EDIT_MODE_ANGLES) {
				return frameHotSpotX[currentAnimation][0][hs];
			}
        }
        int nextFrame;
        if (currentFrame >= framesCounts[currentAnimation] - 1) {
            nextFrame = 0;
        } else {
            nextFrame = currentFrame + 1;
        }
        return frameHotSpotX[currentAnimation][currentFrame][hs]
            + (frameHotSpotX[currentAnimation][nextFrame][hs] - frameHotSpotX[currentAnimation][currentFrame][hs]) * frameProgress;
    } else {
    	if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return angleHotSpotX[currentAnimation][0][hs];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return angleHotSpotX[currentAnimation][currentFrame][hs];
			}
		} else {
			float x, y;
			game->getHge()->Input_GetMousePos(&x, &y);
			float worldX = game->worldX(x);
			float worldY = game->worldY(y);
			float angle = atan2(worldY, worldX);
			int prevAngle = 0; int nextAngle = 0;
			float prevDiff = - M_PI * 2;
			float nextDiff = M_PI * 2;
			for (int i = 0; i < angleCounts[currentAnimation]; i++) {
				float nAngle = angles[currentAnimation][i];
				float pAngle = angles[currentAnimation][i];
				while (nAngle < angle) {
					nAngle += M_PI * 2;
				}
				while (pAngle > angle) {
					pAngle -= M_PI * 2;
				}
				if (nAngle - angle < nextDiff) {
					nextDiff = nAngle - angle;
					nextAngle = i;
				}
				if (pAngle - angle > prevDiff) {
					prevDiff = pAngle - angle;
					prevAngle = i;
				}
			}

			float progress = (prevDiff / (prevDiff - nextDiff));
			return angleHotSpotX[currentAnimation][prevAngle][hs]
				+ (angleHotSpotX[currentAnimation][nextAngle][hs] - angleHotSpotX[currentAnimation][prevAngle][hs]) * progress;
		}
	}
}

float hsY(int hs)
{
    if (!hotSpotRotating[currentAnimation][hs]) {
        if (mode != MODE_PLAYING) {
            if (editMode == EDIT_MODE_FRAMES) {
                return frameHotSpotY[currentAnimation][currentFrame][hs];
            } else if (editMode == EDIT_MODE_ANGLES) {
				return frameHotSpotY[currentAnimation][0][hs];
			}
        }
        int nextFrame;
        if (currentFrame >= framesCounts[currentAnimation] - 1) {
            nextFrame = 0;
        } else {
            nextFrame = currentFrame + 1;
        }
        return frameHotSpotY[currentAnimation][currentFrame][hs]
            + (frameHotSpotY[currentAnimation][nextFrame][hs] - frameHotSpotY[currentAnimation][currentFrame][hs]) * frameProgress;
    } else {
    	if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return angleHotSpotY[currentAnimation][0][hs];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return angleHotSpotY[currentAnimation][currentFrame][hs];
			}
		} else {
			float x, y;
			game->getHge()->Input_GetMousePos(&x, &y);
			float worldX = game->worldX(x);
			float worldY = game->worldY(y);
			float angle = atan2(worldY, worldX);
			int prevAngle = 0; int nextAngle = 0;
			float prevDiff = - M_PI * 2;
			float nextDiff = M_PI * 2;
			for (int i = 0; i < angleCounts[currentAnimation]; i++) {
				float nAngle = angles[currentAnimation][i];
				float pAngle = angles[currentAnimation][i];
				while (nAngle < angle) {
					nAngle += M_PI * 2;
				}
				while (pAngle > angle) {
					pAngle -= M_PI * 2;
				}
				if (nAngle - angle < nextDiff) {
					nextDiff = nAngle - angle;
					nextAngle = i;
				}
				if (pAngle - angle > prevDiff) {
					prevDiff = pAngle - angle;
					prevAngle = i;
				}
			}

			float progress = (prevDiff / (prevDiff - nextDiff));
			return angleHotSpotY[currentAnimation][prevAngle][hs]
				+ (angleHotSpotY[currentAnimation][nextAngle][hs] - angleHotSpotY[currentAnimation][prevAngle][hs]) * progress;
		}
	}
}

float hsAngle(int hs)
{
    if (!hotSpotRotating[currentAnimation][hs]) {
        if (mode != MODE_PLAYING) {
            if (editMode == EDIT_MODE_FRAMES) {
                return frameHotSpotAngle[currentAnimation][currentFrame][hs];
            } else if (editMode == EDIT_MODE_ANGLES) {
				return frameHotSpotAngle[currentAnimation][0][hs];
			}
        }
        int nextFrame;
        if (currentFrame >= framesCounts[currentAnimation] - 1) {
            nextFrame = 0;
        } else {
            nextFrame = currentFrame + 1;
        }
        return frameHotSpotAngle[currentAnimation][currentFrame][hs]
            + (frameHotSpotAngle[currentAnimation][nextFrame][hs] - frameHotSpotAngle[currentAnimation][currentFrame][hs]) * frameProgress;
    } else {
    	if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return angleHotSpotAngle[currentAnimation][0][hs];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return angleHotSpotAngle[currentAnimation][currentFrame][hs];
			}
		} else {
			float x, y;
			game->getHge()->Input_GetMousePos(&x, &y);
			float worldX = game->worldX(x);
			float worldY = game->worldY(y);
			float angle = atan2(worldY, worldX);
			int prevAngle = 0; int nextAngle = 0;
			float prevDiff = - M_PI * 2;
			float nextDiff = M_PI * 2;
			for (int i = 0; i < angleCounts[currentAnimation]; i++) {
				float nAngle = angles[currentAnimation][i];
				float pAngle = angles[currentAnimation][i];
				while (nAngle < angle) {
					nAngle += M_PI * 2;
				}
				while (pAngle > angle) {
					pAngle -= M_PI * 2;
				}
				if (nAngle - angle < nextDiff) {
					nextDiff = nAngle - angle;
					nextAngle = i;
				}
				if (pAngle - angle > prevDiff) {
					prevDiff = pAngle - angle;
					prevAngle = i;
				}
			}

			float progress = (prevDiff / (prevDiff - nextDiff));
			return angleHotSpotAngle[currentAnimation][prevAngle][hs]
				+ (angleHotSpotAngle[currentAnimation][nextAngle][hs] - angleHotSpotAngle[currentAnimation][prevAngle][hs]) * progress;
		}
	}
}
float hsShow(int hs) {
	if (!hotSpotRotating[currentAnimation][hs]) {
		if (editMode == EDIT_MODE_FRAMES) {
			return frameHotSpotShow[currentAnimation][currentFrame][hs];
		} else if (editMode == EDIT_MODE_ANGLES) {
			return frameHotSpotShow[currentAnimation][0][hs];
		}
	} else {
		if (editMode == EDIT_MODE_FRAMES) {
			return angleHotSpotShow[currentAnimation][0][hs];
		} else if (editMode == EDIT_MODE_ANGLES) {
			return angleHotSpotShow[currentAnimation][currentFrame][hs];
		}
	}
}

void setAnimX(int anim, float x)
{
	if (editMode == EDIT_MODE_FRAMES) {
		frameAnimX[currentAnimation][currentFrame][anim] = x;
		animRotating[currentAnimation][anim] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleAnimX[currentAnimation][currentFrame][anim] = x;
		animRotating[currentAnimation][anim] = true;
	}
}
void setAnimY(int anim, float y)
{
	if (editMode == EDIT_MODE_FRAMES) {
		frameAnimY[currentAnimation][currentFrame][anim] = y;
		animRotating[currentAnimation][anim] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleAnimY[currentAnimation][currentFrame][anim] = y;
		animRotating[currentAnimation][anim] = true;
	}
}
void setAnimAngle(int anim, float angle)
{
	if (editMode == EDIT_MODE_FRAMES) {
		frameAnimAngle[currentAnimation][currentFrame][anim] = angle;
		animRotating[currentAnimation][anim] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleAnimAngle[currentAnimation][currentFrame][anim] = angle;
		animRotating[currentAnimation][anim] = true;
	}
}
void setAnimShow(int anim, bool show)
{
	if (editMode == EDIT_MODE_FRAMES) {
		frameAnimShow[currentAnimation][currentFrame][anim] = show;
		animRotating[currentAnimation][anim] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleAnimShow[currentAnimation][currentFrame][anim] = show;
		animRotating[currentAnimation][anim] = true;
	}
}

void setHsX(int hs, float x)
{
	if (editMode == EDIT_MODE_FRAMES) {
		frameHotSpotX[currentAnimation][currentFrame][hs] = x;
		hotSpotRotating[currentAnimation][hs] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleHotSpotX[currentAnimation][currentFrame][hs] = x;
		hotSpotRotating[currentAnimation][hs] = true;
	}
}
void setHsY(int hs, float y)
{
	if (editMode == EDIT_MODE_FRAMES) {
		frameHotSpotY[currentAnimation][currentFrame][hs] = y;
		hotSpotRotating[currentAnimation][hs] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleHotSpotY[currentAnimation][currentFrame][hs] = y;
		hotSpotRotating[currentAnimation][hs] = true;
	}
}
void setHsAngle(int hs, float angle)
{
	if (editMode == EDIT_MODE_FRAMES) {
		frameHotSpotAngle[currentAnimation][currentFrame][hs] = angle;
		hotSpotRotating[currentAnimation][hs] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleHotSpotAngle[currentAnimation][currentFrame][hs] = angle;
		hotSpotRotating[currentAnimation][hs] = true;
	}
}
void setHsShow(int hs, bool show)
{
	if (editMode == EDIT_MODE_FRAMES) {
		frameHotSpotShow[currentAnimation][currentFrame][hs] = show;
		hotSpotRotating[currentAnimation][hs] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleHotSpotShow[currentAnimation][currentFrame][hs] = show;
		hotSpotRotating[currentAnimation][hs] = true;
	}
}

int getPointedBody(float x, float y)
{
	int selected = -1;
	for (int index = 0; index < bodiesCount; index++) {
		int i = animLayer(index);
		if (animShow(i) && !frameAnimHideLayer[currentAnimation][currentFrame][i]) {
			hgeRect* bb = new hgeRect();
			animations[i]->GetBoundingBoxEx(
				game->screenX(animX(i)),
				game->screenY(animY(i)),
				animAngle(i),
				game->getScaleFactor(),
				game->getScaleFactor(),
				bb
			);

			if (bb->TestPoint(x, y))
				selected = i;
		} else if (!animShow(i)) {
			hgeRect* bb = new hgeRect();
			if (animations[i]->GetBoundingBoxEx(1300 + animationX[i], 450 - 1000 * currentTab + animationY[i], animationAngle[i], 1, 1, bb)->TestPoint(x, y))
				selected = i;
		}
	}

	return selected;
}

int getPointedHotSpot(float x, float y)
{
	int selected = -1;
	//b2Vec2 point(x, y);
	for (int i = 0; i < hotSpotsCount; i++) {
		if (hsShow(i)) {
			b2Transform trans(
				b2Vec2(hsX(i), hsY(i)),
				b2Rot(hsAngle(i))
			);
			if (hotSpots[i]->getShape()->TestPoint(trans, b2Vec2(game->worldX(x), game->worldY(y))))
				selected = i;
		} else {
			b2Transform trans(
				b2Vec2(hotSpotX[i] / game->getPixelsPerMeter(), hotSpotY[i] / game->getPixelsPerMeter()),
				b2Rot(hotSpotAngle[i])
			);
			if (
				hotSpots[i]->getShape()->TestPoint(
					trans,
					b2Vec2(
						(x - 1300) / game->getPixelsPerMeter(),
						(y - 450 - 1000 * currentTab) / game->getPixelsPerMeter()
					)
				)
			) {
				selected = i;
			}
		}
	}
	return selected;
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
	game->getHge()->Input_SetMousePos(1450, 600);
}

bool saveCharacter(char* fn)
{
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );
	TiXmlElement* characterElem = new TiXmlElement( "character" );
	characterElem->SetAttribute("bodies", bodiesCount);
	characterElem->SetAttribute("hotspots", hotSpotsCount);
	characterElem->SetDoubleAttribute("height", characterHeight);
	characterElem->SetDoubleAttribute("width", characterWidth);

	for (int i = 0; i < bodiesCount; i++) {
        TiXmlElement* element = new TiXmlElement( "body" );
        characterElem->LinkEndChild( element );
        element->SetAttribute("animation", animationNames[i]);
        element->SetDoubleAttribute("x", animationX[i]);
        element->SetDoubleAttribute("y", animationY[i]);
        element->SetDoubleAttribute("angle", animationAngle[i]);
        //cxn->SetDoubleAttribute("timeout", 123.456);
	}
	for (int i = 0; i < hotSpotsCount; i++) {
        TiXmlElement* element = new TiXmlElement( "hotspot" );
        characterElem->LinkEndChild( element );
        element->SetAttribute("shape", "box");
        element->SetDoubleAttribute("x", hotSpotX[i]);
        element->SetDoubleAttribute("y", hotSpotY[i]);
        element->SetDoubleAttribute("angle", hotSpotAngle[i]);
        element->SetDoubleAttribute("w", hotSpotWidths[i]);
        element->SetDoubleAttribute("h", hotSpotHeights[i]);
        //cxn->SetDoubleAttribute("timeout", 123.456);
	}

	characterElem->SetAttribute("animations", animationsCount);
	for (int i = 0; i < animationsCount; i++) {
        TiXmlElement* animationElem = new TiXmlElement( "animation" );
		animationElem->SetAttribute("frames", framesCounts[i]);
		animationElem->SetAttribute("angles", angleCounts[i]);
		for (int j = 0; j < bodiesCount; j++) {
			TiXmlElement* bodyElem = new TiXmlElement( "body" );
			bodyElem->SetAttribute("rotating", animRotating[i][j] ? 1 : 0);
			animationElem->LinkEndChild( bodyElem );
		}
        //animationElem->SetAttribute("animation", animationNames[i]);
        for (int j = 0; j < framesCounts[i]; j++) {
			TiXmlElement* frameElem = new TiXmlElement( "frame" );
			frameElem->SetDoubleAttribute("length", framesLengths[i][j]);

			for (int k = 0; k < bodiesCount; k++) {
				TiXmlElement* bodyElem = new TiXmlElement( "body" );
				bodyElem->SetDoubleAttribute("x", frameAnimX[i][j][k]);
				bodyElem->SetDoubleAttribute("y", frameAnimY[i][j][k]);
				while (frameAnimAngle[i][j][k] > M_PI) {
					frameAnimAngle[i][j][k] -= 2 * M_PI;
				}
				while (frameAnimAngle[i][j][k] < -M_PI) {
					frameAnimAngle[i][j][k] += 2 * M_PI;
				}
				bodyElem->SetDoubleAttribute("angle", frameAnimAngle[i][j][k]);
				bodyElem->SetAttribute("show", frameAnimShow[i][j][k]);
				bodyElem->SetAttribute("layer", frameAnimLayer[i][j][k]);
				frameElem->LinkEndChild( bodyElem );
			}

			for (int k = 0; k < hotSpotsCount; k++) {
				TiXmlElement* bodyElem = new TiXmlElement( "hotspot" );
				bodyElem->SetDoubleAttribute("x", frameHotSpotX[i][j][k]);
				bodyElem->SetDoubleAttribute("y", frameHotSpotY[i][j][k]);
				while (frameHotSpotAngle[i][j][k] > M_PI) {
					frameHotSpotAngle[i][j][k] -= 2 * M_PI;
				}
				while (frameHotSpotAngle[i][j][k] < -M_PI) {
					frameHotSpotAngle[i][j][k] += 2 * M_PI;
				}
				bodyElem->SetDoubleAttribute("angle", frameHotSpotAngle[i][j][k]);
				bodyElem->SetAttribute("show", frameHotSpotShow[i][j][k]);
				frameElem->LinkEndChild( bodyElem );
			}
			animationElem->LinkEndChild( frameElem );

        }

        for (int j = 0; j < angleCounts[i]; j++) {
			TiXmlElement* frameElem = new TiXmlElement( "angle" );
			frameElem->SetDoubleAttribute("value", angles[i][j]);

			for (int k = 0; k < bodiesCount; k++) {
				TiXmlElement* bodyElem = new TiXmlElement( "body" );
				bodyElem->SetDoubleAttribute("x", angleAnimX[i][j][k]);
				bodyElem->SetDoubleAttribute("y", angleAnimY[i][j][k]);
				while (angleAnimAngle[i][j][k] > M_PI) {
					angleAnimAngle[i][j][k] -= 2 * M_PI;
				}
				while (angleAnimAngle[i][j][k] < -M_PI) {
					angleAnimAngle[i][j][k] += 2 * M_PI;
				}
				bodyElem->SetDoubleAttribute("angle", angleAnimAngle[i][j][k]);
				bodyElem->SetAttribute("show", angleAnimShow[i][j][k]);
				frameElem->LinkEndChild( bodyElem );
			}

			for (int k = 0; k < hotSpotsCount; k++) {
				TiXmlElement* bodyElem = new TiXmlElement( "hotspot" );
				bodyElem->SetDoubleAttribute("x", angleHotSpotX[i][j][k]);
				bodyElem->SetDoubleAttribute("y", angleHotSpotY[i][j][k]);
				while (angleHotSpotAngle[i][j][k] > M_PI) {
					angleHotSpotAngle[i][j][k] -= 2 * M_PI;
				}
				while (angleHotSpotAngle[i][j][k] < -M_PI) {
					angleHotSpotAngle[i][j][k] += 2 * M_PI;
				}
				bodyElem->SetDoubleAttribute("angle", angleHotSpotAngle[i][j][k]);
				bodyElem->SetAttribute("show", angleHotSpotShow[i][j][k]);
				frameElem->LinkEndChild( bodyElem );
			}
			animationElem->LinkEndChild( frameElem );

        }

        TiXmlElement* actionsRoot = new TiXmlElement( "actions" );
        actionsRoot->SetAttribute("count", actionsCounts[i]);
		for (int j = 0; j < actionsCounts[i]; j++) {
			TiXmlElement* actionElem = new TiXmlElement( "action" );
			actions[i][j]->save(actionElem);
			actionsRoot->LinkEndChild( actionElem );
        }

        animationElem->LinkEndChild( actionsRoot );
        characterElem->LinkEndChild( animationElem );
        //cxn->SetDoubleAttribute("timeout", 123.456);
	}
	doc.LinkEndChild( characterElem );
	doc.SaveFile(fn);
}

bool loadCharacter(char* fn)
{
	animations = new hgeAnimation*[256];
	actions = new CharacterAction**[256];

	bodiesCount = 0;

	animationX = new float[256];
	animationY = new float[256];
	animationAngle = new float[256];
	animationNames = new char*[256];

	hotSpotX = new float[256];
	hotSpotY = new float[256];
	hotSpotAngle = new float[256];
	hotSpotWidths = new float[256];
	hotSpotHeights = new float[256];

	printf("loading character %s ... \n", fn);
    TiXmlDocument doc(fn);
    bool loadOkay = doc.LoadFile();
    if (loadOkay) {
    	TiXmlElement* characterElem = doc.FirstChildElement("character");

    	if (characterElem->Attribute("height")) {
			characterHeight = atof(characterElem->Attribute("height"));
		}
		if (characterElem->Attribute("width")) {
			characterWidth = atof(characterElem->Attribute("width"));
		}

    	b2Vec2 origin(0, 0);
        TiXmlElement* element = characterElem->FirstChildElement("body");
        int i = 0;
        while (element) {
        	printf("loading body...\n");
        	float x = atof(element->Attribute("x"));
			float y = atof(element->Attribute("y"));
			float angle = atof(element->Attribute("angle"));

			char* animationName = (char*)element->Attribute("animation");
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
            element = element->NextSiblingElement("body");
        }
        bodiesCount = i;

        element = characterElem->FirstChildElement("hotspot");
        i = 0;
        while (element) {
        	printf("loading hotspot...\n");
        	float x = atof(element->Attribute("x"));
			float y = atof(element->Attribute("y"));
			float angle = atof(element->Attribute("angle"));
			float w = atof(element->Attribute("w"));
			float h = atof(element->Attribute("h"));

			//animation->SetHotSpot(-game->getPixelsPerMeter() * atof(element->Attribute("x")), -game->getPixelsPerMeter() * atof(element->Attribute("y")));
			b2PolygonShape* box = new b2PolygonShape();
			box->SetAsBox(0.5f * w, 0.5f * h);
			hotSpots[i] = new CharacterHotSpot(box, 1);

			hotSpotX[i] = x;
			hotSpotY[i] = y;
			hotSpotAngle[i] = angle;
			hotSpotWidths[i] = w;
			hotSpotHeights[i] = h;

			i++;
            element = element->NextSiblingElement("hotspot");
        }
        hotSpotsCount = i;

        TiXmlElement* animationElem = characterElem->FirstChildElement("animation");
        i = 0;
        while (animationElem) {
			int j = 0;
			TiXmlElement* bodyElem = animationElem->FirstChildElement("body");
			while (bodyElem) {
				animRotating[i][j] = atoi(bodyElem->Attribute("rotating"));
				j++;
				bodyElem = bodyElem->NextSiblingElement("body");
			}
        	j = 0;
        	TiXmlElement* frameElem = animationElem->FirstChildElement("frame");
        	while (frameElem) {
        		framesLengths[i][j] = atof(frameElem->Attribute("length"));
        		TiXmlElement* bodyElem = frameElem->FirstChildElement("body");
				int k = 0;
				while (bodyElem) {
					frameAnimX[i][j][k] = atof(bodyElem->Attribute("x"));
					frameAnimY[i][j][k] = atof(bodyElem->Attribute("y"));
					frameAnimAngle[i][j][k] = atof(bodyElem->Attribute("angle"));
					frameAnimShow[i][j][k] = atoi(bodyElem->Attribute("show"));
					frameAnimLayer[i][j][k] = atoi(bodyElem->Attribute("layer"));
					k++;
					bodyElem = bodyElem->NextSiblingElement("body");
				}

				bodyElem = frameElem->FirstChildElement("hotspot");
				k = 0;
				while (bodyElem) {
					frameHotSpotX[i][j][k] = atof(bodyElem->Attribute("x"));
					frameHotSpotY[i][j][k] = atof(bodyElem->Attribute("y"));
					frameHotSpotAngle[i][j][k] = atof(bodyElem->Attribute("angle"));
					frameHotSpotShow[i][j][k] = atoi(bodyElem->Attribute("show"));
					k++;
					bodyElem = bodyElem->NextSiblingElement("hotspot");
				}

        		j++;
				frameElem = frameElem->NextSiblingElement("frame");
        	}
        	framesCounts[i] = j;

        	j = 0;
        	frameElem = animationElem->FirstChildElement("angle");
        	while (frameElem) {
        		angles[i][j] = atof(frameElem->Attribute("value"));
        		TiXmlElement* bodyElem = frameElem->FirstChildElement("body");
				int k = 0;
				while (bodyElem) {
					angleAnimX[i][j][k] = atof(bodyElem->Attribute("x"));
					angleAnimY[i][j][k] = atof(bodyElem->Attribute("y"));
					angleAnimAngle[i][j][k] = atof(bodyElem->Attribute("angle"));
					angleAnimShow[i][j][k] = atoi(bodyElem->Attribute("show"));
					k++;
					bodyElem = bodyElem->NextSiblingElement("body");
				}

				bodyElem = frameElem->FirstChildElement("hotspot");
				k = 0;
				while (bodyElem) {
					angleHotSpotX[i][j][k] = atof(bodyElem->Attribute("x"));
					angleHotSpotY[i][j][k] = atof(bodyElem->Attribute("y"));
					angleHotSpotAngle[i][j][k] = atof(bodyElem->Attribute("angle"));
					angleHotSpotShow[i][j][k] = atoi(bodyElem->Attribute("show"));
					k++;
					bodyElem = bodyElem->NextSiblingElement("hotspot");
				}

        		j++;
				frameElem = frameElem->NextSiblingElement("angle");
        	}
        	angleCounts[i] = j;

        	TiXmlElement* actionsRoot = animationElem->FirstChildElement("actions");
        	actions[i] = new CharacterAction*[256];
			if (actionsRoot) {
				printf("loading actions\n");
				TiXmlElement* actionElem = actionsRoot->FirstChildElement("action");
				j = 0;
				while (actionElem) {
					actions[i][j] = new CharacterAction(actionElem);
					j++;
					actionElem = actionElem->NextSiblingElement("action");
					printf("action loaded\n");
				}
				actionsCounts[i] = j;
			} else {
				actionsCounts[i] = 0;
			}

        	i++;
            animationElem = animationElem->NextSiblingElement("animation");
        }
		animationsCount = i;
    }
    printf("construction loaded\n");
}

bool saveCharacterButtonClick(hgeGUIMenuItem* sender)
{
	saveCharacter("test.xml");
}

bool loadCharacterButtonClick(hgeGUIMenuItem* sender)
{
	loadCharacter("test.xml");
}


void setFrame(int frame, bool resetTime)
{
	float time = 0;
	for (int i = 0; i < frame; i++) {
		time += framesLengths[currentAnimation][i];
	}
	if (resetTime)
		currentTime = time;
	currentFrame = frame;
	for (int i = 0; i < bodiesCount; i++) {
		//animations[i]->SetZ(1.0f - 0.001 * frameAnimLayer[currentAnimation][currentFrame][i]);
		//animationX[i] = frameAnimX[currentAnimation][currentFrame][i];
		//animationY[i] = frameAnimY[currentAnimation][currentFrame][i];
		//animationAngle[i] = frameAnimAngle[currentAnimation][currentFrame][i];
	}
}

void setFrame(int frame)
{
	setFrame(frame, false);
}

bool insertFrameButtonClick(hgeGUIMenuItem* sender)
{
	for (int i = 0; i < bodiesCount; i++) {
		frameAnimX[currentAnimation][framesCounts[currentAnimation]][i] = frameAnimX[currentAnimation][currentFrame][i];
		frameAnimY[currentAnimation][framesCounts[currentAnimation]][i] = frameAnimY[currentAnimation][currentFrame][i];
		frameAnimAngle[currentAnimation][framesCounts[currentAnimation]][i] = frameAnimAngle[currentAnimation][currentFrame][i];
		frameAnimShow[currentAnimation][framesCounts[currentAnimation]][i] = frameAnimShow[currentAnimation][currentFrame][i];
		frameAnimLayer[currentAnimation][framesCounts[currentAnimation]][i] = frameAnimLayer[currentAnimation][currentFrame][i];
		frameAnimHideLayer[currentAnimation][framesCounts[currentAnimation]][i] = frameAnimHideLayer[currentAnimation][currentFrame][i];
	}
	framesLengths[currentAnimation][framesCounts[currentAnimation]] = 0.5f;
	for (int i = 0; i < hotSpotsCount; i++) {
		frameHotSpotX[currentAnimation][framesCounts[currentAnimation]][i] = frameHotSpotX[currentAnimation][currentFrame][i];
		frameHotSpotY[currentAnimation][framesCounts[currentAnimation]][i] = frameHotSpotY[currentAnimation][currentFrame][i];
		frameHotSpotAngle[currentAnimation][framesCounts[currentAnimation]][i] = frameHotSpotAngle[currentAnimation][currentFrame][i];
		frameHotSpotShow[currentAnimation][framesCounts[currentAnimation]][i] = frameHotSpotShow[currentAnimation][currentFrame][i];
	}

	currentFrame = framesCounts[currentAnimation];
	framesCounts[currentAnimation] += 1;
}

bool insertAngleButtonClick(hgeGUIMenuItem* sender)
{
	for (int i = 0; i < bodiesCount; i++) {
		angleAnimX[currentAnimation][angleCounts[currentAnimation]][i] = angleAnimX[currentAnimation][currentFrame][i];
		angleAnimY[currentAnimation][angleCounts[currentAnimation]][i] = angleAnimY[currentAnimation][currentFrame][i];
		angleAnimAngle[currentAnimation][angleCounts[currentAnimation]][i] = angleAnimAngle[currentAnimation][currentFrame][i];
		angleAnimShow[currentAnimation][angleCounts[currentAnimation]][i] = angleAnimShow[currentAnimation][currentFrame][i];
	}
	angles[currentAnimation][angleCounts[currentAnimation]] = angles[currentAnimation][currentFrame];
	for (int i = 0; i < hotSpotsCount; i++) {
		angleHotSpotX[currentAnimation][angleCounts[currentAnimation]][i] = angleHotSpotX[currentAnimation][currentFrame][i];
		angleHotSpotY[currentAnimation][angleCounts[currentAnimation]][i] = angleHotSpotY[currentAnimation][currentFrame][i];
		angleHotSpotAngle[currentAnimation][angleCounts[currentAnimation]][i] = angleHotSpotAngle[currentAnimation][currentFrame][i];
		//angleHotSpotShow[currentAnimation][angleCounts[currentAnimation]][i] = angleHotSpotShow[currentAnimation][currentFrame][i];
	}

	currentFrame = angleCounts[currentAnimation];
	angleCounts[currentAnimation] += 1;
}

bool nextFrameButtonClick(hgeGUIMenuItem* sender)
{
	currentFrame += 1;
	if (currentFrame >= framesCounts[currentAnimation]) {
		currentFrame = 0;
	}

	setFrame(currentFrame);
}

bool prevFrameButtonClick(hgeGUIMenuItem* sender)
{
	currentFrame -= 1;
	if (currentFrame < 0) {
		currentFrame = framesCounts[currentAnimation] - 1;
	}

	setFrame(currentFrame);
}

bool increaseLengthButtonClick(hgeGUIMenuItem* sender)
{
	framesLengths[currentAnimation][currentFrame] += 0.1f;
}
bool decreaseLengthButtonClick(hgeGUIMenuItem* sender)
{
	if (framesLengths[currentAnimation][currentFrame] > 0.15)
		framesLengths[currentAnimation][currentFrame] -= 0.1f;
}

void setAnimation(int animation)
{
	currentAnimation = animation;
	printf("animation %i\n", animation);
	setFrame(0, true);
}

bool insertAnimationButtonClick(hgeGUIMenuItem* sender)
{
	currentAnimation = animationsCount;
	animationsCount += 1;
	framesCounts[currentAnimation] = 1;
	currentFrame = 0;

	for (int i = 0; i < bodiesCount; i++) {
		frameAnimShow[currentAnimation][currentFrame][i] = false;
		frameAnimHideLayer[currentAnimation][currentFrame][i] = false;
		frameAnimX[currentAnimation][currentFrame][i] = 0;
		frameAnimY[currentAnimation][currentFrame][i] = 0;
		frameAnimLayer[currentAnimation][currentFrame][i] = i;
		frameAnimAngle[currentAnimation][currentFrame][i] = 0;
	}
	for (int i = 0; i < hotSpotsCount; i++) {
		frameHotSpotX[currentAnimation][currentFrame][i] = 0;
		frameHotSpotY[currentAnimation][currentFrame][i] = 0;
		frameHotSpotAngle[currentAnimation][currentFrame][i] = 0;
		frameHotSpotShow[currentAnimation][currentFrame][i] = false;
	}
}

bool nextAnimationButtonClick(hgeGUIMenuItem* sender)
{
	currentAnimation += 1;
	if (currentAnimation >= animationsCount) {
		currentAnimation = 0;
	}

	setAnimation(currentAnimation);
}

bool prevAnimationButtonClick(hgeGUIMenuItem* sender)
{
	currentAnimation -= 1;
	if (currentAnimation < 0) {
		currentAnimation = animationsCount - 1;
	}

	setAnimation(currentAnimation);
}


bool setTabButtonClick(hgeGUIMenuItem* sender)
{
	currentTab = atoi(sender->getTitle()) - 1;
}

bool addHotspotButtonClick(hgeGUIMenuItem* sender)
{
	mainWindow->Hide();
	hotspotTypesWindow->Show();
	mode = MODE_ADD_HOTSPOT;
}

bool boxHotspotButtonClick(hgeGUIMenuItem* sender)
{
	mainWindow->Show();
	hotspotTypesWindow->Hide();
	mode = MODE_ADD_HOTSPOT_BOX_STEP1;
}

bool animDetailsButtonClick(hgeGUIMenuItem* sender)
{
	resetMode();
}

bool anglesModeButtonClick(hgeGUIMenuItem* sender)
{
	resetMode();
	editMode = EDIT_MODE_ANGLES;
}
bool framesModeButtonClick(hgeGUIMenuItem* sender)
{
	resetMode();
	editMode = EDIT_MODE_FRAMES;
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

	float layerHeight = 50;
	if (bodiesCount > 16) {
		layerHeight = 800 / bodiesCount;
	}

	if (game->getHge()->Input_KeyUp(HGEK_TAB)) {
		drawBoxes = !drawBoxes;
	}

	float time = 0;
	int frame = 0;


	switch(mode) {
		case MODE_DEFAULT:
			if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && !game->getHge()->Input_GetKeyState(HGEK_SHIFT)) {
				if (x < 50 && y > 110 && y < 110 + layerHeight * bodiesCount) {
					selectedLayer = floor((bodiesCount * layerHeight - (y - 110)) / layerHeight);
					mode = MODE_LAYER_DRAG;
				}
				if (y > 100 && x > 50) {
					selectedBody = getPointedBody(x, y);
					selectedHotSpot = getPointedHotSpot(x, y);
					if (selectedBody != -1) {
						if (frameAnimShow[currentAnimation][currentFrame][selectedBody]) {
							selectedBodyX = game->screenX(animX(selectedBody));
							selectedBodyY = game->screenY(animY(selectedBody));
							selectedBodyAngle = animAngle(selectedBody);
							dragOffsetX = x - selectedBodyX;
							dragOffsetY = y - selectedBodyY;
						} else {
							selectedBodyX = animationX[selectedBody] + 1300;
							selectedBodyY = animationY[selectedBody] - 1000 * currentTab + 450;
							selectedBodyAngle = animationAngle[selectedBody];
							dragOffsetX = x - selectedBodyX;
							dragOffsetY = y - selectedBodyY;
							//mainWindow->Hide();
							//mode = MODE_ANIM_DRAG;
						}
						mode = MODE_ANIM_DRAG;
					} else if (selectedHotSpot != -1) {
						if (hsShow(selectedHotSpot)) {
							selectedHotSpotX = game->screenX(hsX(selectedHotSpot));
							selectedHotSpotY = game->screenY(hsY(selectedHotSpot));
							selectedHotSpotAngle = hsAngle(selectedHotSpot);
							dragOffsetX = x - selectedHotSpotX;
							dragOffsetY = y - selectedHotSpotY;
						} else {
							selectedHotSpotX = hotSpotX[selectedHotSpot] + 1300;
							selectedHotSpotY = hotSpotY[selectedHotSpot] - 1000 * currentTab + 450;
							selectedHotSpotAngle = hotSpotAngle[selectedHotSpot];
							dragOffsetX = x - selectedHotSpotX;
							dragOffsetY = y - selectedHotSpotY;
							//mainWindow->Hide();
							//mode = MODE_ANIM_DRAG;
						}
						mode = MODE_HOTSPOT_DRAG;
					}
				}
				if (y < 50 && x < 1300) {
					int anim = floor(x / 50);
					if (anim < animationsCount)
						setAnimation(anim);
				}
				if (y > 50 && y < 100 && x < 1300) {
					if (editMode == EDIT_MODE_FRAMES) {
						float leftX = 0;
						float rightX = 0;
						for (int i = 0; i < framesCounts[currentAnimation]; i++) {
							rightX = leftX + 100 * framesLengths[currentAnimation][i];
							if (x > leftX && x < rightX) {
								setFrame(i, true);
							}
							leftX = rightX;
						}
						if (x > leftX && x < leftX + 50) {
							insertFrameButtonClick(NULL);
						}
					}
					if (editMode == EDIT_MODE_ANGLES) {
						float leftX = 0;
						float rightX = 0;
						for (int i = 0; i < angleCounts[currentAnimation]; i++) {
							rightX = leftX + 50;
							if (x > leftX && x < rightX) {
								setFrame(i, true);
							}
							leftX = rightX;
						}
						if (x > angleCounts[currentAnimation] * 50 && x < angleCounts[currentAnimation] * 50 + 50) {
							insertAngleButtonClick(NULL);
						}
					}
				}
				if (editMode == EDIT_MODE_ANGLES) {
					if ((b2Vec2(worldX, worldY) - b2Vec2(cos(angles[currentAnimation][currentFrame]) * 2.9f, sin(angles[currentAnimation][currentFrame]) * 2.9f)).Length() < 0.1f) {
						mode = MODE_ANGLE_DRAG;
					}
				}

				if (sqrt(pow(x - game->screenX(characterWidth * 0.5), 2) + pow(y - game->screenY(-characterHeight * 0.5), 2)) < 5) {
                    ///Если мышка возле угла прямоугольника, показывающего размер, то будем менять размер
                    mode = MODE_RESIZE_CHARACTER;
                }
			}
			if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && game->getHge()->Input_GetKeyState(HGEK_SHIFT)) {
				selectedBody = getPointedBody(x, y);
				selectedHotSpot = getPointedHotSpot(x, y);
				if (selectedBody != -1 && frameAnimShow[currentAnimation][currentFrame][selectedBody]) {
					//mainWindow->Hide();
					dragOffsetX = worldX;
					dragOffsetY = worldY;
					selectedBodyX = game->screenX(animX(selectedBody));
					selectedBodyY = game->screenY(animY(selectedBody));
					mode = MODE_ANIM_ROTATE;
				} else if (selectedHotSpot != -1 && hsShow(selectedHotSpot)) {
					dragOffsetX = worldX;
					dragOffsetY = worldY;
					selectedHotSpotX = game->screenX(hsX(selectedHotSpot));
					selectedHotSpotY = game->screenY(hsY(selectedHotSpot));
					mode = MODE_HOTSPOT_ROTATE;
				}
			}
			if (game->getHge()->Input_KeyDown(HGEK_RBUTTON)) {
				if (x < 50 && y > 110 && y < 110 + layerHeight * bodiesCount) {
					int layer = floor((bodiesCount * layerHeight - (y - 110)) / layerHeight);
					frameAnimHideLayer[currentAnimation][currentFrame][frameAnimLayer[currentAnimation][currentFrame][layer]]
						= !frameAnimHideLayer[currentAnimation][currentFrame][frameAnimLayer[currentAnimation][currentFrame][layer]];
					;
				} else {
					selectedBody = getPointedBody(x, y);
					if (selectedBody != -1) {
						mode = MODE_ANIM_CONTEXT_MENU;
						animContextMenuWindow->Move(x, y);
						animContextMenuWindow->Show();
					} else {
						dragOffsetX = x;
						dragOffsetY = y;
						mode = MODE_CAMERA_MOVE;
						selectedBody = -1;
						selectedHotSpot = -1;
					}
					//mainWindow->Hide();
				}
			}
			if (game->getHge()->Input_KeyUp(HGEK_SPACE)) {
				game->getGUI()->SetFocus(0);
				mode = MODE_PLAYING;
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
		case MODE_ANIM_DRAG:
			if (selectedBody != -1) {
				float currentX = x - dragOffsetX;
				float currentY = y - dragOffsetY;
				selectedBodyX = currentX;
				selectedBodyY = currentY;
				//frameAnimX[currentAnimation][currentFrame][selectedBody] = currentX;
				//frameAnimY[currentAnimation][currentFrame][selectedBody] = currentY;

				if (game->getHge()->Input_KeyUp(HGEK_UP)) {
					frameAnimLayer[currentAnimation][currentFrame][selectedBody] += 1;
					if (frameAnimLayer[currentAnimation][currentFrame][selectedBody] > 1000)
						frameAnimLayer[currentAnimation][currentFrame][selectedBody] = 1000;
					//animations[selectedBody]->SetZ(1.0f - 0.001 * frameAnimLayer[currentAnimation][currentFrame][selectedBody]);
					printf("layer %f\n", animations[selectedBody]->GetZ());
				}
				if (game->getHge()->Input_KeyUp(HGEK_DOWN)) {
					frameAnimLayer[currentAnimation][currentFrame][selectedBody] -= 1;
					if (frameAnimLayer[currentAnimation][currentFrame][selectedBody] < 0)
						frameAnimLayer[currentAnimation][currentFrame][selectedBody] = 0;
					//animations[selectedBody]->SetZ(1.0f - 0.001 * frameAnimLayer[currentAnimation][currentFrame][selectedBody]);
					printf("layer %f\n", animations[selectedBody]->GetZ());
				}
				if (selectedBodyX < 1300) {
					setAnimX(selectedBody, game->worldX(selectedBodyX));
					setAnimY(selectedBody, game->worldY(selectedBodyY));
					setAnimAngle(selectedBody, selectedBodyAngle);
					setAnimShow(selectedBody, true);
				} else {
					setAnimShow(selectedBody, false);
					animationX[selectedBody] = selectedBodyX - 1300;
					animationY[selectedBody] = selectedBodyY + 1000 * currentTab - 450;
					animationAngle[selectedBody] = selectedBodyAngle;
					if (animationY[selectedBody] < 1000 * currentTab)
						animationY[selectedBody] = 1000 * currentTab;
				}
				if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
					resetMode();
				}
			}
            break;
		case MODE_ANIM_ROTATE:
			if (selectedBody != -1) {
				if (sqrt(pow(dragOffsetX - worldX, 2) + pow(dragOffsetY - worldY, 2)) < 0.1f) {
					dragOffsetAngle = atan2(worldY - dragOffsetY, worldX - dragOffsetX) - frameAnimAngle[currentAnimation][currentFrame][selectedBody];
				} else {
					float angle = atan2(worldY - dragOffsetY, worldX - dragOffsetX) - dragOffsetAngle;
					float offsetAngle = atan2(
						dragOffsetY - animY(selectedBody),
						dragOffsetX - animX(selectedBody)
					);
					float len = sqrt(
						pow(dragOffsetX - animX(selectedBody), 2) +
						pow(dragOffsetY - animY(selectedBody), 2)
					);

					float newOffsetX = animX(selectedBody) + cos(angle - selectedBodyAngle + offsetAngle) * len;
					float newOffsetY = animY(selectedBody) + sin(angle - selectedBodyAngle + offsetAngle) * len;

					float transX = newOffsetX - dragOffsetX;
					float transY = newOffsetY - dragOffsetY;
					//printf("%f %f %f\n", len, transX, transY);

					setAnimX(selectedBody, animX(selectedBody) - transX);
					setAnimY(selectedBody, animY(selectedBody) - transY);
					setAnimAngle(selectedBody, angle);

					selectedBodyX = game->screenX(animX(selectedBody));
					selectedBodyY = game->screenY(animY(selectedBody));
					selectedBodyAngle = angle;
				}
			}
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON) || game->getHge()->Input_KeyUp(HGEK_SHIFT)) {
				resetMode();
			}
			break;
		case MODE_INSERT_ANIM:
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
				animationX[bodiesCount] = x - 1300;
				animationY[bodiesCount] = y - 450 + 1000 * currentTab;
				animations[bodiesCount] = beingInsertedAnim;

				frameAnimX[currentAnimation][currentFrame][bodiesCount] = 0;
				frameAnimY[currentAnimation][currentFrame][bodiesCount] = 0;
				frameAnimAngle[currentAnimation][currentFrame][bodiesCount] = 0;
				frameAnimShow[currentAnimation][currentFrame][bodiesCount] = false;
				frameAnimHideLayer[currentAnimation][currentFrame][bodiesCount] = false;
				frameAnimLayer[currentAnimation][currentFrame][bodiesCount] = bodiesCount;

				animationNames[bodiesCount] = beingInsertedAnimName;

				bodiesCount++;
				resetMode();
			}
			break;
		case MODE_LAYER_DRAG:
			if (bodiesCount * layerHeight - (y - 110) > 0 && bodiesCount * layerHeight - (y - 110) < selectedLayer * layerHeight) {
				int oldAnimNum = frameAnimLayer[currentAnimation][currentFrame][selectedLayer];
				frameAnimLayer[currentAnimation][currentFrame][selectedLayer] = frameAnimLayer[currentAnimation][currentFrame][selectedLayer - 1];
				frameAnimLayer[currentAnimation][currentFrame][selectedLayer - 1] = oldAnimNum;
				selectedLayer -= 1;
			}
			if (bodiesCount * layerHeight - (y - 110) < bodiesCount * layerHeight && bodiesCount * layerHeight - (y - 110) > selectedLayer * layerHeight + layerHeight) {
				int oldAnimNum = frameAnimLayer[currentAnimation][currentFrame][selectedLayer];
				frameAnimLayer[currentAnimation][currentFrame][selectedLayer] = frameAnimLayer[currentAnimation][currentFrame][selectedLayer + 1];
				frameAnimLayer[currentAnimation][currentFrame][selectedLayer + 1] = oldAnimNum;
				selectedLayer += 1;
			}
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
				resetMode();
			}
			break;
		case MODE_PLAYING:
			currentTime += game->getHge()->Timer_GetDelta();
			for (int i = 0; i < framesCounts[currentAnimation]; i++) {
				if (currentTime > time) {
					frame = i;
					frameProgress = (currentTime - time) / framesLengths[currentAnimation][i];
				}
				time += framesLengths[currentAnimation][i];
			}
			setFrame(frame);
			if (currentTime > time)
				currentTime = 0;

			if (game->getHge()->Input_KeyUp(HGEK_SPACE)) {
				game->getGUI()->SetFocus(0);
				resetMode();
			}
			break;

		case MODE_ADD_HOTSPOT_BOX_STEP1:
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
				dragOffsetX = worldX;
				dragOffsetY = worldY;
				mode = MODE_ADD_HOTSPOT_BOX_STEP2;
			}
			break;
		case MODE_ADD_HOTSPOT_BOX_STEP2:
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
				float left, right, top, bottom;
				if (worldX < dragOffsetX) {
					left = worldX;
					right = dragOffsetX;
				} else {
					left = dragOffsetX;
					right = worldX;
				}
				if (worldY < dragOffsetY) {
					top = worldY;
					bottom = dragOffsetY;
				} else {
					top = dragOffsetY;
					bottom = worldY;
				}

				hotSpotWidths[hotSpotsCount] = right - left;
				hotSpotHeights[hotSpotsCount] = bottom - top;
				hotSpotX[hotSpotsCount] = 150;
				hotSpotY[hotSpotsCount] = 250;
				frameHotSpotX[currentAnimation][currentFrame][hotSpotsCount] = 0.5f * (right + left);
				frameHotSpotY[currentAnimation][currentFrame][hotSpotsCount] = 0.5f * (bottom + top);
				frameHotSpotAngle[currentAnimation][currentFrame][hotSpotsCount] = 0;
				frameHotSpotShow[currentAnimation][currentFrame][hotSpotsCount] = true;

				b2PolygonShape* box = new b2PolygonShape();
				box->SetAsBox(0.5f * (right - left), 0.5f * (bottom - top));
				hotSpots[hotSpotsCount] = new CharacterHotSpot(box, 1);

				hotSpotsCount++;
				resetMode();
			}
			break;
		case MODE_HOTSPOT_DRAG:
			if (selectedHotSpot != -1) {
				float currentX = x - dragOffsetX;
				float currentY = y - dragOffsetY;
				selectedHotSpotX = currentX;
				selectedHotSpotY = currentY;

				if (selectedHotSpotX < 1300) {
					setHsX(selectedHotSpot, game->worldX(selectedHotSpotX));
					setHsY(selectedHotSpot, game->worldY(selectedHotSpotY));
					setHsAngle(selectedHotSpot, selectedHotSpotAngle);
					setHsShow(selectedHotSpot, true);

				} else {
					setHsShow(selectedHotSpot, false);
					hotSpotX[selectedHotSpot] = selectedHotSpotX - 1300;
					hotSpotY[selectedHotSpot] = selectedHotSpotY + 1000 * currentTab - 450;
					hotSpotAngle[selectedHotSpot] = selectedHotSpotAngle;
					if (hotSpotY[selectedHotSpot] < 1000 * currentTab)
						hotSpotY[selectedHotSpot] = 1000 * currentTab;

				}

				if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
					resetMode();
				}
			}
            break;
		case MODE_HOTSPOT_ROTATE:
			if (selectedHotSpot != -1) {
				if (sqrt(pow(dragOffsetX - worldX, 2) + pow(dragOffsetY - worldY, 2)) < 0.1f) {
					dragOffsetAngle = atan2(worldY - dragOffsetY, worldX - dragOffsetX) - hsAngle(selectedHotSpot);
				} else {
					float angle = atan2(worldY - dragOffsetY, worldX - dragOffsetX) - dragOffsetAngle;
					float offsetAngle = atan2(
						dragOffsetY - hsY(selectedHotSpot),
						dragOffsetX - hsX(selectedHotSpot)
					);
					float len = sqrt(
						pow(dragOffsetX - hsX(selectedHotSpot), 2) +
						pow(dragOffsetY - hsY(selectedHotSpot), 2)
					);

					float newOffsetX = hsX(selectedHotSpot) + cos(angle - selectedHotSpotAngle + offsetAngle) * len;
					float newOffsetY = hsY(selectedHotSpot) + sin(angle - selectedHotSpotAngle + offsetAngle) * len;

					float transX = newOffsetX - dragOffsetX;
					float transY = newOffsetY - dragOffsetY;
					//printf("%f %f %f\n", len, transX, transY);

					setHsX(selectedHotSpot, hsX(selectedHotSpot) - transX);
					setHsY(selectedHotSpot, hsY(selectedHotSpot) - transY);
					setHsAngle(selectedHotSpot, angle);

					selectedHotSpotX = game->screenX(hsX(selectedHotSpot));
					selectedHotSpotY = game->screenY(hsY(selectedHotSpot));
					selectedHotSpotAngle = angle;
				}
			}
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON) || game->getHge()->Input_KeyUp(HGEK_SHIFT)) {
				resetMode();
			}
			break;
		case MODE_ANIM_CONTEXT_MENU:
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
				resetMode();
			}
			break;
		case MODE_ANGLE_DRAG:
			angles[currentAnimation][currentFrame] = atan2f(worldY, worldX);
			if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
				resetMode();
			}
			break;
        case MODE_RESIZE_CHARACTER:
            characterWidth = worldX * 2;
            characterHeight = -worldY * 2;
            if (characterWidth < 0)
                characterWidth = 0;
            if (characterHeight < 0)
                characterHeight = 0;
            if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
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

//	game->getHge()->Gfx_RenderLine(game->screenX(-0.5), game->screenY(0), game->screenX(-0.5), game->screenY(-characterHeight), 0x55000000);
//	game->getHge()->Gfx_RenderLine(game->screenX(-0.5), game->screenY(-characterHeight), game->screenX(0.5), game->screenY(-characterHeight), 0x55000000);
//	game->getHge()->Gfx_RenderLine(game->screenX(0.5), game->screenY(-characterHeight), game->screenX(0.5), game->screenY(0), 0x55000000);
//	game->drawArc(game->screenX(0), game->screenY(0), 50 * game->getScaleFactor(), M_PI, 2 * M_PI, 0x55000000, 0);
//	game->getHge()->Gfx_RenderLine(game->screenX(-10), game->screenY(0.5), game->screenX(10), game->screenY(0.5), 0x55000000);
    ///Положение мышки на экране и в мире
    float x, y;
    game->getHge()->Input_GetMousePos(&x, &y);
    float worldX = game->worldX(x);
	float worldY = game->worldY(y);
	///Добавление анимации в пул
	float insertX = x; float insertY = y;
	if (insertX < 1300) insertX = 1300; if (insertY < 450) insertY = 450;

    ///Прямоугольник, показывающий размер персонажа в мире
	game->drawRect(game->screenX(0), game->screenY(0), characterWidth * 0.5 * game->getFullScale(), characterHeight * 0.5 * game->getFullScale(), 0, 0x55000000, 0);
	if (sqrt(pow(x - game->screenX(characterWidth * 0.5), 2) + pow(y - game->screenY(-characterHeight * 0.5), 2)) < 5) {
        resizeIcon->Render(x, y);///Нарисуем иконку, если мышка возле правого верхнего угла
	}

	for (int index = 0; index < bodiesCount; index++) {
		int i = animLayer(index);
		//if (frameAnimShow[currentAnimation][currentFrame][i] && !frameAnimHideLayer[currentAnimation][currentFrame][i]) {
		if (animShow(i) && !animHideLayer(i)) {
			DWORD color = 0xFFFFFFFF;
			if (mode != MODE_PLAYING && (
                    (editMode == EDIT_MODE_FRAMES && animRotating[currentAnimation][i]) ||
                    (editMode == EDIT_MODE_ANGLES && !animRotating[currentAnimation][i])
                )
			) {
				color = 0xAAAAAAAA;
			}
			animations[i]->SetColor(color);
			animations[i]->RenderEx(
				game->screenX(animX(i)),
				game->screenY(animY(i)),
				animAngle(i),
				game->getScaleFactor(),
				game->getScaleFactor()
			);

			color = 0xFFAA0000;
			if (i == selectedBody) {
				color = 0xFFFF0000;
			}
			if (index == selectedLayer) {
				color = 0xFF00FF00;
			}
			if (drawBoxes && mode != MODE_PLAYING) {
				hgeRect* bb = new hgeRect();
				animations[i]->GetBoundingBoxEx(
					game->screenX(animX(i)),
					game->screenY(animY(i)),
					animAngle(i),
					game->getScaleFactor(),
					game->getScaleFactor(),
					bb
				);

				game->getHge()->Gfx_RenderLine(bb->x1, bb->y1, bb->x2, bb->y1, color);
				game->getHge()->Gfx_RenderLine(bb->x2, bb->y1, bb->x2, bb->y2, color);
				game->getHge()->Gfx_RenderLine(bb->x2, bb->y2, bb->x1, bb->y2, color);
				game->getHge()->Gfx_RenderLine(bb->x1, bb->y2, bb->x1, bb->y1, color);
			}
		} else {
		}
	}

	for (int i = 0; i < hotSpotsCount; i++) {
		if (hsShow(i)) {
			b2Transform trans(
				b2Vec2(hsX(i), hsY(i)),
				b2Rot(hsAngle(i))
			);
			DWORD color = 0xFFFF0000; DWORD fillcolor = 0xAAFF0000;
			if (mode != MODE_PLAYING && (
                    (editMode == EDIT_MODE_FRAMES && hotSpotRotating[currentAnimation][i]) ||
                    (editMode == EDIT_MODE_ANGLES && !hotSpotRotating[currentAnimation][i])
                )
			) {
				color = 0xAAAA0000; fillcolor = 0x55AA0000;
			}
			game->drawPoly(
				(b2PolygonShape*)hotSpots[i]->getShape(),
				trans,
				-game->getPixelsPerMeter() * game->getScaleFactor() * game->getCameraPos(),
				game->getPixelsPerMeter() * game->getScaleFactor(),
				color, fillcolor
			);
		}
	}

	//game->getHge()->Gfx_RenderLine(game->screenX(0) - 10, game->screenY(0) - 10, game->screenX(0) + 10, game->screenY(0) + 10, 0xFF000000);
	//game->getHge()->Gfx_RenderLine(game->screenX(0) - 10, game->screenY(0) + 10, game->screenX(0) + 10, game->screenY(0) - 10, 0xFF000000);


	//game->getHge()->Gfx_RenderQuad(&grayQuad);

//	if (selectedBody != -1) {
//		float scale = 1;
//		if (selectedBodyX < 1300) {
//			scale = game->getScaleFactor();
//		}
//		animations[selectedBody]->RenderEx(
//			selectedBodyX,
//			selectedBodyY,
//			selectedBodyAngle,
//			scale,
//			scale
//		);
//	}
//	if (selectedHotSpot != -1) {
//		float scale = game->getPixelsPerMeter();
//		if (selectedHotSpotX < 1300) {
//			scale *= game->getScaleFactor();
//		}
//		b2Transform trans(b2Vec2_zero, b2Rot(selectedHotSpotAngle));
//		game->drawPoly(
//			(b2PolygonShape*)hotSpots[selectedHotSpot]->getShape(),
//			trans,
//			b2Vec2(selectedHotSpotX, selectedHotSpotY),
//			scale,
//			0xFFFF0000, 0xAAFF0000
//		);
//	}

	switch (mode) {
		case MODE_INSERT_ANIM:
            break;
		case MODE_ANIM_ROTATE:
		case MODE_HOTSPOT_ROTATE:
			game->getHge()->Gfx_RenderLine(game->screenX(dragOffsetX), game->screenY(dragOffsetY), x, y, 0xFFffa500);
			break;
		case MODE_ADD_HOTSPOT_BOX_STEP2:
			game->drawRect(game->screenX(dragOffsetX), game->screenY(dragOffsetY), x, y, 0xFFFF0000, 0xAAFF0000);
	}

	game->drawRect(0, 0, 1300, 50, 0xFF000000, 0xFFFFFFFF);
	for (int i = 0; i < animationsCount; i++) {
		DWORD color = 0xFF000000;
		if (i == currentAnimation)
			color = 0xFFFF0000;
		game->drawRect(1 + 50 * i, 1, 50 + 50 * i, 49, color, 0);
		for (int index = 0; index < bodiesCount; index++) {
			int j = frameAnimLayer[i][0][index];
			if (frameAnimShow[i][0][j]) {
				animations[j]->RenderEx(
					25 + i * 50 + frameAnimX[i][0][j] * 10,
					25 + frameAnimY[i][0][j] * 10,
					frameAnimAngle[i][0][j],
					10 / game->getPixelsPerMeter(),
					10 / game->getPixelsPerMeter()
				);
			} else {
			}
		}
	}
	game->drawRect(0, 50, 1300, 100, 0xFF000000, 0xFFFFFFFF);

	if (editMode == EDIT_MODE_FRAMES) {
		float leftX = 0;
		float midX = 0;
		float rightX = 0;
		for (int i = 0; i < framesCounts[currentAnimation]; i++) {
			DWORD color = 0xFF000000;
			if (i == currentFrame)
				color = 0xFFFF0000;
			midX = leftX + 50 * framesLengths[currentAnimation][i];
			rightX = leftX + 100 * framesLengths[currentAnimation][i];
			game->drawRect(1 + leftX, 51, rightX, 99, color, 0xFFFFFFFF);

			for (int index = 0; index < bodiesCount; index++) {
				int j = frameAnimLayer[currentAnimation][i][index];
				if (frameAnimShow[currentAnimation][i][j]) {
					animations[j]->RenderEx(
						midX + frameAnimX[currentAnimation][i][j] * 10,
						75 + frameAnimY[currentAnimation][i][j] * 10,
						frameAnimAngle[currentAnimation][i][j],
						10 / game->getPixelsPerMeter(),
						10 / game->getPixelsPerMeter()
					);
				}
			}
			leftX = rightX;
		}
		game->drawRect(1 + leftX, 51, 50 + leftX, 99, 0xFF00AA00, 0xFFFFFFFF);
		game->drawRect(6 + leftX, 73, 44 + leftX, 77, 0xFF00AA00, 0xFF00AA00);
		game->drawRect(23 + leftX, 56, 27 + leftX, 94, 0xFF00AA00, 0xFF00AA00);
		int len = 0;
		for (int i = 0; i < 130; i++) {
			len = 5;
			if ((i % 10) == 0)
				len = 10;
			game->getHge()->Gfx_RenderLine(i * 10, 100, i * 10, 100 + len, 0xFF888888);
		}
		game->getHge()->Gfx_RenderLine(currentTime * 100, 100, currentTime * 100, 110, 0xFFFF0000);
	}
	if (editMode == EDIT_MODE_ANGLES) {
		for (int i = 0; i < angleCounts[currentAnimation]; i++) {
			DWORD color = 0xFF000000;
			if (i == currentFrame)
				color = 0xFFFF0000;

			game->drawRect(0 + i * 50, 51, 50 + i * 50, 99, color, 0xFFFFFFFF);

			b2Vec2* arrow = new b2Vec2[4];
			arrow[0] = b2Vec2(25 + i * 50, 75);
			arrow[1] = b2Vec2(25 + i * 50 + cos(angles[currentAnimation][i] + 0.25f) * 20, 75 + sin(angles[currentAnimation][i] + 0.25f) * 20);
			arrow[2] = b2Vec2(25 + i * 50 + cos(angles[currentAnimation][i]) * 24, 75 + sin(angles[currentAnimation][i]) * 24);
			arrow[3] = b2Vec2(25 + i * 50 + cos(angles[currentAnimation][i] - 0.25f) * 20, 75 + sin(angles[currentAnimation][i] - 0.25f) * 20);
			game->DrawPolygonScreen(arrow, 4, color, color);
			delete arrow;
		}

		game->drawRect(0 + angleCounts[currentAnimation] * 50, 51, 50 + 50 * angleCounts[currentAnimation], 99, 0xFF00AA00, 0xFFFFFFFF);
		game->drawRect(6 + angleCounts[currentAnimation] * 50, 73, 44 + angleCounts[currentAnimation] * 50, 77, 0xFF00AA00, 0xFF00AA00);
		game->drawRect(23 + angleCounts[currentAnimation] * 50, 56, 27 + angleCounts[currentAnimation] * 50, 94, 0xFF00AA00, 0xFF00AA00);

		b2Vec2* arrow = new b2Vec2[4];
		arrow[0] = b2Vec2(cos(angles[currentAnimation][currentFrame]) * 3.3, sin(angles[currentAnimation][currentFrame]) * 3.3);
		arrow[1] = b2Vec2(cos(angles[currentAnimation][currentFrame] + 0.2f) * (3 - 0.1f), sin(angles[currentAnimation][currentFrame] + 0.2f) * (2.9f));
		arrow[2] = b2Vec2(cos(angles[currentAnimation][currentFrame]) * (3), sin(angles[currentAnimation][currentFrame]) * (3));
		arrow[3] = b2Vec2(cos(angles[currentAnimation][currentFrame] - 0.2f) * (3 - 0.1f), sin(angles[currentAnimation][currentFrame] - 0.2f) * (2.9f));

		game->DrawPolygon(arrow, 4, 0xFF000000, 0x55000000);
		game->DrawCircle(b2Vec2(cos(angles[currentAnimation][currentFrame]) * 2.9f, sin(angles[currentAnimation][currentFrame]) * 2.9f), 0.1f, 0xFF000000, 0x55000000);
		delete arrow;
	}

	float layerHeight = 50;
	if (bodiesCount > 16) {
		layerHeight = 800 / bodiesCount;
	}

	for (int index = 0; index < bodiesCount; index++) {
		int i = frameAnimLayer[currentAnimation][currentFrame][index];
		if (index != selectedLayer) {
			DWORD color = 0xFF000000;
			if (i == selectedBody) {
				color = 0xFFFF0000;
			}
			DWORD bgcolor = 0xFFFFFFFF;
			if (!frameAnimShow[currentAnimation][currentFrame][i]) {
				bgcolor = 0xFFAAAAAA;
			}
			if (frameAnimHideLayer[currentAnimation][currentFrame][i]) {
				bgcolor = 0xFFAA0000;
			}
			game->drawRect(1, 60 + layerHeight * (bodiesCount - index), 50, 109 + layerHeight * (bodiesCount - index), color, bgcolor);
			animations[i]->RenderEx(
				25,
				85 + layerHeight * (bodiesCount - index),
				frameAnimAngle[currentAnimation][currentFrame][i],
				30 / game->getPixelsPerMeter(),
				30 / game->getPixelsPerMeter()
			);
		}
	}

	if (selectedLayer > -1) {
		float selectedLayerY = y;
		if (selectedLayerY < 110) {
			selectedLayerY = 110;
		}
		if (selectedLayerY > (110 + bodiesCount * layerHeight)) {
			selectedLayerY = 110 + bodiesCount * layerHeight;
		}
		game->drawRect(layerHeight, selectedLayerY - layerHeight * 0.5, 100, selectedLayerY + layerHeight * 0.5, 0xFF000000, 0xFFFFFFFF);
		animations[frameAnimLayer[currentAnimation][currentFrame][selectedLayer]]->RenderEx(
			75,
			selectedLayerY,
			frameAnimAngle[currentAnimation][currentFrame][selectedLayer],
			30 / game->getPixelsPerMeter(),
			30 / game->getPixelsPerMeter()
		);
	}

	game->getHge()->Gfx_RenderLine(1300, 0, 1300, 900, 0xFF000000);
	game->getHge()->Gfx_RenderLine(1300, 450, 1600, 450, 0xFF000000);

	game->drawRect(1300, 420, 1600, 900, 0xFF555555, 0xFF999999);
	game->drawRect(1300 + currentTab * 30, 420, 1330 + currentTab * 30, 450, 0xFF000000, 0);

	for (int i = 0; i < bodiesCount; i++) {
		if (animShow(i)) {
		} else {
//			if (i != selectedBody) {
				animations[i]->RenderEx(1300 + animationX[i], 450 - 1000 * currentTab + animationY[i], animationAngle[i], 1, 1);

				DWORD color = 0xFFAA0000;
				if (i == selectedBody) {
					color = 0xFFFF0000;
				}

				hgeRect* bb = new hgeRect();
				animations[i]->GetBoundingBoxEx(1300 + animationX[i], 450 - 1000 * currentTab + animationY[i], animationAngle[i], 1, 1, bb);

				game->drawRect(bb->x1, bb->y1, bb->x2, bb->y2, color, 0);
//			}
		}
	}

	for (int i = 0; i < hotSpotsCount; i++) {
		if (!hsShow(i)) {
			b2Transform trans(b2Vec2_zero, b2Rot(hotSpotAngle[i]));
			game->drawPoly(
				(b2PolygonShape*)hotSpots[i]->getShape(),
				trans,
				b2Vec2(1300 + hotSpotX[i], 450 - 1000 * currentTab + hotSpotY[i]),
				game->getPixelsPerMeter(),
				0xFFFF0000, 0xAAFF0000
			);
		}
	}

	if (mode == MODE_INSERT_ANIM) {
        beingInsertedAnim->Render(insertX, insertY);
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
	hge->System_SetState(HGE_LOGFILE, "character_editor.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "SoD character editor");

	game = new Game(hge);

	for (int i = 0; i < 256; i++) {
		frameAnimShow[i] = new bool*[256];
		frameAnimHideLayer[i] = new bool*[256];
		frameAnimX[i] = new float*[256];
		frameAnimY[i] = new float*[256];
		frameAnimAngle[i] = new float*[256];
		frameAnimLayer[i] = new int*[256];
		frameHotSpotX[i] = new float*[256];
		frameHotSpotY[i] = new float*[256];
		frameHotSpotAngle[i] = new float*[256];
		frameHotSpotShow[i] = new bool*[256];
		framesLengths[i] = new float[256];
		framesCounts[i] = 1;
		actionsCounts[i] = 0;

		animRotating[i] = new bool[256];
		hotSpotRotating[i] = new bool[256];

		angleAnimShow[i] = new bool*[256];
		angleAnimX[i] = new float*[256];
		angleAnimY[i] = new float*[256];
		angleAnimAngle[i] = new float*[256];
		angles[i] = new float[256];
		angleCounts[i] = 1;
		angleHotSpotX[i] = new float*[256];
		angleHotSpotY[i] = new float*[256];
		angleHotSpotAngle[i] = new float*[256];
		angleHotSpotShow[i] = new bool*[256];
		for (int j = 0; j < 256; j++) {
			frameAnimX[i][j] = new float[256];
			frameAnimY[i][j] = new float[256];
			frameAnimAngle[i][j] = new float[256];
			frameAnimShow[i][j] = new bool[256];
			frameAnimHideLayer[i][j] = new bool[256];
			frameAnimLayer[i][j] = new int[256];
			frameHotSpotX[i][j] = new float[256];
			frameHotSpotY[i][j] = new float[256];
			frameHotSpotAngle[i][j] = new float[256];
			frameHotSpotShow[i][j] = new bool[256];
			framesLengths[i][j] = 0.5f;

			animRotating[i][j] = false;
			hotSpotRotating[i][j] = false;

			angleAnimShow[i][j] = new bool[256];
			angleAnimX[i][j] = new float[256];
			angleAnimY[i][j] = new float[256];
			angleAnimAngle[i][j] = new float[256];
			angles[i][j] = 0;
			angleHotSpotX[i][j] = new float[256];
			angleHotSpotY[i][j] = new float[256];
			angleHotSpotAngle[i][j] = new float[256];
			angleHotSpotShow[i][j] = new bool[256];
			for (int k = 0; k < 256; k++) {
				frameAnimX[i][j][k] = 0;
				frameAnimY[i][j][k] = 0;
				frameAnimAngle[i][j][k] = 0;
				frameAnimShow[i][j][k] = false;
				frameAnimHideLayer[i][j][k] = false;
				frameAnimLayer[i][j][k] = k;
				frameHotSpotX[i][j][k] = 0;
				frameHotSpotY[i][j][k] = 0;
				frameHotSpotAngle[i][j][k] = 0;
				frameHotSpotShow[i][j][k] = false;

				angleAnimShow[i][j][k] = false;
				angleAnimX[i][j][k] = 0;
				angleAnimY[i][j][k] = 0;
				angleAnimAngle[i][j][k] = 0;
				angleHotSpotX[i][j][k] = 0;
				angleHotSpotY[i][j][k] = 0;
				angleHotSpotAngle[i][j][k] = 0;
				angleHotSpotShow[i][j][k] = false;
			}
		}
	}

	if (game->preload()) {
		//bgTex = game->getHge()->Texture_Load("box.png");

		fnt = new hgeFont("font1.fnt");

		disabledIcon = game->loadAnimation("disabled_icon.xml");
		resizeIcon = game->loadAnimation("resize_icon.xml");

		grayQuad.v[0].x = 1300; grayQuad.v[0].y = 0; grayQuad.v[0].col = 0xFFAAAAAA; grayQuad.v[0].z = 1;
		grayQuad.v[1].x = 1600; grayQuad.v[1].y = 0; grayQuad.v[1].col = 0xFFAAAAAA; grayQuad.v[1].z = 1;
		grayQuad.v[2].x = 1600; grayQuad.v[2].y = 900; grayQuad.v[2].col = 0xFFAAAAAA; grayQuad.v[2].z = 1;
		grayQuad.v[3].x = 1300; grayQuad.v[3].y = 900; grayQuad.v[3].col = 0xFFAAAAAA; grayQuad.v[3].z = 1;
		grayQuad.tex = 0;//bgTex;
		grayQuad.blend = 2;


		mainWindow = new GUIWindow(game, 1, 1300, 0, 300, 420);
		game->getGUI()->AddCtrl(mainWindow);

		mainWindow->AddCtrl(new hgeGUIMenuItem(2, fnt, 150, 20, 0.0f, "insert anim", insertAnimButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(4, fnt, 120, 60, 0.0f, "save", saveCharacterButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(5, fnt, 180, 60, 0.0f, "load", loadCharacterButtonClick));

		//mainWindow->AddCtrl(new hgeGUIMenuItem(6, fnt, 150, 120, 0.0f, "insert frame", insertFrameButtonClick));
		//mainWindow->AddCtrl(new hgeGUIMenuItem(7, fnt, 130, 145, 0.0f, "<<", prevFrameButtonClick));
		//mainWindow->AddCtrl(new hgeGUIMenuItem(8, fnt, 170, 145, 0.0f, ">>", nextFrameButtonClick));

		mainWindow->AddCtrl(new hgeGUIMenuItem(9, fnt, 130, 165, 0.0f, "-", decreaseLengthButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(10, fnt, 170, 165, 0.0f, "+", increaseLengthButtonClick));

		mainWindow->AddCtrl(new hgeGUIMenuItem(11, fnt, 150, 200, 0.0f, "insert animation", insertAnimationButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(12, fnt, 130, 225, 0.0f, "<<", prevAnimationButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(13, fnt, 170, 225, 0.0f, ">>", nextAnimationButtonClick));

		mainWindow->AddCtrl(new hgeGUIMenuItem(14, fnt, 150, 260, 0.0f, "add hotspot", addHotspotButtonClick));

		mainWindow->AddCtrl(new hgeGUIMenuItem(15, fnt, 150, 300, 0.0f, "angles mode", anglesModeButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(16, fnt, 150, 330, 0.0f, "frames mode", framesModeButtonClick));
		hotspotTypesWindow = new GUIWindow(game, 40, 1300, 0, 300, 600);
		game->getGUI()->AddCtrl(hotspotTypesWindow);
		hotspotTypesWindow->AddCtrl(new hgeGUIMenuItem(41, fnt, 150, 20, 0.0f, "box", boxHotspotButtonClick));
		hotspotTypesWindow->Hide();

		for (int i = 0; i < 10; i++) {
			char* s = new char();
			mainWindow->AddCtrl(new hgeGUIMenuItem(20 + i, fnt, 15 + 30 * i, 420, 0.0f, itoa(i + 1, s, 10), setTabButtonClick));
		}


		animsWindow = new GUIWindow(game, 100, 1000, 120, 800, 600);
		game->getGUI()->AddCtrl(animsWindow);
		for (int i = 0; i < game->getAnimationsCount(); i++) {
			animsWindow->AddCtrl(new hgeGUIMenuItem(101 + i, fnt, 250, 10 + i * 20, 0.0f, game->getAnimationName(i), animButtonClick));
			//printf("foo %s\n", game->getAnimationName(i));
		}
		animsWindow->Hide();

		animContextMenuWindow = new GUIWindow(game, 50, 0, 0, 100, 30);
		game->getGUI()->AddCtrl(animContextMenuWindow);
		animContextMenuWindow->AddCtrl(new hgeGUIMenuItem(51, fnt, 50, 0, 0.0f, "details", animDetailsButtonClick));
		animContextMenuWindow->Hide();

		game->moveCamera(b2Vec2(-6.5, -4.5));

		game->loop();

		//delete fnt;

	}

	return 0;
}
