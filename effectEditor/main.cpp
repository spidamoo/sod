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

Game* game;

hgeFont* fnt;
hgeFont* arial12;

GUIWindow* mainWindow;
GUIWindow* effectDetailsWindow;
hgeGUIEditableLabel* effectNameLabel;
GUIWindow* actionDetailsWindow;

int selectedEffect = 0;
int selectedAction = 0;

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

    game->getGUI()->GetCtrl(83)->selected = false;
    game->getGUI()->GetCtrl(84)->selected = false;
    game->getGUI()->GetCtrl(85)->selected = false;

    game->getGUI()->GetCtrl(82 + game->getEffectPrototype(selectedEffect)->getPositionType())->selected = true;

    game->getGUI()->GetCtrl(87)->selected = false;
    game->getGUI()->GetCtrl(88)->selected = false;
    game->getGUI()->GetCtrl(89)->selected = false;

    game->getGUI()->GetCtrl(86 + game->getEffectPrototype(selectedEffect)->getAreaType())->selected = true;

    game->getGUI()->GetCtrl(91)->selected = game->getEffectPrototype(selectedEffect)->getBlendMode() & BLEND_COLORADD;
    game->getGUI()->GetCtrl(92)->selected = game->getEffectPrototype(selectedEffect)->getBlendMode() & BLEND_ALPHABLEND;
    game->getGUI()->GetCtrl(93)->selected = game->getEffectPrototype(selectedEffect)->getBlendMode() & BLEND_ZWRITE;

    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(95) )->setTitle(game->getEffectPrototype(selectedEffect)->getHotSpotName());

    game->getGUI()->GetCtrl(96)->selected = game->getEffectPrototype(selectedEffect)->getList() == EFFECT_LIST_NORMAL;
    game->getGUI()->GetCtrl(97)->selected = game->getEffectPrototype(selectedEffect)->getList() == EFFECT_LIST_BACKGROUND;
}
void setSelectedAction(int selected) {
    selectedAction = selected;

    game->getGUI()->GetCtrl(191)->selected = false;
    game->getGUI()->GetCtrl(192)->selected = false;
    game->getGUI()->GetCtrl(193)->selected = false;

    game->getGUI()->GetCtrl(190 + game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->getType())->selected = true;

    char buffer[20];
    sprintf( buffer, "%d", game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->getParam() );
    ( (hgeGUIEditableLabel*)(game->getGUI()->GetCtrl(194)) )->setTitle(buffer);

    sprintf( buffer, "%.4f", game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->getStartTime() );
    ( (hgeGUIEditableLabel*)(game->getGUI()->GetCtrl(196)) )->setTitle(buffer);

    sprintf( buffer, "%.4f", game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->getInterval() );
    ( (hgeGUIEditableLabel*)(game->getGUI()->GetCtrl(198)) )->setTitle(buffer);

    game->getGUI()->GetCtrl(200)->selected = false;
    game->getGUI()->GetCtrl(201)->selected = false;
    game->getGUI()->GetCtrl(202)->selected = false;
    game->getGUI()->GetCtrl(203)->selected = false;

    game->getGUI()->GetCtrl(200 + game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->getCondition())->selected = true;

    game->getGUI()->GetCtrl(205)->selected = game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->getTargets() & EFFECTACTION_TARGET_FRIEND;
    game->getGUI()->GetCtrl(206)->selected = game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->getTargets() & EFFECTACTION_TARGET_ENEMY;

    sprintf( buffer, "%d", game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->getMaxInteractions() );
    ( (hgeGUIEditableLabel*)(game->getGUI()->GetCtrl(208)) )->setTitle(buffer);

    actionDetailsWindow->Show();
    actionDetailsWindow->Move(1090.0f, 58 + 12 * selected);
}

