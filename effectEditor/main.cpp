#include <stdio.h>
#include <hge.h>
#include <hgefont.h>
#include <hgeanim.h>
#include <Box2D/Box2D.h>
#include <SoDlib.h>
#include <windows.h>
#include <list>

Game* game;

hgeFont* fnt;
hgeFont* arial12;

GUIWindow* mainWindow;
GUIWindow* effectDetailsWindow;
hgeGUIEditableLabel* effectNameLabel;

int selectedEffect = 0;

const char* effectTypes[10] = {};
const char* conditions[10] = {};

void setSelectedEffect(int selected) {
    selectedEffect = selected;
    effectNameLabel->setTitle( game->getEffectPrototype(selectedEffect)->getName() );

    for ( int i = 0; i < EFFECT_FUNCTIONS_COUNT; i++ ) {
        if (game->getEffectPrototype(selectedEffect)->getExpressionExists(i)) {
            ((hgeGUIEditableLabel*)game->getGUI()->GetCtrl(41 + i))->setTitle(game->getEffectPrototype(selectedEffect)->getFunction(i));
        }
        else {
            ((hgeGUIEditableLabel*)game->getGUI()->GetCtrl(41 + i))->setTitle("");
        }
        ((hgeGUIEditableLabel*)game->getGUI()->GetCtrl(61 + i))->setTitle(game->getEffectPrototype(selectedEffect)->getStartFunction(i));
    }
}

bool effectNameChange(hgeGUIObject* sender) {
    game->getEffectPrototype(selectedEffect)->setName( ((hgeGUIEditableLabel*)sender)->getTitle() );
}
bool functionChange(hgeGUIObject* sender) {
    if (((hgeGUIEditableLabel*)sender)->getTitleLen() ) {
        game->getEffectPrototype(selectedEffect)->setFunction(sender->id - 41, ((hgeGUIEditableLabel*)sender)->getTitle());
        game->getEffectPrototype(selectedEffect)->setExpressionExists(sender->id - 41, true);
    }
    else {
        game->getEffectPrototype(selectedEffect)->setFunction(sender->id - 41, "0");
        game->getEffectPrototype(selectedEffect)->setExpressionExists(sender->id - 41, false);
    }
}
bool startFunctionChange(hgeGUIObject* sender) {
    game->getEffectPrototype(selectedEffect)->setStartFunction(sender->id - 61, ((hgeGUIEditableLabel*)sender)->getTitle());
}

bool testEffectClick(hgeGUIObject* sender) {
    Effect* newObject = new Effect(game, game->getEffectPrototype(selectedEffect));
    newObject->setPosition(b2Vec2_zero);
    newObject->setAnimation(
        game->loadAnimation(
            (char*)game->getEffectPrototype(selectedEffect)->getAnimation(
                game->getHge()->Random_Int(1, game->getEffectPrototype(selectedEffect)->getAnimationsCount()) - 1
            )
        ),
        game->getEffectPrototype(selectedEffect)->getBlendMode()
    );
    game->addEffect( newObject );
}

void save(const char* filename) {
    TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

	TiXmlElement* root = new TiXmlElement( "effect_prototypes" );
	root->SetAttribute("count", game->getEffectPrototypesCount());

	for (int i = 0; i < game->getEffectPrototypesCount(); i++) {
        TiXmlElement* element = new TiXmlElement( "prototype" );
        game->getEffectPrototype(i)->saveToXml(element);
        root->LinkEndChild(element);
    }
    doc.LinkEndChild( root );
	doc.SaveFile(filename);
}
bool saveButtonClick(hgeGUIObject* sender) {
    save("effects.xml");
}
bool loadButtonClick(hgeGUIObject* sender) {
    game->loadEffectPrototypes("effects.xml");
}

bool FrameFunc() {
	float x, y;
	game->getHge()->Input_GetMousePos(&x, &y);
	float worldX = game->worldX(x);
	float worldY = game->worldY(y);

	for (int i = 0; i < game->getEffectPrototypesCount(); i++) {
        if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && x > 1 && x < 195 && y > 1 + i * 16 && y < i * 16 + 17) {
            setSelectedEffect(i);
        }
    }


	return game->update(true);
}

