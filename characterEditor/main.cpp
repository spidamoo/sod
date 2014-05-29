#include <stdio.h>
#include <hge.h>
#include <hgefont.h>
#include <hgeanim.h>
#include <Box2D/Box2D.h>
#include <SoDlib.h>
#include <windows.h>
#include <list>

#include "resource.h"

LPTSTR icon;

/// Переменные
//{

Game* game;

const int MODE_DEFAULT           = 0;
const int MODE_SELECT_ANIM       = 1;
const int MODE_INSERT_ANIM       = 2;
const int MODE_ANIM_DRAG         = 7;
const int MODE_CAMERA_MOVE       = 8;
const int MODE_ANIM_ROTATE       = 9;
const int MODE_LAYER_DRAG        = 10;
const int MODE_ANIM_CONTEXT_MENU = 11;
const int MODE_RESIZE_CHARACTER  = 12;
const int MODE_ANIM_RESIZE       = 13;

const int MODE_PLAYING           = 20;

const int MODE_ADD_HOTSPOT            = 31;
const int MODE_ADD_HOTSPOT_BOX_STEP1  = 32;
const int MODE_ADD_HOTSPOT_BOX_STEP2  = 33;
const int MODE_HOTSPOT_DRAG           = 34;
const int MODE_HOTSPOT_ROTATE         = 35;
const int MODE_HOTSPOT_CONTEXT_MENU   = 36;

const int MODE_ANGLE_DRAG  = 40;

const int MODE_FRAME_CONTEXT_MENU = 50;
const int MODE_MOVE_CONTEXT_MENU  = 51;

const int MODE_MOVES_INDEX = 100;
const int MODE_MOVE_ACTIONCAUSE_EDIT = 101;
const int MODE_MOVE_ACTIONEFFECT_EDIT = 102;

const int EDIT_MODE_FRAMES   = 0;
const int EDIT_MODE_ANGLES   = 1;

int mode     = MODE_DEFAULT;
int editMode = EDIT_MODE_FRAMES;

GUIWindow* mainWindow;
GUIWindow* animsWindow;
GUIWindow* hotspotTypesWindow;
GUIWindow* animContextMenuWindow;
GUIWindow* movesIndexWindow;
hgeGUIEditableLabel* moveNameLabel;
GUIWindow* hotSpotContextWindow;

GUIWindow* actionCauseContextWindow;
GUIWindow* actionCauseTypeWindow;
GUIWindow* actionCauseParamWindowSwitch;
GUIWindow* actionCauseParamWindowInput;
GUIWindow* actionCauseParamWindowKey;
GUIWindow* actionCauseParamWindowSide;
hgeGUIEditableLabel* actionCauseParamInput;

GUIWindow* actionEffectContextWindow;
GUIWindow* actionEffectTypeWindow;
GUIWindow* actionEffectParamWindowSwitch;
GUIWindow* actionEffectParamWindowInput;
GUIWindow* actionEffectParamWindowKey;
GUIWindow* actionEffectParamWindowSide;
hgeGUIEditableLabel* actionEffectParamInput;
hgeGUIEditableLabel* actionEffectParam2Input;

GUIWindow* frameContextWindow;
GUIWindow* moveContextWindow;

hgeAnimation* beingInsertedAnim;
char* beingInsertedAnimName;

char** listAnimationNames = new char*[256];

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

int selectedAction = -1;
int selectedActionCoE = -1;

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
int movesCount = 1;
int currentMove = 0;
float*** frameAnimX = new float**[256];
float*** frameAnimY = new float**[256];
float*** frameAnimAngle = new float**[256];
float*** frameAnimScaleX = new float**[256];
float*** frameAnimScaleY = new float**[256];
int*** frameAnimLayer = new int**[256];
bool*** frameAnimShow = new bool**[256];
bool*** frameAnimHideLayer = new bool**[256];
float*** angleAnimX = new float**[256]; float*** angleAnimY = new float**[256]; float*** angleAnimAngle = new float**[256];
float*** angleAnimScaleX = new float**[256]; float*** angleAnimScaleY = new float**[256];
float*** angleHotSpotX = new float**[256]; float*** angleHotSpotY = new float**[256]; float*** angleHotSpotAngle = new float**[256];
bool*** angleHotSpotShow = new bool**[256];
bool*** angleAnimShow = new bool**[256];
int* angleCounts = new int[256]; float** angles = new float*[256];
bool** animRotating = new bool*[256]; bool** hotSpotRotating = new bool*[256];
float*** frameHotSpotX = new float**[256];
float*** frameHotSpotY = new float**[256];
float*** frameHotSpotAngle = new float**[256];
bool*** frameHotSpotShow = new bool**[256];
char** moveNames = new char*[256];
char** bodyNames = new char*[256];

float* resourceBarWidths = new float[64];
float* resourceBarHeights = new float[64];
float* resourceBarPositionX = new float[64];
float* resourceBarPositionY = new float[64];
DWORD* resourceBarEmptyColors = new DWORD[64];
DWORD* resourceBarFullColors = new DWORD[64];
DWORD* resourceBarBoostColors = new DWORD[64];
DWORD* resourceBarFrameColors = new DWORD[64];

int currentTab = 0;
float currentTime = 0;
float frameProgress = 0;
bool drawBoxes = true;

HTEXTURE bgTex;
hgeFont* fnt;
hgeFont* arial12;

hgeAnimation* disabledIcon; hgeAnimation* resizeIcon;

hgeQuad grayQuad;

char* actionCauses[500] = {};
char* actionEffects[500] = {};
char* keyNames[20] = {};

//} Переменные

/// Получение параметров тел
//{
float animX(int anim) {
	if (!animRotating[currentMove][anim]) {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return frameAnimX[currentMove][currentFrame][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return frameAnimX[currentMove][0][anim];
			}
		}
		int nextFrame;
		if (currentFrame >= framesCounts[currentMove] - 1) {
			nextFrame = 0;
		} else {
			nextFrame = currentFrame + 1;
		}
		return frameAnimX[currentMove][currentFrame][anim]
			+ (frameAnimX[currentMove][nextFrame][anim] - frameAnimX[currentMove][currentFrame][anim]) * frameProgress;
	} else {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return angleAnimX[currentMove][0][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return angleAnimX[currentMove][currentFrame][anim];
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
			for (int i = 0; i < angleCounts[currentMove]; i++) {
				float nAngle = angles[currentMove][i];
				float pAngle = angles[currentMove][i];
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
			return angleAnimX[currentMove][prevAngle][anim]
				+ (angleAnimX[currentMove][nextAngle][anim] - angleAnimX[currentMove][prevAngle][anim]) * progress;
		}
	}
}
float animY(int anim) {
	if (!animRotating[currentMove][anim]) {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return frameAnimY[currentMove][currentFrame][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return frameAnimY[currentMove][0][anim];
			}
		}
		int nextFrame;
		if (currentFrame >= framesCounts[currentMove] - 1) {
			nextFrame = 0;
		} else {
			nextFrame = currentFrame + 1;
		}
		return frameAnimY[currentMove][currentFrame][anim]
			+ (frameAnimY[currentMove][nextFrame][anim] - frameAnimY[currentMove][currentFrame][anim]) * frameProgress;
	} else {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return angleAnimY[currentMove][0][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return angleAnimY[currentMove][currentFrame][anim];
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
			for (int i = 0; i < angleCounts[currentMove]; i++) {
				float nAngle = angles[currentMove][i];
				float pAngle = angles[currentMove][i];
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
			return angleAnimY[currentMove][prevAngle][anim]
				+ (angleAnimY[currentMove][nextAngle][anim] - angleAnimY[currentMove][prevAngle][anim]) * progress;
		}
	}
}
float animAngle(int anim) {
	if (!animRotating[currentMove][anim]) {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return frameAnimAngle[currentMove][currentFrame][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return frameAnimAngle[currentMove][0][anim];
			}
		}
		int nextFrame;
		if (currentFrame >= framesCounts[currentMove] - 1) {
			nextFrame = 0;
		} else {
			nextFrame = currentFrame + 1;
		}

		while (frameAnimAngle[currentMove][currentFrame][anim] > M_PI) {
            frameAnimAngle[currentMove][currentFrame][anim] -= 2.0f * M_PI;
		}
		while (frameAnimAngle[currentMove][currentFrame][anim] < -M_PI) {
            frameAnimAngle[currentMove][currentFrame][anim] += 2.0f * M_PI;
		}
		while (frameAnimAngle[currentMove][nextFrame][anim] - frameAnimAngle[currentMove][currentFrame][anim] > M_PI) {
            frameAnimAngle[currentMove][nextFrame][anim] -= 2.0f * M_PI;
		}
		while (frameAnimAngle[currentMove][nextFrame][anim] - frameAnimAngle[currentMove][currentFrame][anim] < -M_PI) {
            frameAnimAngle[currentMove][nextFrame][anim] += 2.0f * M_PI;
		}

		return frameAnimAngle[currentMove][currentFrame][anim]
			+ (frameAnimAngle[currentMove][nextFrame][anim] - frameAnimAngle[currentMove][currentFrame][anim]) * frameProgress;
	} else {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return angleAnimAngle[currentMove][0][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return angleAnimAngle[currentMove][currentFrame][anim];
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
			for (int i = 0; i < angleCounts[currentMove]; i++) {
				float nAngle = angles[currentMove][i];
				float pAngle = angles[currentMove][i];
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

			while (angleAnimAngle[currentMove][prevAngle][anim] > M_PI) {
                angleAnimAngle[currentMove][prevAngle][anim] -= 2.0f * M_PI;
            }
            while (angleAnimAngle[currentMove][prevAngle][anim] < -M_PI) {
                angleAnimAngle[currentMove][prevAngle][anim] += 2.0f * M_PI;
            }
            while (angleAnimAngle[currentMove][nextAngle][anim] - angleAnimAngle[currentMove][prevAngle][anim] > M_PI) {
                angleAnimAngle[currentMove][nextAngle][anim] -= 2.0f * M_PI;
            }
            while (angleAnimAngle[currentMove][nextAngle][anim] - angleAnimAngle[currentMove][prevAngle][anim] < -M_PI) {
                angleAnimAngle[currentMove][nextAngle][anim] += 2.0f * M_PI;
            }

//			if (nAngle < pAngle) {
//                nAngle += 2 * M_PI;
//			}
			return angleAnimAngle[currentMove][prevAngle][anim]
				+ (angleAnimAngle[currentMove][nextAngle][anim] - angleAnimAngle[currentMove][prevAngle][anim]) * progress;
		}
	}
}
float animScaleX(int anim) {
	if (!animRotating[currentMove][anim]) {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return frameAnimScaleX[currentMove][currentFrame][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return frameAnimScaleX[currentMove][0][anim];
			}
		}
		int nextFrame;
		if (currentFrame >= framesCounts[currentMove] - 1) {
			nextFrame = 0;
		} else {
			nextFrame = currentFrame + 1;
		}
		return frameAnimScaleX[currentMove][currentFrame][anim]
			+ (frameAnimScaleX[currentMove][nextFrame][anim] - frameAnimScaleX[currentMove][currentFrame][anim]) * frameProgress;
	} else {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return angleAnimScaleX[currentMove][0][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return angleAnimScaleX[currentMove][currentFrame][anim];
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
			for (int i = 0; i < angleCounts[currentMove]; i++) {
				float nAngle = angles[currentMove][i];
				float pAngle = angles[currentMove][i];
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
			return angleAnimScaleX[currentMove][prevAngle][anim]
				+ (angleAnimScaleX[currentMove][nextAngle][anim] - angleAnimScaleX[currentMove][prevAngle][anim]) * progress;
		}
	}
}
float animScaleY(int anim) {
	if (!animRotating[currentMove][anim]) {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return frameAnimScaleY[currentMove][currentFrame][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return frameAnimScaleY[currentMove][0][anim];
			}
		}
		int nextFrame;
		if (currentFrame >= framesCounts[currentMove] - 1) {
			nextFrame = 0;
		} else {
			nextFrame = currentFrame + 1;
		}
		return frameAnimScaleY[currentMove][currentFrame][anim]
			+ (frameAnimScaleY[currentMove][nextFrame][anim] - frameAnimScaleY[currentMove][currentFrame][anim]) * frameProgress;
	} else {
		if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return angleAnimScaleY[currentMove][0][anim];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return angleAnimScaleY[currentMove][currentFrame][anim];
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
			for (int i = 0; i < angleCounts[currentMove]; i++) {
				float nAngle = angles[currentMove][i];
				float pAngle = angles[currentMove][i];
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
			return angleAnimScaleY[currentMove][prevAngle][anim]
				+ (angleAnimScaleY[currentMove][nextAngle][anim] - angleAnimScaleY[currentMove][prevAngle][anim]) * progress;
		}
	}
}
bool animShow(int anim) {
	if (!animRotating[currentMove][anim]) {
		if (editMode == EDIT_MODE_FRAMES) {
			return frameAnimShow[currentMove][currentFrame][anim];
		} else if (editMode == EDIT_MODE_ANGLES) {
			return frameAnimShow[currentMove][0][anim];
		}
	} else {
		if (editMode == EDIT_MODE_FRAMES) {
			return angleAnimShow[currentMove][0][anim];
		} else if (editMode == EDIT_MODE_ANGLES) {
			return angleAnimShow[currentMove][currentFrame][anim];
		}
	}
}
bool animHideLayer(int anim) {
	return frameAnimHideLayer[currentMove][currentFrame][anim];
}

int animLayer(int anim) {
	if (editMode == EDIT_MODE_FRAMES) {
		return frameAnimLayer[currentMove][currentFrame][anim];
	} else if (editMode == EDIT_MODE_ANGLES) {
		return frameAnimLayer[currentMove][0][anim];
	}
}

