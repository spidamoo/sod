#include <stdio.h>
#include <hge.h>
#include <hgefont.h>
#include <hgeanim.h>
#include <Box2D/Box2D.h>
#include <SoDlib.h>

#include "resource.h"

LPTSTR icon;

Game * game;

bool FrameFunc()
{
//	if (game->getHge()->Input_KeyDown(HGEK_SPACE))
//		game->loadConstruction("test.xml", b2Vec2(2 + game->getHge()->Random_Float(-1, 1), 0));
	return game->update();
}

// This function will be called by HGE when
// the application window should be redrawn.
// Put your rendering code here.
bool RenderFunc()
{
    game->draw();
//	game->startDraw();
//	game->drawGame();
//	game->endDraw();
	return false;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    icon = MAKEINTRESOURCE(_ICON);

	// Get HGE interface
	HGE * hge = hgeCreate(HGE_VERSION);
	hge->System_SetState(HGE_USESOUND, false);

	// Set up log file, frame function, render function and window title
	hge->System_SetState(HGE_LOGFILE, "sod.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "plainlands game");

	hge->System_SetState(HGE_ICON,         icon);

	//b2Vec2 gravity(0.0f, 10.0f);
	//b2World* world = new b2World(gravity);

	game = new Game(hge);
	if (game->preload()) {
		//game->loadConstruction("ground.xml", b2Vec2(10, 5), b2_staticBody);
		game->loadPlayerCharacter("mage.xml", b2Vec2(2, 1));
		game->loadNonPlayerCharacter("rat.xml", b2Vec2(10, 1));
		//game->loadNonPlayerCharacter("stone_ant.xml", b2Vec2(8, 0));
		//game->addGroundLine(new GroundLine(game, -1, 2, 20, 3));
		game->loadMap("map.xml");
		game->loadEffectPrototypes("effects.xml");
		game->loadConditionPrototypes("conditions.xml");
		game->moveCamera(b2Vec2(0, -4.5));
		game->loop();
	}

	return 0;
}