bool RenderFunc() {
	game->startDraw();

	float x, y;
	game->getHge()->Input_GetMousePos(&x, &y);

    game->drawGame();

    game->drawRect(0, 0, 199, 900, 0, 0xFFFFFFFF);
    for (int i = 0; i < game->getEffectPrototypesCount(); i++) {
        DWORD color = 0xFF000000;
        if (x > 1 && x < 195 && y > 1 + i * 16 && y < i * 16 + 17) {
            color = 0xFF0000FF;
        }
        if (i == selectedEffect) {
            color = 0xFF0000FF;
            game->drawRect(1, 1 + i * 16, 199, i * 16 + 17, color, 0);
        }
        arial12->SetColor(color);
        arial12->printf( 1, 1 + i * 16, HGETEXT_LEFT, "%d: %s", i, game->getEffectPrototype(i)->getName() );
    }

    game->drawRect(1080, 0, 1600, 500, 0, 0xFFFFFFFF);
    fnt->SetColor(0xFF000000);
    fnt->Render(1100, 12, HGETEXT_LEFT, "Actions");
	for (int i = 0; i < game->getEffectPrototype(selectedEffect)->getActionsCount(); i++) {
        char text[50];
        switch (game->getEffectPrototype(selectedEffect)->getAction(i)->getType()) {
            case EFFECTACTION_TYPE_SPAWN_EFFECT:
                sprintf(
                    text, "spawn effect #%d: %s",
                    game->getEffectPrototype(selectedEffect)->getAction(i)->getParam(),
                    game->getEffectPrototype(game->getEffectPrototype(selectedEffect)->getAction(i)->getParam())->getName()
                );
                break;
            case EFFECTACTION_TYPE_DESTRUCT:
                sprintf(
                    text, "destruct"
                );
                break;
            default:
                sprintf(
                    text, "%s %d",
                    effectTypes[game->getEffectPrototype(selectedEffect)->getAction(i)->getType()],
                    game->getEffectPrototype(selectedEffect)->getAction(i)->getParam()
                );
                break;
        }
        DWORD color = 0xFF000000;
        if (x > 1100 && x < 1300 && y > 46 + i * 12 && y < 58 + i * 12) {
            color = 0xFF0000FF;
        }
        arial12->SetColor(color);
        arial12->Render(1100, 42 + i * 12, HGETEXT_LEFT, text);
	}

	game->getGUI()->Render();

	game->getHge()->Gfx_EndScene();

	return false;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// Get HGE interface
	HGE * hge = hgeCreate(HGE_VERSION);
	hge->System_SetState(HGE_USESOUND, false);
	//hge->System_SetState(HGE_ZBUFFER, true);

	// Set up log file, frame function, render function and window title
	hge->System_SetState(HGE_LOGFILE, "effects_editor.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "SoD effects editor");

	game = new Game(hge);

	effectTypes[EFFECTACTION_TYPE_SPAWN_EFFECT] = "spawn effect";
	effectTypes[EFFECTACTION_TYPE_DESTRUCT] = "destruct";
	effectTypes[EFFECTACTION_TYPE_INFLICT_CONDITION] = "inflict condition";

	conditions[EFFECTACTION_CONDITION_NONE] = "none";
	conditions[EFFECTACTION_CONDITION_CROSS_GROUND] = "cross ground";
	conditions[EFFECTACTION_CONDITION_CROSS_ENEMY_BODY] = "cross enemy";
	conditions[EFFECTACTION_CONDITION_CROSS_FRIEND_BODY] = "cross friend";

	if (game->preload()) {
		fnt = new hgeFont("font1.fnt");
		arial12 = new hgeFont("arial12.fnt");

		game->loadEffectPrototypes("effects.xml");

		effectDetailsWindow = new GUIWindow(game, 1, 200, 0, 880, 500);
		game->getGUI()->AddCtrl(effectDetailsWindow);
		effectNameLabel = new hgeGUIEditableLabel(game, 2, arial12, 60.0f, 10.0f, 200.0f, 19.0f, "");
		effectDetailsWindow->AddCtrl(effectNameLabel);
		effectNameLabel->setOnChange(effectNameChange);

		mainWindow = new GUIWindow(game, 500, 1300, 500, 300, 400);
		effectDetailsWindow->AddCtrl(mainWindow);
		mainWindow->AddCtrl(new hgeGUIMenuItem(501, fnt, 120, 60, "save", saveButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(502, fnt, 180, 60, "load", loadButtonClick));

		hgeGUIMenuItem* functionsLabel = new hgeGUIMenuItem(11, arial12, 260.0f, 45.0f, "function", NULL);
        functionsLabel->bEnabled = false;
        effectDetailsWindow->AddCtrl(functionsLabel);

        hgeGUIMenuItem* startFunctionsLabel = new hgeGUIMenuItem(12, arial12, 665.0f, 45.0f, "start function", NULL);
        startFunctionsLabel->bEnabled = false;
        effectDetailsWindow->AddCtrl(startFunctionsLabel);

		for ( int i = 0; i < EFFECT_FUNCTIONS_COUNT; i++ ) {
            hgeGUIMenuItem* titleLabel = new hgeGUIMenuItem(21 + i, arial12, 30.0f, 65.0f + 25.0f * i, (char*)EFFECT_PARAM_NAMES[i], NULL);
            titleLabel->bEnabled = false;
            effectDetailsWindow->AddCtrl(titleLabel);

		    hgeGUIEditableLabel* functionInput = new hgeGUIEditableLabel(game, 41 + i, arial12, 60.0f, 65.0f + 25.0f * i, 400.0f, 19.0f, "");
            functionInput->setOnChange(functionChange);
            effectDetailsWindow->AddCtrl(functionInput);

            hgeGUIEditableLabel* startFunctionInput = new hgeGUIEditableLabel(game, 61 + i, arial12, 465.0f, 65.0f + 25.0f * i, 400.0f, 19.0f, "");
            startFunctionInput->setOnChange(startFunctionChange);
            effectDetailsWindow->AddCtrl(startFunctionInput);
		}

        effectDetailsWindow->AddCtrl(new hgeGUIMenuItem(81, fnt, 60.0f, 470.0f, "test", testEffectClick));

		setSelectedEffect(0);

//		game->moveCamera(b2Vec2(-17, -12));

        game->moveScreen(b2Vec2(-750.0f, - 700.0f));

		game->loop();
	}

	return 0;
}