void addEffect() {
    game->addEffectPrototype();
    setSelectedEffect( game->getEffectPrototypesCount() - 1 );
}
void removeEffect(int index) {
    game->removeEffectPrototype(index);
    if ( selectedEffect >= game->getEffectPrototypesCount() ) {
        setSelectedEffect( game->getEffectPrototypesCount() - 1 );
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
bool actionTypeClick(hgeGUIObject* sender) {
    game->getGUI()->GetCtrl(191)->selected = false;
    game->getGUI()->GetCtrl(192)->selected = false;
    game->getGUI()->GetCtrl(193)->selected = false;

    sender->selected = true;
    game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->setType(sender->id - 190);
}
bool actionParamChange(hgeGUIObject* sender) {
    game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->setParam( atoi( ( (hgeGUIEditableLabel*)(sender) )->getTitle() ) );
}
bool actionConditionClick(hgeGUIObject* sender) {
    game->getGUI()->GetCtrl(200)->selected = false;
    game->getGUI()->GetCtrl(201)->selected = false;
    game->getGUI()->GetCtrl(202)->selected = false;
    game->getGUI()->GetCtrl(203)->selected = false;

    sender->selected = true;
    game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->setCondition(sender->id - 200);
}
bool actionTargetClick(hgeGUIObject* sender) {
    sender->selected = !sender->selected;
    game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->setTarget(sender->id - 204, sender->selected);
}
bool actionMaxInteractionsChange(hgeGUIObject* sender) {
    game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->setMaxInteractions( atoi( ( (hgeGUIEditableLabel*)(sender) )->getTitle() ) );
}
bool actionDetailsCloseClick(hgeGUIObject* sender) {
    actionDetailsWindow->Hide();
}
bool actionStartTimeChange(hgeGUIObject* sender) {
    game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->setStartTime( atof( ( (hgeGUIEditableLabel*)(sender) )->getTitle() ) );
}
bool actionIntervalChange(hgeGUIObject* sender) {
    game->getEffectPrototype(selectedEffect)->getAction(selectedAction)->setInterval( atof( ( (hgeGUIEditableLabel*)(sender) )->getTitle() ) );
}

bool effectPositionTypeClick(hgeGUIObject* sender) {
    game->getGUI()->GetCtrl(83)->selected = false;
    game->getGUI()->GetCtrl(84)->selected = false;
    game->getGUI()->GetCtrl(85)->selected = false;

    sender->selected = true;
    game->getEffectPrototype(selectedEffect)->setPositionType(sender->id - 82);
}
bool effectAreaTypeClick(hgeGUIObject* sender) {
    game->getGUI()->GetCtrl(87)->selected = false;
    game->getGUI()->GetCtrl(88)->selected = false;
    game->getGUI()->GetCtrl(89)->selected = false;

    sender->selected = true;
    game->getEffectPrototype(selectedEffect)->setAreaType(sender->id - 86);
}
bool effectBlendModeClick(hgeGUIObject* sender) {
    int mode = ( 1 << (sender->id - 91) );
    if (!sender->selected) {
        game->getEffectPrototype(selectedEffect)->setBlendMode( game->getEffectPrototype(selectedEffect)->getBlendMode() | mode);
    }
    else {
        game->getEffectPrototype(selectedEffect)->setBlendMode( game->getEffectPrototype(selectedEffect)->getBlendMode() & ~mode);
    }
    sender->selected = !sender->selected;
}
bool hotSpotNameChange(hgeGUIObject* sender) {
    game->getEffectPrototype(selectedEffect)->setHotSpotName( ( (hgeGUIEditableLabel*)(sender) )->getTitle() );
}
bool effectListClick(hgeGUIObject* sender) {
    game->getGUI()->GetCtrl(96)->selected = false;
    game->getGUI()->GetCtrl(97)->selected = false;
    game->getEffectPrototype(selectedEffect)->setList( sender->id - 96);
    sender->selected = true;
}

bool testEffectClick(hgeGUIObject* sender) {
    Effect* newObject = new Effect(game, game->getEffectPrototype(selectedEffect));
    newObject->setPosition(b2Vec2_zero);
    game->addEffect( newObject, game->getEffectPrototype(selectedEffect)->getList() );
    printf("foo");
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
    setSelectedEffect(0);
}

bool FrameFunc() {
	float x, y;
	game->getHge()->Input_GetMousePos(&x, &y);
	float worldX = game->worldX(x);
	float worldY = game->worldY(y);

    if (!actionDetailsWindow->bVisible && game->getHge()->Input_KeyDown(HGEK_LBUTTON) && x > 190 && x < 200
        && y > 1 + game->getEffectPrototypesCount() * 16 && y < game->getEffectPrototypesCount() * 16 + 17
    ) {
        addEffect();
    }
    else {
        for (int i = 0; i < game->getEffectPrototypesCount(); i++) {
            if (!actionDetailsWindow->bVisible && game->getHge()->Input_KeyDown(HGEK_LBUTTON) && y > 1 + i * 16 && y < i * 16 + 17) {
                if (x > 1 && x < 190) {
                    setSelectedEffect(i);
                }
                else if (x > 190 && x < 200) {
                    removeEffect(i);
                }
            }
        }
    }

    if (!actionDetailsWindow->bVisible && game->getHge()->Input_KeyDown(HGEK_LBUTTON) && x > 1290 && x < 1300
        && y > 46 + game->getEffectPrototype(selectedEffect)->getActionsCount() * 12 && y < 58 + game->getEffectPrototype(selectedEffect)->getActionsCount() * 12
    ) {
        game->getEffectPrototype(selectedEffect)->addAction();
    }
    else {
        for (int i = 0; i < game->getEffectPrototype(selectedEffect)->getActionsCount(); i++) {
            if (!actionDetailsWindow->bVisible && game->getHge()->Input_KeyDown(HGEK_LBUTTON) && x > 1100 && x < 1290 && y > 46 + i * 12 && y < 58 + i * 12) {
                setSelectedAction(i);
            }
            else if (!actionDetailsWindow->bVisible && game->getHge()->Input_KeyDown(HGEK_LBUTTON) && x > 1290 && x < 1300 && y > 46 + i * 12 && y < 58 + i * 12) {
                game->getEffectPrototype(selectedEffect)->removeAction(i);
            }
        }
    }

	int i = 0;
	for (i = 0; i < game->getEffectPrototype(selectedEffect)->getAnimationsCount(); i++) {
        if (!actionDetailsWindow->bVisible && game->getHge()->Input_KeyDown(HGEK_LBUTTON) && x > 1280 && x < 1300 && y > 296 + i * 12 && y < 308 + i * 12) {
            game->getEffectPrototype(selectedEffect)->removeAnimation(i);
        }
	}
    if (!actionDetailsWindow->bVisible && game->getHge()->Input_KeyDown(HGEK_LBUTTON) && x > 1280 && x < 1300 && y > 296 + i * 12 && y < 308 + i * 12) {
        OPENFILENAME ofn;
        char szFile[512]="\0";
        char szTemp[512];

        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = game->getHge()->System_GetState(HGE_HWND);
        ofn.lpstrFilter = "XML file\0*.xml\0All Files\0*.*\0\0";
        ofn.lpstrFile= szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrDefExt="xml";
        ofn.Flags = ofn.Flags | OFN_NOCHANGEDIR;
        if ( GetOpenFileName(&ofn) ) {
            game->getEffectPrototype(selectedEffect)->addAnimation(szFile);
        }

    }

	return game->update(true);
}

bool RenderFunc() {
	game->startDraw();

	float x, y;
	game->getHge()->Input_GetMousePos(&x, &y);

    game->drawGame();

    game->drawRect(0, 0, 200, 900, 0xFF000000, 0xFFFFFFFF);
    for (int i = 0; i < game->getEffectPrototypesCount(); i++) {
        DWORD color = 0xFF000000;
        if (x > 1 && x < 190 && y > 1 + i * 16 && y < i * 16 + 17) {
            color = 0xFF0000FF;
        }
        if (i == selectedEffect) {
            color = 0xFF0000FF;
            game->drawRect(1, 1 + i * 16, 199, i * 16 + 17, color, 0);
        }
        if (x > 190 && x < 200 && y > 1 + i * 16 && y < i * 16 + 17) {
            color = 0xFFFF0000;
            game->drawRect(1, 1 + i * 16, 199, i * 16 + 17, color, 0);
        }
        arial12->SetColor(color);
        arial12->printf( 1, 1 + i * 16, HGETEXT_LEFT, "%d: %s", i, game->getEffectPrototype(i)->getName() );
        arial12->printf( 191, 1 + i * 16, HGETEXT_LEFT, "-" );
    }

    DWORD color = 0xFF000000;
    if (x > 190 && x < 200 && y > 1 + game->getEffectPrototypesCount() * 16 && y < game->getEffectPrototypesCount() * 16 + 17) {
        color = 0xFF00AA00;
        game->drawRect(1, 1 + game->getEffectPrototypesCount() * 16, 199, game->getEffectPrototypesCount() * 16 + 17, color, 0);
    }
    arial12->SetColor(color);
    arial12->printf( 190, 1 + game->getEffectPrototypesCount() * 16, HGETEXT_LEFT, "+" );

    game->drawRect(1080, 0, 1300, 500, 0xFF000000, 0xFFFFFFFF);
    fnt->SetColor(0xFF000000);
    fnt->Render(1090, 12, HGETEXT_LEFT, "Actions");
	for (int i = 0; i < game->getEffectPrototype(selectedEffect)->getActionsCount(); i++) {
        char text[50];
        switch (game->getEffectPrototype(selectedEffect)->getAction(i)->getType()) {
            case EFFECTACTION_TYPE_SPAWN_EFFECT:
                if ( game->getEffectPrototype( game->getEffectPrototype(selectedEffect)->getAction(i)->getParam() ) ) {
                    sprintf(
                        text, "spawn effect #%d: %s",
                        game->getEffectPrototype(selectedEffect)->getAction(i)->getParam(),
                        game->getEffectPrototype(game->getEffectPrototype(selectedEffect)->getAction(i)->getParam())->getName()
                    );
                }
                else {
                    sprintf(text, "WRONG PARAM!");
                }
                break;
            case EFFECTACTION_TYPE_DESTRUCT:
                sprintf(
                    text, "destruct"
                );
                break;
            case EFFECTACTION_TYPE_INFLICT_CONDITION:
                if ( game->getConditionPrototype( game->getEffectPrototype(selectedEffect)->getAction(i)->getParam() ) ) {
                    sprintf(
                        text, "inflict #%d: %s",
                        game->getEffectPrototype(selectedEffect)->getAction(i)->getParam(),
                        game->getConditionPrototype(game->getEffectPrototype(selectedEffect)->getAction(i)->getParam())->getName()
                    );
                }
                else {
                    sprintf(text, "WRONG PARAM!");
                }
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
        if (!actionDetailsWindow->bVisible && x > 1090 && x < 1290 && y > 46 + i * 12 && y < 58 + i * 12) {
            color = 0xFF0000FF;
        }
        if (actionDetailsWindow->bVisible && i == selectedAction && actionDetailsWindow->bVisible) {
            color = 0xFF0000FF;
            game->drawRect(1090, 46 + i * 12, 1290, 58 + i * 12, color, 0);
        }
        if (!actionDetailsWindow->bVisible && x > 1290 && x < 1300 && y > 46 + i * 12 && y < 58 + i * 12) {
            color = 0xFFFF0000;
        }
        arial12->SetColor(color);
        arial12->Render(1090, 42 + i * 12, HGETEXT_LEFT, text);
        arial12->Render(1291, 42 + i * 12, HGETEXT_LEFT, "-");
	}
	color = 0xFF000000;
	if (!actionDetailsWindow->bVisible && x > 1290 && x < 1300
        && y > 46 + game->getEffectPrototype(selectedEffect)->getActionsCount() * 12 && y < 58 + game->getEffectPrototype(selectedEffect)->getActionsCount() * 12
    ) {
        color = 0xFF00AA00;
    }
    arial12->SetColor(color);
    arial12->Render(1290, 42 + game->getEffectPrototype(selectedEffect)->getActionsCount() * 12, HGETEXT_LEFT, "+");

	fnt->SetColor(0xFF000000);
    fnt->Render(1090, 262, HGETEXT_LEFT, "Animations");
    int i = 0;
	for (i = 0; i < game->getEffectPrototype(selectedEffect)->getAnimationsCount(); i++) {
        DWORD color = 0xFF000000;
        if (x > 1280 && x < 1300 && y > 296 + i * 12 && y < 308 + i * 12) {
            color = 0xFFFF0000;
        }
        arial12->SetColor(color);
        arial12->Render(1280, 292 + i * 12, HGETEXT_RIGHT, game->getEffectPrototype(selectedEffect)->getAnimation(i));
        arial12->Render(1291, 292 + i * 12, HGETEXT_CENTER, "-");
	}
	color = 0xFF000000;
    if (x > 1280 && x < 1300 && y > 296 + i * 12 && y < 308 + i * 12) {
        color = 0xFF00AA00;
    }
    arial12->SetColor(color);
    arial12->Render(1290, 292 + i * 12, HGETEXT_CENTER, "+");

	game->getGUI()->Render();

	game->getHge()->Gfx_EndScene();

	return false;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    icon = MAKEINTRESOURCE(EE_ICON);
	// Get HGE interface
	HGE * hge = hgeCreate(HGE_VERSION);
	hge->System_SetState(HGE_USESOUND, false);
	//hge->System_SetState(HGE_ZBUFFER, true);

	// Set up log file, frame function, render function and window title
	hge->System_SetState(HGE_LOGFILE, "effects_editor.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "SoD effects editor");

	hge->System_SetState(HGE_ICON,         icon);

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

		game->loadCharacterParamPrototypes("character_params.xml");
        game->loadCharacterResourcePrototypes("character_resources.xml");
        game->loadCharacterStatusPrototypes("character_statuses.xml");
        game->loadCharacterMoveTypes("move_types.xml");
		game->loadEffectPrototypes("effects.xml");
		game->loadConditionPrototypes("conditions.xml");

		effectDetailsWindow = new GUIWindow(game, 1, 200, 0, 880, 550);
		game->getGUI()->AddCtrl(effectDetailsWindow);
		effectNameLabel = new hgeGUIEditableLabel(game, 2, arial12, 60.0f, 10.0f, 200.0f, 19.0f, "");
		effectDetailsWindow->AddCtrl(effectNameLabel);
		effectNameLabel->setOnChange(effectNameChange);

		mainWindow = new GUIWindow(game, 500, 1300, 0, 300, 500);
		game->getGUI()->AddCtrl(mainWindow);
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

        effectDetailsWindow->AddCtrl( new hgeGUIMenuItem(81, fnt, 60.0f, 520.0f, "test", testEffectClick) );

        hgeGUIMenuItem* positionTypeLabel = new hgeGUIMenuItem(82, arial12, 50.0f, 470.0f, "position:", NULL);
        positionTypeLabel->bEnabled = false;
        effectDetailsWindow->AddCtrl( positionTypeLabel );

        effectDetailsWindow->AddCtrl( new hgeGUIMenuItem(83, arial12, 100.0f, 470.0f, "static", effectPositionTypeClick) );
        effectDetailsWindow->AddCtrl( new hgeGUIMenuItem(84, arial12, 150.0f, 470.0f, "hotspot", effectPositionTypeClick) );
        effectDetailsWindow->AddCtrl( new hgeGUIMenuItem(85, arial12, 200.0f, 470.0f, "dynamic", effectPositionTypeClick) );

        hgeGUIMenuItem* areaTypeLabel = new hgeGUIMenuItem(86, arial12, 50.0f, 490.0f, "area:", NULL);
        areaTypeLabel->bEnabled = false;
        effectDetailsWindow->AddCtrl( areaTypeLabel );

        effectDetailsWindow->AddCtrl( new hgeGUIMenuItem(87, arial12, 100.0f, 490.0f, "point", effectAreaTypeClick) );
        effectDetailsWindow->AddCtrl( new hgeGUIMenuItem(88, arial12, 150.0f, 490.0f, "rect", effectAreaTypeClick) );
        effectDetailsWindow->AddCtrl( new hgeGUIMenuItem(89, arial12, 200.0f, 490.0f, "circle", effectAreaTypeClick) );

        hgeGUIMenuItem* blendModeLabel = new hgeGUIMenuItem(90, arial12, 300.0f, 470.0f, "blend mode:", NULL);
        blendModeLabel->bEnabled = false;
        effectDetailsWindow->AddCtrl( blendModeLabel );

        effectDetailsWindow->AddCtrl( new hgeGUIMenuItem(91, arial12, 380.0f, 470.0f, "coloradd", effectBlendModeClick) );
        effectDetailsWindow->AddCtrl( new hgeGUIMenuItem(92, arial12, 460.0f, 470.0f, "alphablend", effectBlendModeClick) );
        effectDetailsWindow->AddCtrl( new hgeGUIMenuItem(93, arial12, 540.0f, 470.0f, "zwrite", effectBlendModeClick) );

        hgeGUIMenuItem* hotSpotNameLabel = new hgeGUIMenuItem(94, arial12, 300.0f, 490.0f, "hotspot:", NULL);
        hotSpotNameLabel->bEnabled = false;
        effectDetailsWindow->AddCtrl( hotSpotNameLabel );

        hgeGUIEditableLabel* hotSpotNameInput = new hgeGUIEditableLabel(game, 95, arial12, 350.0f, 490.0f, 100.0f, 19.0f, "");
        hotSpotNameInput->setOnChange(hotSpotNameChange);
        effectDetailsWindow->AddCtrl( hotSpotNameInput );

        effectDetailsWindow->AddCtrl( new hgeGUIMenuItem(96, arial12, 560.0f, 490.0f, "normal", effectListClick) );
        effectDetailsWindow->AddCtrl( new hgeGUIMenuItem(97, arial12, 640.0f, 490.0f, "background", effectListClick) );

        actionDetailsWindow = new GUIWindow(game, 190, 1100, 50, 200.0f, 180.0f);
        game->getGUI()->AddCtrl(actionDetailsWindow);
        actionDetailsWindow->AddCtrl( new hgeGUIMenuItem(191, arial12, 50, 5, "spawn", actionTypeClick) );
        actionDetailsWindow->AddCtrl( new hgeGUIMenuItem(192, arial12, 100, 5, "destruct", actionTypeClick) );
        actionDetailsWindow->AddCtrl( new hgeGUIMenuItem(193, arial12, 150, 5, "inflict", actionTypeClick) );

        hgeGUIEditableLabel* actionParamInput = new hgeGUIEditableLabel(game, 194, arial12, 10.0f, 25.0f, 180.0f, 19.0f, "");
        actionParamInput->setOnChange(actionParamChange);
        actionDetailsWindow->AddCtrl( actionParamInput );

        hgeGUIMenuItem* startTimeLabel = new hgeGUIMenuItem(195, arial12, 15.0f, 50.0f, "st", NULL);
        startTimeLabel->bEnabled = false;
        actionDetailsWindow->AddCtrl( startTimeLabel );

        hgeGUIEditableLabel* startTimeInput = new hgeGUIEditableLabel(game, 196, arial12, 25.0f, 50.0f, 70.0f, 19.0f, "");
        startTimeInput->setOnChange(actionStartTimeChange);
        actionDetailsWindow->AddCtrl( startTimeInput );

        hgeGUIMenuItem* intervalLabel = new hgeGUIMenuItem(197, arial12, 105.0f, 50.0f, "i", NULL);
        intervalLabel->bEnabled = false;
        actionDetailsWindow->AddCtrl( intervalLabel );

        hgeGUIEditableLabel* actionIntervalInput = new hgeGUIEditableLabel(game, 198, arial12, 120.0f, 50.0f, 70.0f, 19.0f, "");
        actionIntervalInput->setOnChange(actionIntervalChange);
        actionDetailsWindow->AddCtrl( actionIntervalInput );

        hgeGUIMenuItem* conditionLabel = new hgeGUIMenuItem(199, arial12, 100.0f, 75.0f, "condition:", NULL);
        conditionLabel->bEnabled = false;
        actionDetailsWindow->AddCtrl( conditionLabel );
        actionDetailsWindow->AddCtrl( new hgeGUIMenuItem(200, arial12, 25.0f, 90.0f, "none", actionConditionClick) );
        actionDetailsWindow->AddCtrl( new hgeGUIMenuItem(201, arial12, 75.0f, 90.0f, "ground", actionConditionClick) );
        actionDetailsWindow->AddCtrl( new hgeGUIMenuItem(202, arial12, 125.0f, 90.0f, "enemy", actionConditionClick) );
        actionDetailsWindow->AddCtrl( new hgeGUIMenuItem(203, arial12, 175.0f, 90.0f, "friend", actionConditionClick) );

        hgeGUIMenuItem* targetsLabel = new hgeGUIMenuItem(204, arial12, 30.0f, 110.0f, "targets:", NULL);
        targetsLabel->bEnabled = false;
        actionDetailsWindow->AddCtrl( targetsLabel );

        actionDetailsWindow->AddCtrl( new hgeGUIMenuItem(205, arial12, 100.0f, 110.0f, "friend", actionTargetClick) );
        actionDetailsWindow->AddCtrl( new hgeGUIMenuItem(206, arial12, 150.0f, 110.0f, "enemy", actionTargetClick) );

        hgeGUIMenuItem* maxInteractionsLabel = new hgeGUIMenuItem(207, arial12, 55.0f, 130.0f, "max interactions", NULL);
        maxInteractionsLabel->bEnabled = false;
        actionDetailsWindow->AddCtrl( maxInteractionsLabel );

        hgeGUIEditableLabel* maxInteractionsInput = new hgeGUIEditableLabel(game, 208, arial12, 105.0f, 130.0f, 85.0f, 19.0f, "");
        maxInteractionsInput->setOnChange(actionMaxInteractionsChange);
        actionDetailsWindow->AddCtrl( maxInteractionsInput );

        actionDetailsWindow->AddCtrl( new hgeGUIMenuItem(209, arial12, 100.0f, 160.0f, "close", actionDetailsCloseClick) );

        actionDetailsWindow->Hide();

		setSelectedEffect(0);

//		game->moveCamera(b2Vec2(-17, -12));

        game->moveScreen(b2Vec2(-100.0f, -250.0f));

		game->loop();
	}

	return 0;
}