float hsX(int hs) {
    if (!hotSpotRotating[currentMove][hs]) {
        if (mode != MODE_PLAYING) {
            if (editMode == EDIT_MODE_FRAMES) {
                return frameHotSpotX[currentMove][currentFrame][hs];
            } else if (editMode == EDIT_MODE_ANGLES) {
				return frameHotSpotX[currentMove][0][hs];
			}
        }
        int nextFrame;
        if (currentFrame >= framesCounts[currentMove] - 1) {
            nextFrame = 0;
        } else {
            nextFrame = currentFrame + 1;
        }
        return frameHotSpotX[currentMove][currentFrame][hs]
            + (frameHotSpotX[currentMove][nextFrame][hs] - frameHotSpotX[currentMove][currentFrame][hs]) * frameProgress;
    } else {
    	if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return angleHotSpotX[currentMove][0][hs];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return angleHotSpotX[currentMove][currentFrame][hs];
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
			for (int i = 0; i < angleCounts[currentMove]; i++) {
				float nAngle = angles[currentMove][i];
				float pAngle = angles[currentMove][i];
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
			return angleHotSpotX[currentMove][prevAngle][hs]
				+ (angleHotSpotX[currentMove][nextAngle][hs] - angleHotSpotX[currentMove][prevAngle][hs]) * progress;
		}
	}
}
float hsY(int hs) {
    if (!hotSpotRotating[currentMove][hs]) {
        if (mode != MODE_PLAYING) {
            if (editMode == EDIT_MODE_FRAMES) {
                return frameHotSpotY[currentMove][currentFrame][hs];
            } else if (editMode == EDIT_MODE_ANGLES) {
				return frameHotSpotY[currentMove][0][hs];
			}
        }
        int nextFrame;
        if (currentFrame >= framesCounts[currentMove] - 1) {
            nextFrame = 0;
        } else {
            nextFrame = currentFrame + 1;
        }
        return frameHotSpotY[currentMove][currentFrame][hs]
            + (frameHotSpotY[currentMove][nextFrame][hs] - frameHotSpotY[currentMove][currentFrame][hs]) * frameProgress;
    } else {
    	if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return angleHotSpotY[currentMove][0][hs];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return angleHotSpotY[currentMove][currentFrame][hs];
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
			for (int i = 0; i < angleCounts[currentMove]; i++) {
				float nAngle = angles[currentMove][i];
				float pAngle = angles[currentMove][i];
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
			return angleHotSpotY[currentMove][prevAngle][hs]
				+ (angleHotSpotY[currentMove][nextAngle][hs] - angleHotSpotY[currentMove][prevAngle][hs]) * progress;
		}
	}
}
float hsAngle(int hs) {
    if (!hotSpotRotating[currentMove][hs]) {
        if (mode != MODE_PLAYING) {
            if (editMode == EDIT_MODE_FRAMES) {
                return frameHotSpotAngle[currentMove][currentFrame][hs];
            } else if (editMode == EDIT_MODE_ANGLES) {
				return frameHotSpotAngle[currentMove][0][hs];
			}
        }
        int nextFrame;
        if (currentFrame >= framesCounts[currentMove] - 1) {
            nextFrame = 0;
        } else {
            nextFrame = currentFrame + 1;
        }
        return frameHotSpotAngle[currentMove][currentFrame][hs]
            + (frameHotSpotAngle[currentMove][nextFrame][hs] - frameHotSpotAngle[currentMove][currentFrame][hs]) * frameProgress;
    } else {
    	if (mode != MODE_PLAYING) {
			if (editMode == EDIT_MODE_FRAMES) {
				return angleHotSpotAngle[currentMove][0][hs];
			} else if (editMode == EDIT_MODE_ANGLES) {
				return angleHotSpotAngle[currentMove][currentFrame][hs];
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
			for (int i = 0; i < angleCounts[currentMove]; i++) {
				float nAngle = angles[currentMove][i];
				float pAngle = angles[currentMove][i];
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
			return angleHotSpotAngle[currentMove][prevAngle][hs]
				+ (angleHotSpotAngle[currentMove][nextAngle][hs] - angleHotSpotAngle[currentMove][prevAngle][hs]) * progress;
		}
	}
}
float hsShow(int hs) {
	if (!hotSpotRotating[currentMove][hs]) {
		if (editMode == EDIT_MODE_FRAMES) {
			return frameHotSpotShow[currentMove][currentFrame][hs];
		} else if (editMode == EDIT_MODE_ANGLES) {
			return frameHotSpotShow[currentMove][0][hs];
		}
	} else {
		if (editMode == EDIT_MODE_FRAMES) {
			return angleHotSpotShow[currentMove][0][hs];
		} else if (editMode == EDIT_MODE_ANGLES) {
			return angleHotSpotShow[currentMove][currentFrame][hs];
		}
	}
}
//} Получение параметров тел

/// Служебные функции
//{
void resetMode() {
    if (mode < MODE_MOVES_INDEX) {
        mode = MODE_DEFAULT;

        selectedBody = -1;
        selectedLayer = -1;
        selectedHotSpot = -1;

        mainWindow->Show();
        animContextMenuWindow->Hide();
        movesIndexWindow->Hide();
        moveNameLabel->Hide();
        actionCauseContextWindow->Hide();
        frameContextWindow->Hide();
        moveContextWindow->Hide();
        hotSpotContextWindow->Hide();
    }
    else {
        mode = MODE_MOVES_INDEX;

        movesIndexWindow->Show();
        moveNameLabel->Show();

        mainWindow->Hide();
        animContextMenuWindow->Hide();
    }
}
int getPointedBody(float x, float y) {
	int selected = -1;
	for (int index = 0; index < bodiesCount; index++) {
		int i = animLayer(index);
		if (animShow(i) && !frameAnimHideLayer[currentMove][currentFrame][i]) {
			hgeRect* bb = new hgeRect();
			animations[i]->GetBoundingBoxEx(
				game->screenX(animX(i)),
				game->screenY(animY(i)),
				animAngle(i),
				animScaleX(i) * game->getScaleFactor(),
				animScaleY(i) * game->getScaleFactor(),
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
int getPointedHotSpot(float x, float y) {
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
void setFrame(int frame, bool resetTime) {
	float time = 0;
	for (int i = 0; i < frame; i++) {
		time += framesLengths[currentMove][i];
	}
	if (resetTime)
		currentTime = time;
	currentFrame = frame;
	for (int i = 0; i < bodiesCount; i++) {
		//animations[i]->SetZ(1.0f - 0.001 * frameAnimLayer[currentMove][currentFrame][i]);
		//animationX[i] = frameAnimX[currentMove][currentFrame][i];
		//animationY[i] = frameAnimY[currentMove][currentFrame][i];
		//animationAngle[i] = frameAnimAngle[currentMove][currentFrame][i];
	}
}
void setFrame(int frame) {
	setFrame(frame, false);
}
void setMove(int move) {
	currentMove = move;
	printf("move %i %s\n", move, moveNames[currentMove]);
	setFrame(0, true);
	moveNameLabel->setTitle(moveNames[currentMove]);
}

bool saveCharacter(char* fn) {
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
        element->SetAttribute("name", bodyNames[i]);
        element->SetDoubleAttribute("x", animationX[i]);
        element->SetDoubleAttribute("y", animationY[i]);
        element->SetDoubleAttribute("angle", animationAngle[i]);
        //cxn->SetDoubleAttribute("timeout", 123.456);
	}
	for (int i = 0; i < hotSpotsCount; i++) {
        TiXmlElement* element = new TiXmlElement( "hotspot" );
        characterElem->LinkEndChild( element );
        element->SetAttribute("shape", "box");
        element->SetAttribute("name", hotSpots[i]->getName());
        element->SetDoubleAttribute("x", hotSpotX[i]);
        element->SetDoubleAttribute("y", hotSpotY[i]);
        element->SetDoubleAttribute("angle", hotSpotAngle[i]);
        element->SetDoubleAttribute("w", hotSpotWidths[i]);
        element->SetDoubleAttribute("h", hotSpotHeights[i]);
        //cxn->SetDoubleAttribute("timeout", 123.456);
	}

	characterElem->SetAttribute("moves", movesCount);
	for (int i = 0; i < movesCount; i++) {
        TiXmlElement* moveElem = new TiXmlElement( "move" );
		moveElem->SetAttribute("frames", framesCounts[i]);
		moveElem->SetAttribute("angles", angleCounts[i]);
		moveElem->SetAttribute("name", moveNames[i]);
		for (int j = 0; j < bodiesCount; j++) {
			TiXmlElement* bodyElem = new TiXmlElement( "body" );
			bodyElem->SetAttribute("rotating", animRotating[i][j] ? 1 : 0);
			moveElem->LinkEndChild( bodyElem );
		}
        //moveElem->SetAttribute("animation", animationNames[i]);
        for (int j = 0; j < framesCounts[i]; j++) {
			TiXmlElement* frameElem = new TiXmlElement( "frame" );
			frameElem->SetDoubleAttribute("length", framesLengths[i][j]);

			for (int k = 0; k < bodiesCount; k++) {
				TiXmlElement* bodyElem = new TiXmlElement( "body" );
				bodyElem->SetDoubleAttribute("x", frameAnimX[i][j][k]);
				bodyElem->SetDoubleAttribute("y", frameAnimY[i][j][k]);
				bodyElem->SetDoubleAttribute("scale_x", frameAnimScaleX[i][j][k]);
				bodyElem->SetDoubleAttribute("scale_y", frameAnimScaleY[i][j][k]);
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
			moveElem->LinkEndChild( frameElem );

        }

        for (int j = 0; j < angleCounts[i]; j++) {
			TiXmlElement* frameElem = new TiXmlElement( "angle" );
			frameElem->SetDoubleAttribute("value", angles[i][j]);

			for (int k = 0; k < bodiesCount; k++) {
				TiXmlElement* bodyElem = new TiXmlElement( "body" );
				bodyElem->SetDoubleAttribute("x", angleAnimX[i][j][k]);
				bodyElem->SetDoubleAttribute("y", angleAnimY[i][j][k]);
				bodyElem->SetDoubleAttribute("scale_x", angleAnimScaleX[i][j][k]);
				bodyElem->SetDoubleAttribute("scale_y", angleAnimScaleY[i][j][k]);
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
			moveElem->LinkEndChild( frameElem );

        }

        TiXmlElement* actionsRoot = new TiXmlElement( "actions" );
        actionsRoot->SetAttribute("count", actionsCounts[i]);
		for (int j = 0; j < actionsCounts[i]; j++) {
			TiXmlElement* actionElem = new TiXmlElement( "action" );
			actions[i][j]->save(actionElem);
			actionsRoot->LinkEndChild( actionElem );
        }

        moveElem->LinkEndChild( actionsRoot );
        characterElem->LinkEndChild( moveElem );
        //cxn->SetDoubleAttribute("timeout", 123.456);
	}

	for (int i = 0; i < game->getCharacterResourcePrototypesCount(); i++) {
        TiXmlElement* element = new TiXmlElement( "resource" );
        characterElem->LinkEndChild( element );

        element->SetDoubleAttribute("width", resourceBarWidths[i]);
        element->SetDoubleAttribute("height", resourceBarHeights[i]);
        element->SetDoubleAttribute("x", resourceBarPositionX[i]);
        element->SetDoubleAttribute("y", resourceBarPositionY[i]);
        element->SetAttribute("full_color", resourceBarFullColors[i]);
        element->SetAttribute("empty_color", resourceBarEmptyColors[i]);
        element->SetAttribute("boost_color", resourceBarBoostColors[i]);
        element->SetAttribute("frame_color", resourceBarFrameColors[i]);
	}
	doc.LinkEndChild( characterElem );
	doc.SaveFile(fn);
}
bool loadCharacter(char* fn) {
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

			if (element->Attribute("name")) {
                bodyNames[i] = copyString( element->Attribute("name") );
			}
			else {
                bodyNames[i] = "<body>";
			}

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
            hotSpots[i]->setName( (char*)element->Attribute("name") );

			hotSpotX[i] = x;
			hotSpotY[i] = y;
			hotSpotAngle[i] = angle;
			hotSpotWidths[i] = w;
			hotSpotHeights[i] = h;

			i++;
            element = element->NextSiblingElement("hotspot");
        }
        hotSpotsCount = i;

        TiXmlElement* moveElem = characterElem->FirstChildElement("move");
        i = 0;
        while (moveElem) {
            if (moveElem->Attribute("name")) {
                delete moveNames[i];
                moveNames[i] = copyString(moveElem->Attribute("name"));
                characterHeight = atof(characterElem->Attribute("height"));
            }
			int j = 0;
			TiXmlElement* bodyElem = moveElem->FirstChildElement("body");
			while (bodyElem) {
				animRotating[i][j] = atoi(bodyElem->Attribute("rotating"));
				j++;
				bodyElem = bodyElem->NextSiblingElement("body");
			}
        	j = 0;
        	TiXmlElement* frameElem = moveElem->FirstChildElement("frame");
        	while (frameElem) {
        		framesLengths[i][j] = atof(frameElem->Attribute("length"));
        		TiXmlElement* bodyElem = frameElem->FirstChildElement("body");
				int k = 0;
				while (bodyElem) {
					frameAnimX[i][j][k] = atof(bodyElem->Attribute("x"));
					frameAnimY[i][j][k] = atof(bodyElem->Attribute("y"));
					frameAnimScaleX[i][j][k] = bodyElem->Attribute("scale_x") ? atof(bodyElem->Attribute("scale_x")) : 1.0f;
					frameAnimScaleY[i][j][k] = bodyElem->Attribute("scale_y") ? atof(bodyElem->Attribute("scale_y")) : 1.0f;
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
        	frameElem = moveElem->FirstChildElement("angle");
        	while (frameElem) {
        		angles[i][j] = atof(frameElem->Attribute("value"));
        		TiXmlElement* bodyElem = frameElem->FirstChildElement("body");
				int k = 0;
				while (bodyElem) {
					angleAnimX[i][j][k] = atof(bodyElem->Attribute("x"));
					angleAnimY[i][j][k] = atof(bodyElem->Attribute("y"));
					angleAnimScaleX[i][j][k] = bodyElem->Attribute("scale_x") ? atof(bodyElem->Attribute("scale_x")) : 1.0f;
					angleAnimScaleY[i][j][k] = bodyElem->Attribute("scale_y") ? atof(bodyElem->Attribute("scale_y")) : 1.0f;
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

        	TiXmlElement* actionsRoot = moveElem->FirstChildElement("actions");
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
            moveElem = moveElem->NextSiblingElement("move");
        }
		movesCount = i;

		element = characterElem->FirstChildElement("resource");
        i = 0;
        while (element) {
            resourceBarWidths[i] = atof( element->Attribute("width") );
            resourceBarHeights[i] = atof( element->Attribute("height") );
            resourceBarPositionX[i] = atof( element->Attribute("x") );
            resourceBarPositionY[i] = atof( element->Attribute("y") );

            resourceBarFullColors[i] = atoi( element->Attribute("full_color") );
            resourceBarEmptyColors[i] = atoi( element->Attribute("empty_color") );
            resourceBarBoostColors[i] = atoi( element->Attribute("boost_color") );
            resourceBarFrameColors[i] = atoi( element->Attribute("frame_color") );

            i++;
            element = element->NextSiblingElement("resource");
        }

		printf("character loaded\n");
		setMove(0);
    }
}

void addAction() {
    actions[currentMove][actionsCounts[currentMove]] = new CharacterAction();
    actionsCounts[currentMove]++;
}
void removeAction(int index) {
    delete actions[currentMove][index];
    for (int i = index; i < actionsCounts[currentMove] - 1; i++) {
        actions[currentMove][i] = actions[currentMove][i + 1];
    }
    actionsCounts[currentMove]--;
}

//} Служебные функции

/// Установка параметров тел
//{
void setAnimX(int anim, float x) {
	if (editMode == EDIT_MODE_FRAMES) {
		frameAnimX[currentMove][currentFrame][anim] = x;
		animRotating[currentMove][anim] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleAnimX[currentMove][currentFrame][anim] = x;
		animRotating[currentMove][anim] = true;
	}
}
void setAnimY(int anim, float y) {
	if (editMode == EDIT_MODE_FRAMES) {
		frameAnimY[currentMove][currentFrame][anim] = y;
		animRotating[currentMove][anim] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleAnimY[currentMove][currentFrame][anim] = y;
		animRotating[currentMove][anim] = true;
	}
}
void setAnimAngle(int anim, float angle) {
	if (editMode == EDIT_MODE_FRAMES) {
		frameAnimAngle[currentMove][currentFrame][anim] = angle;
		animRotating[currentMove][anim] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleAnimAngle[currentMove][currentFrame][anim] = angle;
		animRotating[currentMove][anim] = true;
	}
}
void setAnimScaleX(int anim, float x) {
    if (fabs(x) < 0.01f)
        return;

	if (editMode == EDIT_MODE_FRAMES) {
		frameAnimScaleX[currentMove][currentFrame][anim] = x;
		animRotating[currentMove][anim] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleAnimScaleX[currentMove][currentFrame][anim] = x;
		animRotating[currentMove][anim] = true;
	}
}
void setAnimScaleY(int anim, float y) {
    if (fabs(y) < 0.01f)
        return;

	if (editMode == EDIT_MODE_FRAMES) {
		frameAnimScaleY[currentMove][currentFrame][anim] = y;
		animRotating[currentMove][anim] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleAnimScaleY[currentMove][currentFrame][anim] = y;
		animRotating[currentMove][anim] = true;
	}
}
void setAnimShow(int anim, bool show) {
	if (editMode == EDIT_MODE_FRAMES) {
		frameAnimShow[currentMove][currentFrame][anim] = show;
		animRotating[currentMove][anim] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleAnimShow[currentMove][currentFrame][anim] = show;
		animRotating[currentMove][anim] = true;
	}
}

void setHsX(int hs, float x) {
	if (editMode == EDIT_MODE_FRAMES) {
		frameHotSpotX[currentMove][currentFrame][hs] = x;
		hotSpotRotating[currentMove][hs] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleHotSpotX[currentMove][currentFrame][hs] = x;
		hotSpotRotating[currentMove][hs] = true;
	}
}
void setHsY(int hs, float y) {
	if (editMode == EDIT_MODE_FRAMES) {
		frameHotSpotY[currentMove][currentFrame][hs] = y;
		hotSpotRotating[currentMove][hs] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleHotSpotY[currentMove][currentFrame][hs] = y;
		hotSpotRotating[currentMove][hs] = true;
	}
}
void setHsAngle(int hs, float angle) {
	if (editMode == EDIT_MODE_FRAMES) {
		frameHotSpotAngle[currentMove][currentFrame][hs] = angle;
		hotSpotRotating[currentMove][hs] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleHotSpotAngle[currentMove][currentFrame][hs] = angle;
		hotSpotRotating[currentMove][hs] = true;
	}
}
void setHsShow(int hs, bool show) {
	if (editMode == EDIT_MODE_FRAMES) {
		frameHotSpotShow[currentMove][currentFrame][hs] = show;
		hotSpotRotating[currentMove][hs] = false;
	} else if (editMode == EDIT_MODE_ANGLES) {
		angleHotSpotShow[currentMove][currentFrame][hs] = show;
		hotSpotRotating[currentMove][hs] = true;
	}
}
//} Установка параметров тел

/// Главное меню
//{
bool saveCharacterButtonClick(hgeGUIObject* sender) {
    OPENFILENAME ofn;
    char szFile[512]="\0";
    char szTemp[512];

    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = game->getHge()->System_GetState(HGE_HWND);
    ofn.lpstrFilter = "XML file\0*.xml\0All Files\0*.*\0\0";
    ofn.lpstrFile= szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    ofn.lpstrDefExt="xml";
    if ( GetSaveFileName(&ofn) ) {
        saveCharacter(szFile);
    }
}
bool loadCharacterButtonClick(hgeGUIObject* sender) {
    OPENFILENAME ofn;
    char szFile[512]="\0";
    char szTemp[512];

    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = game->getHge()->System_GetState(HGE_HWND);
    ofn.lpstrFilter = "XML file\0*.xml\0All Files\0*.*\0\0";
    ofn.lpstrFile= szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    ofn.lpstrDefExt="xml";
    if ( GetOpenFileName(&ofn) ) {
        loadCharacter(szFile);
    }
}

bool increaseLengthButtonClick(hgeGUIObject* sender) {
	framesLengths[currentMove][currentFrame] += 0.1f;
}
bool decreaseLengthButtonClick(hgeGUIObject* sender) {
	if (framesLengths[currentMove][currentFrame] > 0.15)
		framesLengths[currentMove][currentFrame] -= 0.1f;
}

bool setTabButtonClick(hgeGUIObject* sender) {
	currentTab = atoi(((hgeGUIMenuItem*)sender)->getTitle()) - 1;
}

bool addHotspotButtonClick(hgeGUIObject* sender) {
	mainWindow->Hide();
	hotspotTypesWindow->Show();
	mode = MODE_ADD_HOTSPOT;
}
bool boxHotspotButtonClick(hgeGUIObject* sender) {
	mainWindow->Show();
	hotspotTypesWindow->Hide();
	mode = MODE_ADD_HOTSPOT_BOX_STEP1;
}

bool anglesModeButtonClick(hgeGUIObject* sender) {
	resetMode();
	editMode = EDIT_MODE_ANGLES;
	setFrame(0, true);
}
bool framesModeButtonClick(hgeGUIObject* sender) {
    setMove(currentMove);
	resetMode();
	editMode = EDIT_MODE_FRAMES;
	setFrame(0, true);
}
bool movesModeButtonClick(hgeGUIObject* sender) {
    if (sender->id == 17)
        mode = MODE_MOVES_INDEX;
    else
        mode = MODE_DEFAULT;

    setMove(currentMove);
    resetMode();
}

bool toggleLayersClick(hgeGUIObject* sender) {
    int shown = 0;
    int hidden = 0;
    for (int i = 0; i < bodiesCount; i++) {
        if (frameAnimHideLayer[currentMove][currentFrame][i])
            hidden++;
        else
            shown++;
    }
    for (int i = 0; i < bodiesCount; i++) {
        if (shown > hidden)
            frameAnimHideLayer[currentMove][currentFrame][i] = true;
        else
            frameAnimHideLayer[currentMove][currentFrame][i] = false;
    }
}

bool insertAnimButtonClick(hgeGUIObject* sender) {
    OPENFILENAME ofn;
    char szFile[512]="\0";
    char szTemp[512];

    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = game->getHge()->System_GetState(HGE_HWND);
    ofn.lpstrFilter = "XML file\0*.xml\0All Files\0*.*\0\0";
    ofn.lpstrFile= szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    ofn.lpstrDefExt="xml";
    if ( GetOpenFileName(&ofn) ) {
        mode = MODE_INSERT_ANIM;
        //mainWindow->Show();
//        animsWindow->Hide();
        beingInsertedAnim = game->loadAnimation(szFile);
        beingInsertedAnimName = copyString(szFile);
        game->getHge()->Input_SetMousePos(1450, 600);
    }

//	mode = MODE_SELECT_ANIM;
	//mainWindow->Hide();
//	animsWindow->Show();
}

//} Главное меню

/// Интерфейс
//{
/// Окошки
//{
void resetActioncauseWindow() {
    actionCauseContextWindow->Move(700, 100 + selectedAction * 75 + selectedActionCoE * 12 + 12);
    actionCauseContextWindow->Show();
    actionCauseParamWindowSwitch->Hide();
    actionCauseParamWindowInput->Hide();
    actionCauseParamWindowKey->Hide();
    actionCauseParamWindowSide->Hide();

    switch (actions[currentMove][selectedAction]->getCause(selectedActionCoE)->getType()) {
        case ACTIONCAUSE_TYPE_NONE:
            break;
        case ACTIONCAUSE_TYPE_ONGROUND:
            actionCauseParamWindowSwitch->Show();
            if (actions[currentMove][selectedAction]->getCause(selectedActionCoE)->getParam() == 0) {
                game->getGUI()->GetCtrl(303)->selected = true;
                game->getGUI()->GetCtrl(304)->selected = false;
            }
            if (actions[currentMove][selectedAction]->getCause(selectedActionCoE)->getParam() == 1) {
                game->getGUI()->GetCtrl(303)->selected = false;
                game->getGUI()->GetCtrl(304)->selected = true;
            }
            break;
        case ACTIONCAUSE_TYPE_KEYDOWN:
        case ACTIONCAUSE_TYPE_KEYHIT:
        case ACTIONCAUSE_TYPE_KEYRELEASED:
            actionCauseParamWindowKey->Show();
            game->getGUI()->GetCtrl(311)->selected = false;
            game->getGUI()->GetCtrl(312)->selected = false;
            game->getGUI()->GetCtrl(313)->selected = false;
            game->getGUI()->GetCtrl(314)->selected = false;
            switch ((int)actions[currentMove][selectedAction]->getCause(selectedActionCoE)->getParam()) {
                case CHARACTER_KEY_FORTH:
                    game->getGUI()->GetCtrl(311)->selected = true;
                    break;
                case CHARACTER_KEY_BACK:
                    game->getGUI()->GetCtrl(312)->selected = true;
                    break;
                case CHARACTER_KEY_ATTACK:
                    game->getGUI()->GetCtrl(313)->selected = true;
                    break;
                case CHARACTER_KEY_JUMP:
                    game->getGUI()->GetCtrl(314)->selected = true;
                    break;
            }
            break;
        case ACTIONCAUSE_TYPE_NPC_TARGETSIDE:
            actionCauseParamWindowSide->Show();
            if (actions[currentMove][selectedAction]->getCause(selectedActionCoE)->getParam() == 0) {
                game->getGUI()->GetCtrl(331)->selected = true;
                game->getGUI()->GetCtrl(332)->selected = false;
            }
            if (actions[currentMove][selectedAction]->getCause(selectedActionCoE)->getParam() == 1) {
                game->getGUI()->GetCtrl(331)->selected = false;
                game->getGUI()->GetCtrl(332)->selected = true;
            }
            break;
        default:
            actionCauseParamWindowInput->Show();
            char buffer[20];
            sprintf(buffer, "%.2f", actions[currentMove][selectedAction]->getCause(selectedActionCoE)->getParam());
            actionCauseParamInput->setTitle( buffer );
            delete buffer;
            break;
    }

    game->getGUI()->GetCtrl(251)->selected = false;
    game->getGUI()->GetCtrl(252)->selected = false;
    game->getGUI()->GetCtrl(253)->selected = false;
    game->getGUI()->GetCtrl(254)->selected = false;
    game->getGUI()->GetCtrl(255)->selected = false;
    game->getGUI()->GetCtrl(256)->selected = false;
    game->getGUI()->GetCtrl(257)->selected = false;
    game->getGUI()->GetCtrl(258)->selected = false;
    game->getGUI()->GetCtrl(259)->selected = false;
    game->getGUI()->GetCtrl(260)->selected = false;

    switch (actions[currentMove][selectedAction]->getCause(selectedActionCoE)->getType()) {
        case ACTIONCAUSE_TYPE_NONE:
            game->getGUI()->GetCtrl(251)->selected = true;
            break;
        case ACTIONCAUSE_TYPE_KEYHIT:
            game->getGUI()->GetCtrl(252)->selected = true;
            break;
        case ACTIONCAUSE_TYPE_KEYRELEASED:
            game->getGUI()->GetCtrl(253)->selected = true;
            break;
        case ACTIONCAUSE_TYPE_KEYDOWN:
            game->getGUI()->GetCtrl(254)->selected = true;
            break;
        case ACTIONCAUSE_TYPE_ONGROUND:
            game->getGUI()->GetCtrl(255)->selected = true;
            break;
        case ACTIONCAUSE_TYPE_ANIM_TIME_PASSED:
            game->getGUI()->GetCtrl(256)->selected = true;
            break;
        case ACTIONCAUSE_TYPE_ANIM_TIME_IS:
            game->getGUI()->GetCtrl(257)->selected = true;
            break;
        case ACTIONCAUSE_TYPE_NPC_TARGETSIDE:
            game->getGUI()->GetCtrl(258)->selected = true;
            break;
        case ACTIONCAUSE_TYPE_NPC_TARGETFARTHER:
            game->getGUI()->GetCtrl(259)->selected = true;
            break;
        case ACTIONCAUSE_TYPE_NPC_TARGETCLOSER:
            game->getGUI()->GetCtrl(260)->selected = true;
            break;
    }
}
void resetActioneffectWindow() {
    actionEffectContextWindow->Move(1050, 100 + selectedAction * 75 + selectedActionCoE * 12 + 12);
    actionEffectContextWindow->Show();
    actionEffectParamWindowInput->Hide();

    switch (actions[currentMove][selectedAction]->getEffect(selectedActionCoE)->getType()) {
        default:
            actionEffectParamWindowInput->Show();
            char buffer[20];
            sprintf(buffer, "%.2f", actions[currentMove][selectedAction]->getEffect(selectedActionCoE)->getParam());
            actionEffectParamInput->setTitle( buffer );
            sprintf(buffer, "%.2f", actions[currentMove][selectedAction]->getEffect(selectedActionCoE)->getParam(2));
            actionEffectParam2Input->setTitle( buffer );
            delete buffer;
            break;
    }

    game->getGUI()->GetCtrl(511)->selected = false;
    game->getGUI()->GetCtrl(512)->selected = false;
    game->getGUI()->GetCtrl(513)->selected = false;
    game->getGUI()->GetCtrl(514)->selected = false;
    game->getGUI()->GetCtrl(515)->selected = false;

    switch (actions[currentMove][selectedAction]->getEffect(selectedActionCoE)->getType()) {
        case ACTIONEFFECT_TYPE_TURN:
            game->getGUI()->GetCtrl(511)->selected = true;
            break;
        case ACTIONEFFECT_TYPE_CHANGEMOVE:
            game->getGUI()->GetCtrl(512)->selected = true;
            break;
        case ACTIONEFFECT_TYPE_RUN:
            game->getGUI()->GetCtrl(513)->selected = true;
            break;
        case ACTIONEFFECT_TYPE_JUMP:
            game->getGUI()->GetCtrl(514)->selected = true;
            break;
        case ACTIONEFFECT_TYPE_SPAWN_EFFECT:
            game->getGUI()->GetCtrl(515)->selected = true;
            break;

    }
}
void showFrameContextWindow() {
    float rightX = 0;
    for (int i = 0; i < currentFrame; i++) {
        rightX += 100 * framesLengths[currentMove][i];
    }
    frameContextWindow->Move(rightX, 100.0f);
    frameContextWindow->Show();
    char* buffer = new char[16];
    sprintf(buffer, "%.2f", framesLengths[currentMove][currentFrame]);
    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(62) )->setTitle(buffer);
    delete buffer;
}
void showMoveContextWindow() {
    float x = currentMove * 50.0f;
    moveContextWindow->Move(x, 50.0f);
    moveContextWindow->Show();

    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(72) )->setTitle( moveNames[currentMove] );
}

//} Окошки

/// События
//{
bool moveNameChange(hgeGUIObject* sender) {
    delete moveNames[currentMove];
    moveNames[currentMove] = copyString(((hgeGUIEditableLabel*)sender)->getTitle());
}
bool setActionCauseClick(hgeGUIObject* sender) {
    int type = ACTIONCAUSE_TYPE_NONE;
    switch (sender->id) {
        case 251:
            type = ACTIONCAUSE_TYPE_NONE;
            break;
        case 252:
            type = ACTIONCAUSE_TYPE_KEYHIT;
            break;
        case 253:
            type = ACTIONCAUSE_TYPE_KEYRELEASED;
            break;
        case 254:
            type = ACTIONCAUSE_TYPE_KEYDOWN;
            break;
        case 255:
            type = ACTIONCAUSE_TYPE_ONGROUND;
            break;
        case 256:
            type = ACTIONCAUSE_TYPE_ANIM_TIME_PASSED;
            break;
        case 257:
            type = ACTIONCAUSE_TYPE_ANIM_TIME_IS;
            break;
        case 258:
            type = ACTIONCAUSE_TYPE_NPC_TARGETSIDE;
            break;
        case 259:
            type = ACTIONCAUSE_TYPE_NPC_TARGETFARTHER;
            break;
        case 260:
            type = ACTIONCAUSE_TYPE_NPC_TARGETCLOSER;
            break;

    }
    actions[currentMove][selectedAction]->getCause(selectedActionCoE)->setType(type);
    actions[currentMove][selectedAction]->getCause(selectedActionCoE)->setParam(1.0f);

    resetActioncauseWindow();
}
bool setActionParamSwitchClick(hgeGUIObject* sender) {
    sender->selected = 1;
    switch (sender->id) {
        case 303:
        case 331:
            actions[currentMove][selectedAction]->getCause(selectedActionCoE)->setParam( 0.0f );
            break;
        case 304:
        case 332:
            actions[currentMove][selectedAction]->getCause(selectedActionCoE)->setParam( 1.0f );
            break;
    }

    resetActioncauseWindow();
}
bool setActionParamKeyClick(hgeGUIObject* sender) {
    int key = 1;
    switch (sender->id) {
        case 311:
            key = CHARACTER_KEY_FORTH;
            break;
        case 312:
            key = CHARACTER_KEY_BACK;
            break;
        case 313:
            key = CHARACTER_KEY_ATTACK;
            break;
        case 314:
            key = CHARACTER_KEY_JUMP;
            break;

    }
    actions[currentMove][selectedAction]->getCause(selectedActionCoE)->setParam( key );

    resetActioncauseWindow();
}
bool actionParamInputChange(hgeGUIObject* sender) {
    switch (sender->id) {
        case 301:
            actions[currentMove][selectedAction]->getCause(selectedActionCoE)->setParam( atof( ((hgeGUIEditableLabel*)sender)->getTitle() ) );
            break;
        case 551:
            actions[currentMove][selectedAction]->getEffect(selectedActionCoE)->setParam( atof( ((hgeGUIEditableLabel*)sender)->getTitle() ) );
            break;
        case 552:
            actions[currentMove][selectedAction]->getEffect(selectedActionCoE)->setParam( atof( ((hgeGUIEditableLabel*)sender)->getTitle() ), 2 );
            break;
    }
}
bool setActionEffectClick(hgeGUIObject* sender) {
    int type = ACTIONEFFECT_TYPE_TURN;

    switch (sender->id) {
        case 511:
            type = ACTIONEFFECT_TYPE_TURN;
            break;
        case 512:
            type = ACTIONEFFECT_TYPE_CHANGEMOVE;
            break;
        case 513:
            type = ACTIONEFFECT_TYPE_RUN;
            break;
        case 514:
            type = ACTIONEFFECT_TYPE_JUMP;
            break;
        case 515:
            type = ACTIONEFFECT_TYPE_SPAWN_EFFECT;
            break;
    }
    actions[currentMove][selectedAction]->getEffect(selectedActionCoE)->setType(type);
    actions[currentMove][selectedAction]->getEffect(selectedActionCoE)->setParam(1.0f);
    actions[currentMove][selectedAction]->getEffect(selectedActionCoE)->setParam(1.0f, 2);

    resetActioneffectWindow();
}
bool removeFrameClick(hgeGUIObject* sender) {
    if (framesCounts[currentMove] <= 1) {
        return true;
    }

    for (int i = currentFrame; i < framesCounts[currentMove] - 1; i++) {
        for (int j = 0; j < bodiesCount; j++) {
            frameAnimX[currentMove][i][j]         = frameAnimX[currentMove][i + 1][j];
            frameAnimY[currentMove][i][j]         = frameAnimY[currentMove][i + 1][j];
            frameAnimAngle[currentMove][i][j]     = frameAnimAngle[currentMove][i + 1][j];
            frameAnimScaleX[currentMove][i][j]    = frameAnimScaleX[currentMove][i + 1][j];
            frameAnimScaleY[currentMove][i][j]    = frameAnimScaleY[currentMove][i + 1][j];
            frameAnimShow[currentMove][i][j]      = frameAnimShow[currentMove][i + 1][j];
            frameAnimLayer[currentMove][i][j]     = frameAnimLayer[currentMove][i + 1][j];
            frameAnimHideLayer[currentMove][i][j] = frameAnimHideLayer[currentMove][i + 1][j];

            frameHotSpotX[currentMove][i][j]     = frameHotSpotX[currentMove][i + 1][j];
            frameHotSpotY[currentMove][i][j]     = frameHotSpotY[currentMove][i + 1][j];
            frameHotSpotAngle[currentMove][i][j] = frameHotSpotAngle[currentMove][i + 1][j];
            frameHotSpotShow[currentMove][i][j]  = frameHotSpotShow[currentMove][i + 1][j];
        }
        framesLengths[currentMove][i] = framesLengths[currentMove][i + 1];
    }
    framesCounts[currentMove]--;
    if (currentFrame >= framesCounts[currentMove]) {
        currentFrame--;
    }

    resetMode();
}

bool bodyNameChange(hgeGUIObject* sender) {
    delete bodyNames[selectedBody];
    bodyNames[selectedBody] = copyString(((hgeGUIEditableLabel*)sender)->getTitle());
}
bool resetBodyClick(hgeGUIObject* sender) {
    if (editMode == EDIT_MODE_FRAMES) {
        frameAnimAngle[currentMove][currentFrame][selectedBody] = 0.0f;
        frameAnimScaleX[currentMove][currentFrame][selectedBody] = 1.0f;
        frameAnimScaleY[currentMove][currentFrame][selectedBody] = 1.0f;
    }
    else if (editMode == EDIT_MODE_ANGLES) {
        angleAnimAngle[currentMove][currentFrame][selectedBody] = 0.0f;
        angleAnimScaleX[currentMove][currentFrame][selectedBody] = 1.0f;
        angleAnimScaleY[currentMove][currentFrame][selectedBody] = 1.0f;
    }
    resetMode();
}
bool closeBodyContextMenuClick(hgeGUIObject* sender) {
    resetMode();
}
bool closeFrameContextMenuClick(hgeGUIObject* sender) {
    resetMode();
}
bool frameLengthChange(hgeGUIObject* sender) {
    framesLengths[currentMove][currentFrame] = atof( ( (hgeGUIEditableLabel*)sender )->getTitle() );
    if (framesLengths[currentMove][currentFrame] < 0.05f) {
        framesLengths[currentMove][currentFrame] = 0.05f;
    }
}

bool closeActioncauseWindowClick(hgeGUIObject* sender) {
    actionCauseContextWindow->Hide();
    resetMode();
}
bool closeActioneffectWindowClick(hgeGUIObject* sender) {
    actionEffectContextWindow->Hide();
    resetMode();
}

bool insertMoveButtonClick(hgeGUIObject* sender) {
	movesCount += 1;
	currentMove = movesCount - 1;
	framesCounts[currentMove] = 1;
	moveNames[currentMove] = "<move>";
	currentFrame = 0;
	framesLengths[currentMove][0] = 0.5f;

	for (int i = 0; i < bodiesCount; i++) {
		frameAnimShow[currentMove][currentFrame][i] = false;
		frameAnimHideLayer[currentMove][currentFrame][i] = false;
		frameAnimX[currentMove][currentFrame][i] = 0;
		frameAnimY[currentMove][currentFrame][i] = 0;
		frameAnimLayer[currentMove][currentFrame][i] = i;
		frameAnimAngle[currentMove][currentFrame][i] = 0;
		frameAnimScaleX[currentMove][currentFrame][i] = 1.0f;
		frameAnimScaleY[currentMove][currentFrame][i] = 1.0f;
	}
	for (int i = 0; i < hotSpotsCount; i++) {
		frameHotSpotX[currentMove][currentFrame][i] = 0;
		frameHotSpotY[currentMove][currentFrame][i] = 0;
		frameHotSpotAngle[currentMove][currentFrame][i] = 0;
		frameHotSpotShow[currentMove][currentFrame][i] = false;
	}

	setMove(currentMove);
}
bool removeMoveClick(hgeGUIObject* sender) {

    delete moveNames[currentMove];

    for (int i = currentMove; i < movesCount - 1; i++) {
        for (int j = 0; j < framesCounts[i]; j++) {
            for (int k = 0; k < bodiesCount; k++) {
                frameAnimShow[i][j][k] = frameAnimShow[i + 1][j][k];
                frameAnimHideLayer[i][j][k] = frameAnimHideLayer[i + 1][j][k];
                frameAnimX[i][j][k] = frameAnimX[i + 1][j][k];
                frameAnimY[i][j][k] = frameAnimY[i + 1][j][k];
                frameAnimLayer[i][j][k] = frameAnimLayer[i + 1][j][k];
                frameAnimAngle[i][j][k] = frameAnimAngle[i + 1][j][k];
                frameAnimScaleX[i][j][k] = frameAnimScaleX[i + 1][j][k];
                frameAnimScaleY[i][j][k] = frameAnimScaleY[i + 1][j][k];

                frameHotSpotX[i][j][k] = frameHotSpotX[i + 1][j][k];
                frameHotSpotY[i][j][k] = frameHotSpotY[i + 1][j][k];
                frameHotSpotAngle[i][j][k] = frameHotSpotAngle[i + 1][j][k];
                frameHotSpotShow[i][j][k] = frameHotSpotShow[i + 1][j][k];
            }
        }
        delete moveNames[i];
        moveNames[i] = copyString(moveNames[i + 1]);
    }

    movesCount--;
    if (currentMove >= movesCount)
        currentMove = movesCount - 1;

	currentFrame = 0;
	resetMode();
}
bool duplicateMoveClick(hgeGUIObject* sender) {
	framesCounts[movesCount] = framesCounts[currentMove];
	angleCounts[movesCount]  = angleCounts[currentMove];

    delete moveNames[movesCount];
    moveNames[movesCount] = new char[256];
	sprintf(moveNames[movesCount], "%s copy", moveNames[currentMove]);

    for (int frame = 0; frame < framesCounts[currentMove]; frame++) {
        for (int i = 0; i < bodiesCount; i++) {
            frameAnimShow[movesCount][frame][i] = frameAnimShow[currentMove][frame][i];
            frameAnimHideLayer[movesCount][frame][i] = frameAnimHideLayer[currentMove][frame][i];
            frameAnimX[movesCount][frame][i] = frameAnimX[currentMove][frame][i];
            frameAnimY[movesCount][frame][i] = frameAnimY[currentMove][frame][i];
            frameAnimLayer[movesCount][frame][i] = frameAnimLayer[currentMove][frame][i];
            frameAnimAngle[movesCount][frame][i] = frameAnimAngle[currentMove][frame][i];
            frameAnimScaleX[movesCount][frame][i] = frameAnimScaleX[currentMove][frame][i];
            frameAnimScaleY[movesCount][frame][i] = frameAnimScaleY[currentMove][frame][i];
        }
        for (int i = 0; i < hotSpotsCount; i++) {
            frameHotSpotX[movesCount][frame][i]     = frameHotSpotX[currentMove][frame][i];
            frameHotSpotY[movesCount][frame][i]     = frameHotSpotY[currentMove][frame][i];
            frameHotSpotAngle[movesCount][frame][i] = frameHotSpotAngle[currentMove][frame][i];
            frameHotSpotShow[movesCount][frame][i]  = frameHotSpotShow[currentMove][frame][i];
        }
        framesLengths[movesCount][frame] = framesLengths[currentMove][frame];
    }

    for (int angle = 0; angle < angleCounts[currentMove]; angle++) {
        for (int i = 0; i < bodiesCount; i++) {
            angleAnimX[movesCount][angle][i] = angleAnimX[currentMove][angle][i];
            angleAnimY[movesCount][angle][i] = angleAnimY[currentMove][angle][i];
            angleAnimAngle[movesCount][angle][i] = angleAnimAngle[currentMove][angle][i];
            angleAnimScaleX[movesCount][angle][i] = angleAnimScaleX[currentMove][angle][i];
            angleAnimScaleY[movesCount][angle][i] = angleAnimScaleY[currentMove][angle][i];
            angleAnimShow[movesCount][angle][i] = angleAnimShow[currentMove][angle][i];
        }
        for (int i = 0; i < hotSpotsCount; i++) {
            angleHotSpotX[movesCount][angle][i] = angleHotSpotX[currentMove][angle][i];
            angleHotSpotY[movesCount][angle][i] = angleHotSpotY[currentMove][angle][i];
            angleHotSpotAngle[movesCount][angle][i] = angleHotSpotAngle[currentMove][angle][i];
            //angleHotSpotShow[currentMove][angleCounts[currentMove]][i] = angleHotSpotShow[currentMove][currentFrame][i];
        }
        angles[movesCount][angle] = angles[currentMove][angle];
    }

	setMove(movesCount);
	movesCount++;
	setFrame(0);
	resetMode();
}
bool closeMoveContextMenuClick(hgeGUIObject* sender) {
    moveContextWindow->Hide();
    resetMode();
}

bool hotSpotNameChange(hgeGUIObject* sender) {
    hotSpots[selectedHotSpot]->setName( ( (hgeGUIEditableLabel*)sender )->getTitle() );
}
bool closeHotSpotContextWindowClick(hgeGUIObject* sender) {
    resetMode();
}

bool insertFrameButtonClick(hgeGUIObject* sender) {
	for (int i = 0; i < bodiesCount; i++) {
		frameAnimX[currentMove][framesCounts[currentMove]][i] = frameAnimX[currentMove][currentFrame][i];
		frameAnimY[currentMove][framesCounts[currentMove]][i] = frameAnimY[currentMove][currentFrame][i];
		frameAnimAngle[currentMove][framesCounts[currentMove]][i] = frameAnimAngle[currentMove][currentFrame][i];
		frameAnimScaleX[currentMove][framesCounts[currentMove]][i] = frameAnimScaleX[currentMove][currentFrame][i];
		frameAnimScaleY[currentMove][framesCounts[currentMove]][i] = frameAnimScaleY[currentMove][currentFrame][i];
		frameAnimShow[currentMove][framesCounts[currentMove]][i] = frameAnimShow[currentMove][currentFrame][i];
		frameAnimLayer[currentMove][framesCounts[currentMove]][i] = frameAnimLayer[currentMove][currentFrame][i];
		frameAnimHideLayer[currentMove][framesCounts[currentMove]][i] = frameAnimHideLayer[currentMove][currentFrame][i];
	}
	framesLengths[currentMove][framesCounts[currentMove]] = 0.5f;
	for (int i = 0; i < hotSpotsCount; i++) {
		frameHotSpotX[currentMove][framesCounts[currentMove]][i] = frameHotSpotX[currentMove][currentFrame][i];
		frameHotSpotY[currentMove][framesCounts[currentMove]][i] = frameHotSpotY[currentMove][currentFrame][i];
		frameHotSpotAngle[currentMove][framesCounts[currentMove]][i] = frameHotSpotAngle[currentMove][currentFrame][i];
		frameHotSpotShow[currentMove][framesCounts[currentMove]][i] = frameHotSpotShow[currentMove][currentFrame][i];
	}

	currentFrame = framesCounts[currentMove];
	framesCounts[currentMove] += 1;
}
bool insertAngleButtonClick(hgeGUIObject* sender) {
	for (int i = 0; i < bodiesCount; i++) {
		angleAnimX[currentMove][angleCounts[currentMove]][i] = angleAnimX[currentMove][currentFrame][i];
		angleAnimY[currentMove][angleCounts[currentMove]][i] = angleAnimY[currentMove][currentFrame][i];
		angleAnimAngle[currentMove][angleCounts[currentMove]][i] = angleAnimAngle[currentMove][currentFrame][i];
		angleAnimScaleX[currentMove][angleCounts[currentMove]][i] = angleAnimScaleX[currentMove][currentFrame][i];
		angleAnimScaleY[currentMove][angleCounts[currentMove]][i] = angleAnimScaleY[currentMove][currentFrame][i];
		angleAnimShow[currentMove][angleCounts[currentMove]][i] = angleAnimShow[currentMove][currentFrame][i];
	}
	angles[currentMove][angleCounts[currentMove]] = angles[currentMove][currentFrame];
	for (int i = 0; i < hotSpotsCount; i++) {
		angleHotSpotX[currentMove][angleCounts[currentMove]][i] = angleHotSpotX[currentMove][currentFrame][i];
		angleHotSpotY[currentMove][angleCounts[currentMove]][i] = angleHotSpotY[currentMove][currentFrame][i];
		angleHotSpotAngle[currentMove][angleCounts[currentMove]][i] = angleHotSpotAngle[currentMove][currentFrame][i];
		//angleHotSpotShow[currentMove][angleCounts[currentMove]][i] = angleHotSpotShow[currentMove][currentFrame][i];
	}

	currentFrame = angleCounts[currentMove];
	angleCounts[currentMove] += 1;
}
//} События

//} Интерфейс

/// Глобальные функции
//{
bool FrameFunc() {
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

	if (mode < MODE_MOVES_INDEX) {

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
                if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && !game->getHge()->Input_GetKeyState(HGEK_SHIFT) && !game->getHge()->Input_GetKeyState(HGEK_CTRL)) {
                    if (x < 50 && y > 110 && y < 110 + layerHeight * bodiesCount) {
                        selectedLayer = floor((bodiesCount * layerHeight - (y - 110)) / layerHeight);
                        mode = MODE_LAYER_DRAG;
                    }
                    if (y > 100 && x > 50) {
                        selectedBody = getPointedBody(x, y);
                        selectedHotSpot = getPointedHotSpot(x, y);
                        if (selectedBody != -1) {
                            if ( animShow(selectedBody) ) {
                                selectedBodyX = animX(selectedBody);
                                selectedBodyY = animY(selectedBody);
                                selectedBodyAngle = animAngle(selectedBody);
                                dragOffsetX = x - game->screenX(animX(selectedBody));
                                dragOffsetY = y - game->screenY(animY(selectedBody));
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
                        if (anim < movesCount)
                            setMove(anim);
                        if (anim == movesCount) {
                            insertMoveButtonClick(NULL);
                        }
                    }
                    if (y > 50 && y < 100 && x < 1300) {
                        if (editMode == EDIT_MODE_FRAMES) {
                            float leftX = 0;
                            float rightX = 0;
                            for (int i = 0; i < framesCounts[currentMove]; i++) {
                                rightX = leftX + 100 * framesLengths[currentMove][i];
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
                            for (int i = 0; i < angleCounts[currentMove]; i++) {
                                rightX = leftX + 50;
                                if (x > leftX && x < rightX) {
                                    setFrame(i, true);
                                }
                                leftX = rightX;
                            }
                            if (x > angleCounts[currentMove] * 50 && x < angleCounts[currentMove] * 50 + 50) {
                                insertAngleButtonClick(NULL);
                            }
                        }
                    }
                    if (editMode == EDIT_MODE_ANGLES) {
                        if ((b2Vec2(worldX, worldY) - b2Vec2(cos(angles[currentMove][currentFrame]) * 2.9f, sin(angles[currentMove][currentFrame]) * 2.9f)).Length() < 0.1f) {
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
                    if ( selectedBody != -1 && animShow(selectedBody) ) {
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
                if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && game->getHge()->Input_GetKeyState(HGEK_CTRL)) {
                    selectedBody = getPointedBody(x, y);
                    if ( selectedBody != -1 && animShow(selectedBody) ) {
                        //mainWindow->Hide();
                        dragOffsetX = x - game->screenX(animX(selectedBody));
                        dragOffsetY = y - game->screenY(animY(selectedBody));
                        mode = MODE_ANIM_RESIZE;
                    }
                }
                if (game->getHge()->Input_KeyDown(HGEK_RBUTTON)) {

                    if (x < 50 && y > 110 && y < 110 + layerHeight * bodiesCount) {
                        int layer = floor((bodiesCount * layerHeight - (y - 110)) / layerHeight);
                        frameAnimHideLayer[currentMove][currentFrame][frameAnimLayer[currentMove][currentFrame][layer]]
                            = !frameAnimHideLayer[currentMove][currentFrame][frameAnimLayer[currentMove][currentFrame][layer]];
                        ;
                    }
                    else if (y > 50 && y < 100 && x < 1300) {
                        if (editMode == EDIT_MODE_FRAMES) {
                            float leftX = 0;
                            float rightX = 0;
                            for (int i = 0; i < framesCounts[currentMove]; i++) {
                                rightX = leftX + 100 * framesLengths[currentMove][i];
                                if (x > leftX && x < rightX) {
                                    mode = MODE_FRAME_CONTEXT_MENU;
                                    setFrame(i, true);
                                    showFrameContextWindow();
                                }
                                leftX = rightX;
                            }
                        }
                    }
                    else if (y < 50 && x < 1300) {
                        int anim = floor(x / 50);
                        if (anim < movesCount) {
                            mode = MODE_MOVE_CONTEXT_MENU;
                            setMove(anim);
                            showMoveContextWindow();
                        }
                    }
                    else {
                        selectedBody = getPointedBody(x, y);
                        selectedHotSpot = getPointedHotSpot(x, y);
                        if (selectedBody != -1) {
                            mode = MODE_ANIM_CONTEXT_MENU;
                            animContextMenuWindow->Move(x, y);
                            animContextMenuWindow->Show();
                            ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(51) )->setTitle(bodyNames[selectedBody]);
                        } else if (selectedHotSpot != -1 && hsShow(selectedHotSpot)) {
                            mode = MODE_HOTSPOT_CONTEXT_MENU;
                            hotSpotContextWindow->Move(x, y);
                            hotSpotContextWindow->Show();
                            ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(81) )->setTitle( hotSpots[selectedHotSpot]->getName() );
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
                    //frameAnimX[currentMove][currentFrame][selectedBody] = currentX;
                    //frameAnimY[currentMove][currentFrame][selectedBody] = currentY;

                    if (game->getHge()->Input_KeyUp(HGEK_UP)) {
                        frameAnimLayer[currentMove][currentFrame][selectedBody] += 1;
                        if (frameAnimLayer[currentMove][currentFrame][selectedBody] > 1000)
                            frameAnimLayer[currentMove][currentFrame][selectedBody] = 1000;
                        //animations[selectedBody]->SetZ(1.0f - 0.001 * frameAnimLayer[currentMove][currentFrame][selectedBody]);
                        printf("layer %f\n", animations[selectedBody]->GetZ());
                    }
                    if (game->getHge()->Input_KeyUp(HGEK_DOWN)) {
                        frameAnimLayer[currentMove][currentFrame][selectedBody] -= 1;
                        if (frameAnimLayer[currentMove][currentFrame][selectedBody] < 0)
                            frameAnimLayer[currentMove][currentFrame][selectedBody] = 0;
                        //animations[selectedBody]->SetZ(1.0f - 0.001 * frameAnimLayer[currentMove][currentFrame][selectedBody]);
                        printf("layer %f\n", animations[selectedBody]->GetZ());
                    }
                    if (currentX < 1300) {
                        setAnimX(selectedBody, game->worldX(currentX));
                        setAnimY(selectedBody, game->worldY(currentY));
                        setAnimAngle(selectedBody, selectedBodyAngle);
                        setAnimShow(selectedBody, true);
                    } else {
                        setAnimX(selectedBody, selectedBodyX);
                        setAnimY(selectedBody, selectedBodyY);
                        setAnimShow(selectedBody, false);
                        animationX[selectedBody] = currentX - 1300;
                        animationY[selectedBody] = currentY + 1000 * currentTab - 450;
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
                        dragOffsetAngle = atan2(worldY - dragOffsetY, worldX - dragOffsetX) - frameAnimAngle[currentMove][currentFrame][selectedBody];
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
            case MODE_ANIM_RESIZE:
                if (selectedBody != -1) {
                    float newOffsetX = x - game->screenX(animX(selectedBody));
                    float newOffsetY = y - game->screenY(animY(selectedBody));
                    setAnimScaleX(selectedBody, animScaleX(selectedBody) * newOffsetX / dragOffsetX);
                    setAnimScaleY(selectedBody, animScaleY(selectedBody) * newOffsetY / dragOffsetY);
                    dragOffsetX = newOffsetX;
                    dragOffsetY = newOffsetY;
                }
                if (game->getHge()->Input_KeyUp(HGEK_LBUTTON) || game->getHge()->Input_KeyUp(HGEK_CTRL)) {
                    resetMode();
                }
                break;
            case MODE_INSERT_ANIM:
                if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
                    animationX[bodiesCount] = x - 1300;
                    animationY[bodiesCount] = y - 450 + 1000 * currentTab;
                    animations[bodiesCount] = beingInsertedAnim;

                    frameAnimX[currentMove][currentFrame][bodiesCount] = 0;
                    frameAnimY[currentMove][currentFrame][bodiesCount] = 0;
                    frameAnimAngle[currentMove][currentFrame][bodiesCount] = 0;
                    frameAnimScaleX[currentMove][currentFrame][bodiesCount] = 1.0f;
                    frameAnimScaleY[currentMove][currentFrame][bodiesCount] = 1.0f;
                    frameAnimShow[currentMove][currentFrame][bodiesCount] = false;
                    frameAnimHideLayer[currentMove][currentFrame][bodiesCount] = false;
                    frameAnimLayer[currentMove][currentFrame][bodiesCount] = bodiesCount;

                    animationNames[bodiesCount] = beingInsertedAnimName;
                    bodyNames[bodiesCount] = getFileName(beingInsertedAnimName);

                    bodiesCount++;
                    resetMode();
                }
                break;
            case MODE_LAYER_DRAG:
                if (bodiesCount * layerHeight - (y - 110) > 0 && bodiesCount * layerHeight - (y - 110) < selectedLayer * layerHeight) {
                    int oldAnimNum = frameAnimLayer[currentMove][currentFrame][selectedLayer];
                    frameAnimLayer[currentMove][currentFrame][selectedLayer] = frameAnimLayer[currentMove][currentFrame][selectedLayer - 1];
                    frameAnimLayer[currentMove][currentFrame][selectedLayer - 1] = oldAnimNum;
                    selectedLayer -= 1;
                }
                if (bodiesCount * layerHeight - (y - 110) < bodiesCount * layerHeight && bodiesCount * layerHeight - (y - 110) > selectedLayer * layerHeight + layerHeight) {
                    int oldAnimNum = frameAnimLayer[currentMove][currentFrame][selectedLayer];
                    frameAnimLayer[currentMove][currentFrame][selectedLayer] = frameAnimLayer[currentMove][currentFrame][selectedLayer + 1];
                    frameAnimLayer[currentMove][currentFrame][selectedLayer + 1] = oldAnimNum;
                    selectedLayer += 1;
                }
                if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
                    resetMode();
                }
                break;
            case MODE_PLAYING:
                currentTime += game->getHge()->Timer_GetDelta();
                time = 0;
                for (int i = 0; i < framesCounts[currentMove]; i++) {
                    if (currentTime > time) {
                        frame = i;
                        frameProgress = (currentTime - time) / framesLengths[currentMove][i];
                    }
                    time += framesLengths[currentMove][i];
                }
                setFrame(frame);
                if (currentTime > time)
                    currentTime -= time;

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
                    frameHotSpotX[currentMove][currentFrame][hotSpotsCount] = 0.5f * (right + left);
                    frameHotSpotY[currentMove][currentFrame][hotSpotsCount] = 0.5f * (bottom + top);
                    frameHotSpotAngle[currentMove][currentFrame][hotSpotsCount] = 0;
                    frameHotSpotShow[currentMove][currentFrame][hotSpotsCount] = true;

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
//                    resetMode();
                }
                break;
            case MODE_ANGLE_DRAG:
                angles[currentMove][currentFrame] = atan2f(worldY, worldX);
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

	}
	else {
        int changeMove = -1;
        if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && x > 685 && x < 700 && y > 98 + actionsCounts[currentMove] * 75 && y < 128 + actionsCounts[currentMove] * 75) {
            addAction();
        } else {
            for (int i = 0; i < actionsCounts[currentMove]; i++) {
                if (mode == MODE_MOVES_INDEX && x > 700 && x < 1300 && y > 100 + i * 75 && y < 175 + i * 75) {

                }

                if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && mode == MODE_MOVES_INDEX && x > 940 && x < 950
                    && y > 100 + i * 75 + actions[currentMove][i]->getCausesCount() * 12 && y < 112 + i * 75 + actions[currentMove][i]->getCausesCount() * 12
                ) {
                    printf("add cause\n");
                    actions[currentMove][i]->addCause();
                }
                else {

                    for (int j = 0; j < actions[currentMove][i]->getCausesCount(); j++) {
                        if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && mode == MODE_MOVES_INDEX && y > 100 + i * 75 + j * 12 && y < 112 + i * 75 + j * 12) {
                            if (x > 700 && x < 940) {
                                mode = MODE_MOVE_ACTIONCAUSE_EDIT;
                                selectedAction = i;
                                selectedActionCoE = j;
                                resetActioncauseWindow();
                            }
                            else if (x > 940 && x < 950) {
                                actions[currentMove][i]->removeCause(j);
                            }
                        }
                    }
                }


                if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && mode == MODE_MOVES_INDEX && mode == MODE_MOVES_INDEX && x > 1290 && x < 1300
                    && y > 100 + i * 75 + actions[currentMove][i]->getEffectsCount() * 12 && y < 112 + i * 75 + actions[currentMove][i]->getEffectsCount() * 12
                ) {
                    printf("add effect\n");
                    actions[currentMove][i]->addEffect();
                }
                else {
                    for (int j = 0; j < actions[currentMove][i]->getEffectsCount(); j++) {
                        int move = (int)actions[currentMove][i]->getEffect(j)->getParam();
                        if (actions[currentMove][i]->getEffect(j)->getType() == ACTIONEFFECT_TYPE_CHANGEMOVE) {

                            if (mode == MODE_MOVES_INDEX && x > 975 && x < 1025 && y > 100 + i * 75 && y < 150 + i * 75) {
                                if (game->getHge()->Input_KeyUp(HGEK_LBUTTON)) {
                                    changeMove = actions[currentMove][i]->getEffect(j)->getParam();
                                }
                            }

                        }

                        if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && mode == MODE_MOVES_INDEX && y > 100 + i * 75 + j * 12 && y < 112 + i * 75 + j * 12) {

                            if (x > 1050 && x < 1290) {
                                mode = MODE_MOVE_ACTIONEFFECT_EDIT;
                                selectedAction = i;
                                selectedActionCoE = j;
                                resetActioneffectWindow();
                            }
                            else if (x > 1290 && x < 1300) {
                                actions[currentMove][i]->removeEffect(j);
                            }
                        }
                    }
                }

                if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && mode == MODE_MOVES_INDEX && x > 685 && x < 700 && y > 98 + i * 75 && y < 128 + i * 75) {
                    removeAction(i);
                }
            }
            if ( changeMove > -1 ) {
                setMove( changeMove );
            }
        }
	}

	return game->update(false);
}

bool RenderFunc() {
	game->startDraw();

    ///Положение мышки на экране и в мире
    float x, y;
    game->getHge()->Input_GetMousePos(&x, &y);
    float worldX = game->worldX(x);
    float worldY = game->worldY(y);
    ///Добавление анимации в пул
    float insertX = x; float insertY = y;
    if (insertX < 1300) insertX = 1300; if (insertY < 450) insertY = 450;

    float miniatureScale = 50.0f / std::max(characterHeight, characterWidth);

    if (mode < MODE_MOVES_INDEX) {
        ///Прямоугольник, показывающий размер персонажа в мире
        game->drawRect(game->screenX(0), game->screenY(0), characterWidth * 0.5 * game->getFullScale(), characterHeight * 0.5 * game->getFullScale(), 0, 0x55000000, 0);
        if (sqrt(pow(x - game->screenX(characterWidth * 0.5), 2) + pow(y - game->screenY(-characterHeight * 0.5), 2)) < 5) {
            resizeIcon->Render(x, y);///Нарисуем иконку, если мышка возле правого верхнего угла
        }

        for (int index = 0; index < bodiesCount; index++) {
            int i = animLayer(index);
            //if (frameAnimShow[currentMove][currentFrame][i] && !frameAnimHideLayer[currentMove][currentFrame][i])
            if (animShow(i) && !animHideLayer(i)) {
                DWORD color = 0xFFFFFFFF;
                if (mode != MODE_PLAYING && (
                        (editMode == EDIT_MODE_FRAMES && animRotating[currentMove][i]) ||
                        (editMode == EDIT_MODE_ANGLES && !animRotating[currentMove][i])
                    )
                ) {
                    color = 0xAAAAAAAA;
                }
                animations[i]->SetColor(color);
                animations[i]->RenderEx(
                    game->screenX(animX(i)),
                    game->screenY(animY(i)),
                    animAngle(i),
                    animScaleX(i) * game->getScaleFactor(),
                    animScaleY(i) * game->getScaleFactor()
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
                        animScaleX(i) * game->getScaleFactor(),
                        animScaleY(i) * game->getScaleFactor(),
                        bb
                    );

                    game->getHge()->Gfx_RenderLine(bb->x1, bb->y1, bb->x2, bb->y1, color);
                    game->getHge()->Gfx_RenderLine(bb->x2, bb->y1, bb->x2, bb->y2, color);
                    game->getHge()->Gfx_RenderLine(bb->x2, bb->y2, bb->x1, bb->y2, color);
                    game->getHge()->Gfx_RenderLine(bb->x1, bb->y2, bb->x1, bb->y1, color);

                    if (mode == MODE_ANIM_RESIZE && i == selectedBody) {
                        arial12->SetColor(0xFF000000);
                        arial12->printf( bb->x2, bb->y2, HGETEXT_LEFT, "%.2fx%.2f", animScaleX(i), animScaleY(i) );
                    }
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
                        (editMode == EDIT_MODE_FRAMES && hotSpotRotating[currentMove][i]) ||
                        (editMode == EDIT_MODE_ANGLES && !hotSpotRotating[currentMove][i])
                    )
                ) {
                    color = 0xAAAA0000; fillcolor = 0x55AA0000;
                }
                game->drawPoly(
                    (b2PolygonShape*)hotSpots[i]->getShape(),
                    trans,
                    -game->getPixelsPerMeter() * game->getScaleFactor() * game->getCameraPos() + 0.5f * b2Vec2(game->getScreenWidth(), game->getScreenHeight()),
                    game->getPixelsPerMeter() * game->getScaleFactor(),
                    color, fillcolor
                );

            }
        }

        switch (mode) {
            case MODE_INSERT_ANIM:
                break;
            case MODE_ANIM_ROTATE:
            case MODE_HOTSPOT_ROTATE:
                game->getHge()->Gfx_RenderLine(game->screenX(dragOffsetX), game->screenY(dragOffsetY), x, y, 0xFFffa500);
                arial12->SetColor(0xFF000000);
                arial12->printf(x, y - 16.0f, HGETEXT_LEFT, "%.2f\xB0", animAngle(selectedBody) * 180.0f / M_PI);
                break;
            case MODE_ADD_HOTSPOT_BOX_STEP2:
                game->drawRect(game->screenX(dragOffsetX), game->screenY(dragOffsetY), x, y, 0xFFFF0000, 0xAAFF0000);
        }

        game->drawRect(0, 0, 1300, 50, 0xFF000000, 0xFFFFFFFF);
        for (int i = 0; i < movesCount; i++) {
            DWORD color = 0xFF000000;
            if (i == currentMove)
                color = 0xFFFF0000;
            game->drawRect(1 + 50 * i, 1, 50 + 50 * i, 49, color, 0);
            for (int index = 0; index < bodiesCount; index++) {
                int j = frameAnimLayer[i][0][index];
                if (frameAnimShow[i][0][j]) {
                    animations[j]->RenderEx(
                        25 + i * 50 + frameAnimX[i][0][j] * miniatureScale,
                        25 + frameAnimY[i][0][j] * miniatureScale,
                        frameAnimAngle[i][0][j],
                        frameAnimScaleX[i][0][j] * miniatureScale / game->getPixelsPerMeter(),
                        frameAnimScaleY[i][0][j] * miniatureScale / game->getPixelsPerMeter()
                    );
                } else {
                }
            }
        }

        game->drawRect(1 + movesCount * 50, 1, 50 + movesCount * 50, 49, 0xFF00AA00, 0xFFFFFFFF);
        game->drawRect(6 + movesCount * 50, 23, 44 + movesCount * 50, 27, 0xFF00AA00, 0xFF00AA00);
        game->drawRect(23 + movesCount * 50, 6, 27 + movesCount * 50, 44, 0xFF00AA00, 0xFF00AA00);

        game->drawRect(0, 50, 1300, 100, 0xFF000000, 0xFFFFFFFF);

        if (editMode == EDIT_MODE_FRAMES) {
            float leftX = 0;
            float midX = 0;
            float rightX = 0;
            for (int i = 0; i < framesCounts[currentMove]; i++) {
                DWORD color = 0xFF000000;
                if (i == currentFrame)
                    color = 0xFFFF0000;
                midX = leftX + 50 * framesLengths[currentMove][i];
                rightX = leftX + 100 * framesLengths[currentMove][i];
                game->drawRect(1 + leftX, 51, rightX, 99, color, 0xFFFFFFFF);

                for (int index = 0; index < bodiesCount; index++) {
                    int j = frameAnimLayer[currentMove][i][index];
                    if (frameAnimShow[currentMove][i][j]) {
                        animations[j]->RenderEx(
                            midX + frameAnimX[currentMove][i][j] * miniatureScale,
                            75 + frameAnimY[currentMove][i][j] * miniatureScale,
                            frameAnimAngle[currentMove][i][j],
                            frameAnimScaleX[currentMove][i][j] * miniatureScale / game->getPixelsPerMeter(),
                            frameAnimScaleY[currentMove][i][j] * miniatureScale / game->getPixelsPerMeter()
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
            for (int i = 0; i < angleCounts[currentMove]; i++) {
                DWORD color = 0xFF000000;
                if (i == currentFrame)
                    color = 0xFFFF0000;

                game->drawRect(0 + i * 50, 51, 50 + i * 50, 99, color, 0xFFFFFFFF);

                b2Vec2* arrow = new b2Vec2[4];
                arrow[0] = b2Vec2(25 + i * 50, 75);
                arrow[1] = b2Vec2(25 + i * 50 + cos(angles[currentMove][i] + 0.25f) * 20, 75 + sin(angles[currentMove][i] + 0.25f) * 20);
                arrow[2] = b2Vec2(25 + i * 50 + cos(angles[currentMove][i]) * 24, 75 + sin(angles[currentMove][i]) * 24);
                arrow[3] = b2Vec2(25 + i * 50 + cos(angles[currentMove][i] - 0.25f) * 20, 75 + sin(angles[currentMove][i] - 0.25f) * 20);
                game->DrawPolygonScreen(arrow, 4, color, color);
                delete arrow;
            }

            game->drawRect(0 + angleCounts[currentMove] * 50, 51, 50 + 50 * angleCounts[currentMove], 99, 0xFF00AA00, 0xFFFFFFFF);
            game->drawRect(6 + angleCounts[currentMove] * 50, 73, 44 + angleCounts[currentMove] * 50, 77, 0xFF00AA00, 0xFF00AA00);
            game->drawRect(23 + angleCounts[currentMove] * 50, 56, 27 + angleCounts[currentMove] * 50, 94, 0xFF00AA00, 0xFF00AA00);

            b2Vec2* arrow = new b2Vec2[4];
            arrow[0] = b2Vec2(cos(angles[currentMove][currentFrame]) * 3.3, sin(angles[currentMove][currentFrame]) * 3.3);
            arrow[1] = b2Vec2(cos(angles[currentMove][currentFrame] + 0.2f) * (3 - 0.1f), sin(angles[currentMove][currentFrame] + 0.2f) * (2.9f));
            arrow[2] = b2Vec2(cos(angles[currentMove][currentFrame]) * (3), sin(angles[currentMove][currentFrame]) * (3));
            arrow[3] = b2Vec2(cos(angles[currentMove][currentFrame] - 0.2f) * (3 - 0.1f), sin(angles[currentMove][currentFrame] - 0.2f) * (2.9f));

            game->DrawPolygon(arrow, 4, 0xFF000000, 0x55000000);
            game->DrawCircle(b2Vec2(cos(angles[currentMove][currentFrame]) * 2.9f, sin(angles[currentMove][currentFrame]) * 2.9f), 0.1f, 0xFF000000, 0x55000000);
            delete arrow;
        }

        float layerHeight = 50;
        if (bodiesCount > 16) {
            layerHeight = 800 / bodiesCount;
        }

        arial12->SetColor(0xFF000000);
        for (int index = 0; index < bodiesCount; index++) {
            int i = frameAnimLayer[currentMove][currentFrame][index];
            if (index != selectedLayer) {
                DWORD color = 0xFF000000;
                if (i == selectedBody) {
                    color = 0xFFFF0000;
                }
                DWORD bgcolor = 0xFFFFFFFF;
                if (!frameAnimShow[currentMove][currentFrame][i]) {
                    bgcolor = 0xFFAAAAAA;
                }
                if (frameAnimHideLayer[currentMove][currentFrame][i]) {
                    bgcolor = 0xFFAA0000;
                }
                game->drawRect(1, 60 + layerHeight * (bodiesCount - index), 50, 109 + layerHeight * (bodiesCount - index), color, bgcolor);
                animations[i]->RenderEx(
                    25,
                    85 + layerHeight * (bodiesCount - index),
                    frameAnimAngle[currentMove][currentFrame][i],
                    miniatureScale / game->getPixelsPerMeter(),
                    miniatureScale / game->getPixelsPerMeter()
                );
                arial12->Render(52, 77 + layerHeight * (bodiesCount - index), HGETEXT_LEFT, bodyNames[i]);
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
            animations[frameAnimLayer[currentMove][currentFrame][selectedLayer]]->RenderEx(
                75,
                selectedLayerY,
                frameAnimAngle[currentMove][currentFrame][selectedLayer],
                miniatureScale / game->getPixelsPerMeter(),
                miniatureScale / game->getPixelsPerMeter()
            );
        }

        game->getHge()->Gfx_RenderLine(1300, 0, 1300, 900, 0xFF000000);
        game->getHge()->Gfx_RenderLine(1300, 450, 1600, 450, 0xFF000000);

        game->drawRect(1300, 420, 1600, 900, 0xFF000000, 0xFFCCCCCC);
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
    }
    else {
        game->drawRect(0, 0, 1300, 900, 0xFF000000, 0xFFFFFFFF);

        game->drawRect(575, 425, 625, 475, 0xFF000000, 0);
        for (int index = 0; index < bodiesCount; index++) {
            int j = frameAnimLayer[currentMove][0][index];
            if (frameAnimShow[currentMove][0][j]) {
                animations[j]->RenderEx(
                    600 + frameAnimX[currentMove][0][j] * miniatureScale,
                    450 + frameAnimY[currentMove][0][j] * miniatureScale,
                    frameAnimAngle[currentMove][0][j],
                    frameAnimScaleX[currentMove][0][j] * miniatureScale / game->getPixelsPerMeter(),
                    frameAnimScaleY[currentMove][0][j] * miniatureScale / game->getPixelsPerMeter()
                );
            } else {
            }
        }

        arial12->SetColor(0xFF000000);
        for (int i = 0; i < actionsCounts[currentMove]; i++) {
            if (mode == MODE_MOVES_INDEX && x > 700 && x < 1300 && y > 100 + i * 75 && y < 175 + i * 75) {
                game->drawRect(700, 98 + i * 75, 1299, 173 + i * 75, 0xFF0000AA, 0);
            }
            for (int j = 0; j < actions[currentMove][i]->getCausesCount(); j++) {
                char text[50];
                switch (actions[currentMove][i]->getCause(j)->getType()) {
                    case ACTIONCAUSE_TYPE_NONE:
                        sprintf(text, "none");
                        break;
                    case ACTIONCAUSE_TYPE_ONGROUND:
                        if (actions[currentMove][i]->getCause(j)->getParam())
                            sprintf(text, "not on ground");
                        else
                            sprintf(text, "on ground");
                        break;
                    case ACTIONCAUSE_TYPE_KEYDOWN:
                    case ACTIONCAUSE_TYPE_KEYHIT:
                    case ACTIONCAUSE_TYPE_KEYRELEASED:
                        sprintf(
                            text, "%s: %s",
                            actionCauses[actions[currentMove][i]->getCause(j)->getType()],
                            keyNames[(int)actions[currentMove][i]->getCause(j)->getParam()]
                        );
                        break;
                    case ACTIONCAUSE_TYPE_NPC_TARGETSIDE:
                        switch ((int)actions[currentMove][i]->getCause(j)->getParam()) {
                            case TARGETSIDE_INFRONT:
                                sprintf(text, "target in front");
                                break;
                            case TARGETSIDE_BEHIND:
                                sprintf(text, "target behind");
                                break;
                            case TARGETSIDE_UNDEFINED:
                                sprintf(text, "target side undefined");
                                break;
                        }
                        break;
                    default:
                        sprintf(
                            text, "%s %.2f",
                            actionCauses[actions[currentMove][i]->getCause(j)->getType()],
                            actions[currentMove][i]->getCause(j)->getParam()
                        );
                        break;
                }

                if (mode == MODE_MOVES_INDEX && x > 700 && x < 940 && y > 100 + i * 75 + j * 12 && y < 112 + i * 75 + j * 12) {
                    game->drawRect(700, 99 + i * 75 + j * 12, 950, 112 + i * 75 + j * 12, 0xFF0000AA, 0);
                }
                if (mode == MODE_MOVE_ACTIONCAUSE_EDIT && selectedAction == i && selectedActionCoE == j) {
                    game->drawRect(700, 99 + i * 75 + j * 12, 950, 112 + i * 75 + j * 12, 0xFF0000AA, 0x440000AA);
                }

                int color = 0xFF000000;
                if (mode == MODE_MOVES_INDEX && x > 940 && x < 950 && y > 100 + i * 75 + j * 12 && y < 112 + i * 75 + j * 12) {
                    game->drawRect(700, 99 + i * 75 + j * 12, 950, 112 + i * 75 + j * 12, 0xFFFF0000, 0);
                    color = 0xFFFF0000;

                }
                arial12->SetColor(color);
                arial12->Render(700, 96 + i * 75 + j * 12, HGETEXT_LEFT, text);
                arial12->Render(941, 96 + i * 75 + j * 12, HGETEXT_LEFT, "-");

                arial12->SetColor(0xFF000000);
            }

            int color = 0xFF000000;
            if (mode == MODE_MOVES_INDEX && x > 940 && x < 950
                && y > 100 + i * 75 + actions[currentMove][i]->getCausesCount() * 12 && y < 112 + i * 75 + actions[currentMove][i]->getCausesCount() * 12) {
                color = 0xFF00AA00;
                game->drawRect(700, 99 + i * 75 + actions[currentMove][i]->getCausesCount() * 12, 950, 112 + i * 75 + actions[currentMove][i]->getCausesCount() * 12, 0xFF00AA00, 0);
            }
            arial12->SetColor(color);
            arial12->Render(940, 96 + i * 75 + actions[currentMove][i]->getCausesCount() * 12, HGETEXT_LEFT, "+");

            arial12->SetColor(0xFF000000);

            for (int j = 0; j < actions[currentMove][i]->getEffectsCount(); j++) {
                int move = (int)actions[currentMove][i]->getEffect(j)->getParam();
                if (actions[currentMove][i]->getEffect(j)->getType() == ACTIONEFFECT_TYPE_CHANGEMOVE) {
                    DWORD color = 0xFF000000;
                    DWORD bgcolor = 0;
                    if (mode == MODE_MOVES_INDEX && x > 975 && x < 1025 && y > 100 + i * 75 && y < 150 + i * 75) {
                        color = 0xFF0000AA;
                        bgcolor = 0x330000AA;
                    }
                    game->drawRect(975, 100 + i * 75, 1025, 150 + i * 75, color, bgcolor);
                    for (int index = 0; index < bodiesCount; index++) {
                        int j = frameAnimLayer[move][0][index];
                        if (frameAnimShow[move][0][j]) {
                            animations[j]->RenderEx(
                                1000 + frameAnimX[move][0][j] * miniatureScale,
                                125 + i * 75 + frameAnimY[move][0][j] * miniatureScale,
                                frameAnimAngle[move][0][j],
                                frameAnimScaleX[move][0][j] * miniatureScale / game->getPixelsPerMeter(),
                                frameAnimScaleY[move][0][j] * miniatureScale / game->getPixelsPerMeter()
                            );
                        } else {
                        }
                    }
                    arial12->printf(
                        1000, 150 + i * 75, HGETEXT_CENTER, "%d: %s",
                        move, moveNames[move]
                    );
                }
                char text[50];
                switch (actions[currentMove][i]->getEffect(j)->getType()) {
                    case ACTIONEFFECT_TYPE_TURN:
                        sprintf(
                            text, "turn"
                        );
                        break;
                    case ACTIONEFFECT_TYPE_JUMP:
                        sprintf(
                            text, "jump %.2f up %.2f forth",
                            actions[currentMove][i]->getEffect(j)->getParam(1),
                            actions[currentMove][i]->getEffect(j)->getParam(2)
                        );
                        break;
                    case ACTIONEFFECT_TYPE_RUN:
                        sprintf(
                            text, "run %.2f",
                            actions[currentMove][i]->getEffect(j)->getParam(1)
                        );
                        break;
                    case ACTIONEFFECT_TYPE_SPAWN_EFFECT:
                        if (game->getEffectPrototype((int)actions[currentMove][i]->getEffect(j)->getParam(1))) {
                            sprintf(
                                text, "spawn effect #%d: %s",
                                (int)actions[currentMove][i]->getEffect(j)->getParam(1),
                                game->getEffectPrototype((int)actions[currentMove][i]->getEffect(j)->getParam(1))->getName()
                            );
                        }
                        else {
                            sprintf(
                                text, "%d IS WRONG EFFECT INDEX",
                                (int)actions[currentMove][i]->getEffect(j)->getParam(1)
                            );
                        }
                        break;
                    case ACTIONEFFECT_TYPE_CHANGEMOVE:
                        sprintf(
                            text, "set move to #%d: %s",
                            move,
                            moveNames[move]
                        );
                        break;
                    default:
                        sprintf(
                            text, "%s %.1f %.1f",
                            actionEffects[actions[currentMove][i]->getEffect(j)->getType()],
                            actions[currentMove][i]->getEffect(j)->getParam(1),
                            actions[currentMove][i]->getEffect(j)->getParam(2)
                        );
                        break;
                }

                if (mode == MODE_MOVES_INDEX && x > 1050 && x < 1290 && y > 100 + i * 75 + j * 12 && y < 112 + i * 75 + j * 12) {
                    game->drawRect(1050, 99 + i * 75 + j * 12, 1300, 112 + i * 75 + j * 12, 0xFF0000AA, 0);
                }
                if (mode == MODE_MOVE_ACTIONEFFECT_EDIT && selectedAction == i && selectedActionCoE == j) {
                    game->drawRect(1050, 99 + i * 75 + j * 12, 1300, 112 + i * 75 + j * 12, 0xFF0000AA, 0x440000AA);
                }

                color = 0xFF000000;
                if (mode == MODE_MOVES_INDEX && x > 1290 && x < 1300 && y > 100 + i * 75 + j * 12 && y < 112 + i * 75 + j * 12) {
                    game->drawRect(1050, 99 + i * 75 + j * 12, 1300, 112 + i * 75 + j * 12, 0xFFFF0000, 0);
                    color = 0xFFFF0000;
                }
                arial12->SetColor(color);
                arial12->Render(1050, 96 + i * 75 + j * 12, HGETEXT_LEFT, text);
                arial12->Render(1291, 96 + i * 75 + j * 12, HGETEXT_LEFT, "-");

                arial12->SetColor(0xFF000000);
            }

            color = 0xFF000000;
            if (mode == MODE_MOVES_INDEX && x > 1290 && x < 1300
                && y > 100 + i * 75 + actions[currentMove][i]->getEffectsCount() * 12 && y < 112 + i * 75 + actions[currentMove][i]->getEffectsCount() * 12) {
                game->drawRect(1050, 99 + i * 75 + actions[currentMove][i]->getEffectsCount() * 12, 1300, 112 + i * 75 + actions[currentMove][i]->getEffectsCount() * 12, 0xFF00AA00, 0);
                color = 0xFF00AA00;
            }
            arial12->SetColor(color);
            arial12->Render(1290, 96 + i * 75 + actions[currentMove][i]->getEffectsCount() * 12, HGETEXT_LEFT, "+");

            arial12->SetColor(0xFF000000);

            color = 0xFF000000;
            if (x > 685 && x < 700 && y > 98 + i * 75 && y < 128 + i * 75) {
                color = 0xFFFF0000;
                game->drawRect(700, 98 + i * 75, 1299, 173 + i * 75, color, 0);
            }
            fnt->SetColor(color);
            fnt->Render(685, 98 + i * 75, HGETEXT_LEFT, "-");
        }
        int color = 0xFF000000;
        if (x > 685 && x < 700 && y > 98 + actionsCounts[currentMove] * 75 && y < 128 + actionsCounts[currentMove] * 75) {
            color = 0xFF00AA00;
            game->drawRect(700, 98 + actionsCounts[currentMove] * 75, 1299, 173 + actionsCounts[currentMove] * 75, color, 0);
        }
        fnt->SetColor(color);
        fnt->Render(685, 98 + actionsCounts[currentMove] * 75, HGETEXT_LEFT, "+");
    }

	game->endDraw();
	return false;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    icon = MAKEINTRESOURCE(CE_ICON);

	// Get HGE interface
	HGE * hge = hgeCreate(HGE_VERSION);
	hge->System_SetState(HGE_USESOUND, false);
	//hge->System_SetState(HGE_ZBUFFER, true);

	// Set up log file, frame function, render function and window title
	hge->System_SetState(HGE_LOGFILE, "character_editor.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "SoD character editor");

	hge->System_SetState(HGE_ICON,         icon);

	game = new Game(hge);

	for (int i = 0; i < 256; i++) {
		frameAnimShow[i] = new bool*[256];
		frameAnimHideLayer[i] = new bool*[256];
		frameAnimX[i] = new float*[256];
		frameAnimY[i] = new float*[256];
		frameAnimAngle[i] = new float*[256];
		frameAnimScaleX[i] = new float*[256];
		frameAnimScaleY[i] = new float*[256];
		frameAnimLayer[i] = new int*[256];
		frameHotSpotX[i] = new float*[256];
		frameHotSpotY[i] = new float*[256];
		frameHotSpotAngle[i] = new float*[256];
		frameHotSpotShow[i] = new bool*[256];
		framesLengths[i] = new float[256];
		framesCounts[i] = 1;
		actionsCounts[i] = 0;
		moveNames[i] = "<title>";
		bodyNames[i] = "<body>";

		animRotating[i] = new bool[256];
		hotSpotRotating[i] = new bool[256];

		angleAnimShow[i] = new bool*[256];
		angleAnimX[i] = new float*[256];
		angleAnimY[i] = new float*[256];
		angleAnimAngle[i] = new float*[256];
		angleAnimScaleX[i] = new float*[256];
		angleAnimScaleY[i] = new float*[256];
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
			frameAnimScaleX[i][j] = new float[256];
			frameAnimScaleY[i][j] = new float[256];
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
			angleAnimScaleX[i][j] = new float[256];
			angleAnimScaleY[i][j] = new float[256];
			angles[i][j] = 0;
			angleHotSpotX[i][j] = new float[256];
			angleHotSpotY[i][j] = new float[256];
			angleHotSpotAngle[i][j] = new float[256];
			angleHotSpotShow[i][j] = new bool[256];
			for (int k = 0; k < 256; k++) {
				frameAnimX[i][j][k] = 0;
				frameAnimY[i][j][k] = 0;
				frameAnimAngle[i][j][k] = 0;
				frameAnimScaleX[i][j][k] = 1.0f;
				frameAnimScaleY[i][j][k] = 1.0f;
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
				angleAnimScaleX[i][j][k] = 1.0f;
				angleAnimScaleY[i][j][k] = 1.0f;
				angleHotSpotX[i][j][k] = 0;
				angleHotSpotY[i][j][k] = 0;
				angleHotSpotAngle[i][j][k] = 0;
				angleHotSpotShow[i][j][k] = false;
			}
		}
	}

	actionCauses[ACTIONCAUSE_TYPE_NONE] = "none";
    actionCauses[ACTIONCAUSE_TYPE_KEYHIT] = "key hit";
    actionCauses[ACTIONCAUSE_TYPE_KEYRELEASED] = "key released";
    actionCauses[ACTIONCAUSE_TYPE_KEYDOWN] = "key down";

    actionCauses[ACTIONCAUSE_TYPE_NPC_TARGETSIDE] = "target at";
    actionCauses[ACTIONCAUSE_TYPE_NPC_TARGETFARTHER] = "target farther than";
    actionCauses[ACTIONCAUSE_TYPE_NPC_TARGETCLOSER] = "target closer than";

    actionCauses[ACTIONCAUSE_TYPE_ONGROUND] = "on ground";

    actionCauses[ACTIONCAUSE_TYPE_ANIM_TIME_PASSED] = "time passed";
    actionCauses[ACTIONCAUSE_TYPE_ANIM_TIME_IS] = "time is";

    actionEffects[ACTIONEFFECT_TYPE_TURN] = "turn";
    actionEffects[ACTIONEFFECT_TYPE_CHANGEMOVE] = "set move";
    actionEffects[ACTIONEFFECT_TYPE_RUN] = "run";
    actionEffects[ACTIONEFFECT_TYPE_JUMP] = "jump";
    actionEffects[ACTIONEFFECT_TYPE_SPAWN_EFFECT] = "spawn effect";


    keyNames[CHARACTER_KEY_BACK] = "back";
    keyNames[CHARACTER_KEY_FORTH] = "forth";
    keyNames[CHARACTER_KEY_JUMP] = "jump";
    keyNames[CHARACTER_KEY_ATTACK] = "attack";

	if (game->preload()) {
		//bgTex = game->getHge()->Texture_Load("box.png");

		fnt = new hgeFont("font1.fnt");
		arial12 = new hgeFont("arial12.fnt");

		disabledIcon = game->loadAnimation("disabled_icon.xml");
		resizeIcon = game->loadAnimation("resize_icon.xml");

		game->loadEffectPrototypes("effects.xml");

		grayQuad.v[0].x = 1300; grayQuad.v[0].y = 0; grayQuad.v[0].col = 0xFFAAAAAA; grayQuad.v[0].z = 1;
		grayQuad.v[1].x = 1600; grayQuad.v[1].y = 0; grayQuad.v[1].col = 0xFFAAAAAA; grayQuad.v[1].z = 1;
		grayQuad.v[2].x = 1600; grayQuad.v[2].y = 900; grayQuad.v[2].col = 0xFFAAAAAA; grayQuad.v[2].z = 1;
		grayQuad.v[3].x = 1300; grayQuad.v[3].y = 900; grayQuad.v[3].col = 0xFFAAAAAA; grayQuad.v[3].z = 1;
		grayQuad.tex = 0;//bgTex;
		grayQuad.blend = 2;


		mainWindow = new GUIWindow(game, 1, 1300, 0, 300, 420);
		game->getGUI()->AddCtrl(mainWindow);

		mainWindow->AddCtrl(new hgeGUIMenuItem(2, fnt, 150, 20, "insert anim", insertAnimButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(4, fnt, 120, 60, "save", saveCharacterButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(5, fnt, 180, 60, "load", loadCharacterButtonClick));

		//mainWindow->AddCtrl(new hgeGUIMenuItem(6, fnt, 150, 120, 0.0f, "insert frame", insertFrameButtonClick));
		//mainWindow->AddCtrl(new hgeGUIMenuItem(7, fnt, 130, 145, 0.0f, "<<", prevFrameButtonClick));
		//mainWindow->AddCtrl(new hgeGUIMenuItem(8, fnt, 170, 145, 0.0f, ">>", nextFrameButtonClick));

		mainWindow->AddCtrl(new hgeGUIMenuItem(9, fnt, 130, 165, "-", decreaseLengthButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(10, fnt, 170, 165, "+", increaseLengthButtonClick));

//		mainWindow->AddCtrl(new hgeGUIMenuItem(11, fnt, 150, 200, "insert animation", insertMoveButtonClick));
//		mainWindow->AddCtrl(new hgeGUIMenuItem(12, fnt, 130, 225, "<<", prevMoveButtonClick));
//		mainWindow->AddCtrl(new hgeGUIMenuItem(13, fnt, 170, 225, ">>", nextMoveButtonClick));

		mainWindow->AddCtrl(new hgeGUIMenuItem(14, fnt, 150, 260, "add hotspot", addHotspotButtonClick));

		mainWindow->AddCtrl(new hgeGUIMenuItem(15, fnt, 150, 300, "angles mode", anglesModeButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(16, fnt, 150, 330, "frames mode", framesModeButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(17, fnt, 150, 360, "moves mode", movesModeButtonClick));

		mainWindow->AddCtrl(new hgeGUIMenuItem(18, fnt, 150, 100, "toggle layers visibility", toggleLayersClick));

		hotspotTypesWindow = new GUIWindow(game, 40, 1300, 0, 300, 600);
		game->getGUI()->AddCtrl(hotspotTypesWindow);
		hotspotTypesWindow->AddCtrl(new hgeGUIMenuItem(41, fnt, 150, 20, "box", boxHotspotButtonClick));
		hotspotTypesWindow->Hide();

		for (int i = 0; i < 10; i++) {
			char* s = new char();
			mainWindow->AddCtrl(new hgeGUIMenuItem(20 + i, fnt, 15 + 30 * i, 420, itoa(i + 1, s, 10), setTabButtonClick));
		}



		animContextMenuWindow = new GUIWindow(game, 50, 0, 0, 200, 60);
		game->getGUI()->AddCtrl(animContextMenuWindow);
		hgeGUIEditableLabel* bodyNameInput = new hgeGUIEditableLabel(game, 51, arial12, 2, 2, 196, 17, "");
		bodyNameInput->setOnChange(bodyNameChange);
		animContextMenuWindow->AddCtrl(bodyNameInput);
		animContextMenuWindow->AddCtrl(new hgeGUIMenuItem(52, arial12, 50, 20, "reset", resetBodyClick));
		animContextMenuWindow->AddCtrl(new hgeGUIMenuItem(59, arial12, 100, 40, "ok", closeBodyContextMenuClick));
		animContextMenuWindow->Hide();

		frameContextWindow = new GUIWindow(game, 60, 0, 0, 200, 60);
		game->getGUI()->AddCtrl(frameContextWindow);
		hgeGUIMenuItem* frameLengthLabel = new hgeGUIMenuItem(61, arial12, 25, 2, "length", NULL);
		frameLengthLabel->bEnabled = false;
		frameContextWindow->AddCtrl(frameLengthLabel);
		hgeGUIEditableLabel* frameLengthInput = new hgeGUIEditableLabel(game, 62, arial12, 52, 2, 146, 17, "");
		frameLengthInput->setOnChange(frameLengthChange);
		frameContextWindow->AddCtrl(frameLengthInput);
		frameContextWindow->AddCtrl(new hgeGUIMenuItem(63, arial12, 100, 20, "delete", removeFrameClick));
		frameContextWindow->AddCtrl(new hgeGUIMenuItem(67, arial12, 100, 40, "ok", closeFrameContextMenuClick));
		frameContextWindow->Hide();

		moveContextWindow = new GUIWindow(game, 70, 0, 0, 200, 60);
		game->getGUI()->AddCtrl(moveContextWindow);
		hgeGUIEditableLabel* moveNameInput = new hgeGUIEditableLabel(game, 72, arial12, 2, 2, 196, 17, "");
		moveNameInput->setOnChange(moveNameChange);
		moveContextWindow->AddCtrl(moveNameInput);
		moveContextWindow->AddCtrl(new hgeGUIMenuItem(73, arial12, 50, 20, "delete", removeMoveClick));
		moveContextWindow->AddCtrl(new hgeGUIMenuItem(74, arial12, 150, 20, "duplicate", duplicateMoveClick));
		moveContextWindow->AddCtrl(new hgeGUIMenuItem(77, arial12, 100, 40, "ok", closeMoveContextMenuClick));
		moveContextWindow->Hide();

		hotSpotContextWindow = new GUIWindow(game, 80, 0, 0, 200, 60);
		game->getGUI()->AddCtrl(hotSpotContextWindow);
		hgeGUIEditableLabel* hotSpotNameInput = new hgeGUIEditableLabel(game, 81, arial12, 2, 2, 196, 17, "");
		hotSpotNameInput->setOnChange(hotSpotNameChange);
		hotSpotContextWindow->AddCtrl(hotSpotNameInput);
		hotSpotContextWindow->AddCtrl(new hgeGUIMenuItem(89, arial12, 100, 40, "ok", closeHotSpotContextWindowClick));
		hotSpotContextWindow->Hide();

		movesIndexWindow = new GUIWindow(game, 200, 1300, 0, 300, 900);
		game->getGUI()->AddCtrl(movesIndexWindow);
		movesIndexWindow->AddCtrl(new hgeGUIMenuItem(201, fnt, 150, 20, "frames mode", movesModeButtonClick));
		movesIndexWindow->Hide();

		moveNameLabel = new hgeGUIEditableLabel(game, 221, arial12, 550.0f, 480.0f, 100.0f, 17.0f, "");
		game->getGUI()->AddCtrl(moveNameLabel);
		moveNameLabel->setOnChange(moveNameChange);
		moveNameLabel->Hide();

		actionCauseContextWindow = new GUIWindow(game, 240, 0, 0, 250, 220);
		game->getGUI()->AddCtrl(actionCauseContextWindow);
		actionCauseContextWindow->AddCtrl(new hgeGUIMenuItem(243, arial12, 230, 0, "close", closeActioncauseWindowClick));

		actionCauseTypeWindow = new GUIWindow(game, 250, 0, 20, 125, 200);
		actionCauseContextWindow->AddCtrl(actionCauseTypeWindow);
		actionCauseTypeWindow->AddCtrl(new hgeGUIMenuItem(251, arial12, 62, 5, "none", setActionCauseClick));
		actionCauseTypeWindow->AddCtrl(new hgeGUIMenuItem(252, arial12, 62, 20, "key hit", setActionCauseClick));
		actionCauseTypeWindow->AddCtrl(new hgeGUIMenuItem(253, arial12, 62, 35, "key released", setActionCauseClick));
		actionCauseTypeWindow->AddCtrl(new hgeGUIMenuItem(254, arial12, 62, 50, "key down", setActionCauseClick));
		actionCauseTypeWindow->AddCtrl(new hgeGUIMenuItem(255, arial12, 62, 65, "on ground", setActionCauseClick));
		actionCauseTypeWindow->AddCtrl(new hgeGUIMenuItem(256, arial12, 62, 80, "time passed", setActionCauseClick));
		actionCauseTypeWindow->AddCtrl(new hgeGUIMenuItem(257, arial12, 62, 95, "time is", setActionCauseClick));
		actionCauseTypeWindow->AddCtrl(new hgeGUIMenuItem(258, arial12, 62, 110, "npc: target at", setActionCauseClick));
		actionCauseTypeWindow->AddCtrl(new hgeGUIMenuItem(259, arial12, 62, 125, "npc: target farther", setActionCauseClick));
		actionCauseTypeWindow->AddCtrl(new hgeGUIMenuItem(260, arial12, 62, 140, "npc: target closer", setActionCauseClick));

		actionCauseParamWindowInput = new GUIWindow(game, 300, 125, 20, 125, 30);
		actionCauseContextWindow->AddCtrl(actionCauseParamWindowInput);
		actionCauseParamInput = new hgeGUIEditableLabel(game, 301, arial12, 2, 5, 120, 17, "0");
		actionCauseParamWindowInput->AddCtrl(actionCauseParamInput);
		actionCauseParamInput->setOnChange(actionParamInputChange);

		actionCauseParamWindowSwitch = new GUIWindow(game, 302, 125, 20, 125, 20);
		actionCauseContextWindow->AddCtrl(actionCauseParamWindowSwitch);
		actionCauseParamWindowSwitch->AddCtrl(new hgeGUIMenuItem(303, arial12, 31, 0, "yes", setActionParamSwitchClick));
		actionCauseParamWindowSwitch->AddCtrl(new hgeGUIMenuItem(304, arial12, 94, 0, "no", setActionParamSwitchClick));

		actionCauseParamWindowKey = new GUIWindow(game, 310, 125, 20, 125, 200);
		actionCauseContextWindow->AddCtrl(actionCauseParamWindowKey);
		actionCauseParamWindowKey->AddCtrl(new hgeGUIMenuItem(311, arial12, 62, 5, "forth", setActionParamKeyClick));
		actionCauseParamWindowKey->AddCtrl(new hgeGUIMenuItem(312, arial12, 62, 20, "back", setActionParamKeyClick));
		actionCauseParamWindowKey->AddCtrl(new hgeGUIMenuItem(313, arial12, 62, 35, "attack", setActionParamKeyClick));
		actionCauseParamWindowKey->AddCtrl(new hgeGUIMenuItem(314, arial12, 62, 50, "jump", setActionParamKeyClick));

		actionCauseParamWindowSide = new GUIWindow(game, 330, 125, 20, 125, 20);
		actionCauseContextWindow->AddCtrl(actionCauseParamWindowSide);
		actionCauseParamWindowSide->AddCtrl(new hgeGUIMenuItem(331, arial12, 31, 0, "in front", setActionParamSwitchClick));
		actionCauseParamWindowSide->AddCtrl(new hgeGUIMenuItem(332, arial12, 94, 0, "behind", setActionParamSwitchClick));

		actionCauseContextWindow->Hide();

		actionEffectContextWindow = new GUIWindow(game, 500, 0, 0, 250, 220);
		game->getGUI()->AddCtrl(actionEffectContextWindow);
		actionEffectContextWindow->AddCtrl(new hgeGUIMenuItem(501, arial12, 230, 0, "close ", closeActioneffectWindowClick));

		actionEffectTypeWindow = new GUIWindow(game, 510, 0, 20, 125, 200);
		actionEffectContextWindow->AddCtrl(actionEffectTypeWindow);
		actionEffectTypeWindow->AddCtrl(new hgeGUIMenuItem(511, arial12, 62, 5, "turn", setActionEffectClick));
		actionEffectTypeWindow->AddCtrl(new hgeGUIMenuItem(512, arial12, 62, 20, "change move ", setActionEffectClick));
		actionEffectTypeWindow->AddCtrl(new hgeGUIMenuItem(513, arial12, 62, 35, "run", setActionEffectClick));
		actionEffectTypeWindow->AddCtrl(new hgeGUIMenuItem(514, arial12, 62, 50, "jump", setActionEffectClick));
		actionEffectTypeWindow->AddCtrl(new hgeGUIMenuItem(515, arial12, 62, 65, "spawn effect", setActionEffectClick));

		actionEffectParamWindowInput = new GUIWindow(game, 550, 125, 20, 125, 50);
		actionEffectContextWindow->AddCtrl(actionEffectParamWindowInput);
		actionEffectParamInput = new hgeGUIEditableLabel(game, 551, arial12, 2, 5, 120, 17, "0");
		actionEffectParamWindowInput->AddCtrl(actionEffectParamInput);
		actionEffectParamInput->setOnChange(actionParamInputChange);
		actionEffectParam2Input = new hgeGUIEditableLabel(game, 552, arial12, 2, 25, 120, 17, "0");
		actionEffectParamWindowInput->AddCtrl(actionEffectParam2Input);
		actionEffectParam2Input->setOnChange(actionParamInputChange);

		actionEffectContextWindow->Hide();

		game->loop();

		//delete fnt;

	}

	return 0;
}



//}
