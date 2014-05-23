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
GUIWindow* conditionWindow;

int selectedCondition = -1;

void selectCondition(int index) {
    selectedCondition = index;

    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(3) )->setTitle( game->getConditionPrototype(index)->getName() );

    game->getGUI()->GetCtrl(11)->selected = false;
    game->getGUI()->GetCtrl(12)->selected = false;

    game->getGUI()->GetCtrl( 10 + game->getConditionPrototype(index)->getType() )->selected = true;

    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(102) )->setTitle( game->getConditionPrototype(index)->getValueFormula() );
    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(104) )->setTitle( game->getConditionPrototype(index)->getDurationFormula() );
    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(106) )->setTitle( game->getConditionPrototype(index)->getIntervalFormula() );

}

void addCondition() {
    game->addConditionPrototype();
    selectCondition(game->getConditionPrototypesCount() - 1);
}
void removeCondition(int index) {
    game->removeConditionPrototype(index);
    if (index >= game->getConditionPrototypesCount()) {
        selectedCondition--;
    }

    selectCondition(selectedCondition);
}

bool nameChange(hgeGUIObject* sender) {
    game->getConditionPrototype(selectedCondition)->setName( ((hgeGUIEditableLabel*)sender)->getTitle() );
}

bool conditionTypeClick(hgeGUIObject* sender) {
    game->getGUI()->GetCtrl(11)->selected = false;
    game->getGUI()->GetCtrl(12)->selected = false;

    sender->selected = true;

    game->getConditionPrototype(selectedCondition)->setType(sender->id - 10);
}

bool valueChange(hgeGUIObject* sender) {
    game->getConditionPrototype(selectedCondition)->setValueFormula( ((hgeGUIEditableLabel*)sender)->getTitle() );
}
bool durationChange(hgeGUIObject* sender) {
    game->getConditionPrototype(selectedCondition)->setDurationFormula( ((hgeGUIEditableLabel*)sender)->getTitle() );
}
bool intervalChange(hgeGUIObject* sender) {
    game->getConditionPrototype(selectedCondition)->setIntervalFormula( ((hgeGUIEditableLabel*)sender)->getTitle() );
}

void save(const char* filename) {
    TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

	TiXmlElement* root = new TiXmlElement( "condition_prototypes" );
	root->SetAttribute("count", game->getConditionPrototypesCount());

	for (int i = 0; i < game->getConditionPrototypesCount(); i++) {
        TiXmlElement* element = new TiXmlElement( "prototype" );
        game->getConditionPrototype(i)->saveToXml(element);
        root->LinkEndChild(element);
    }
    doc.LinkEndChild( root );
	doc.SaveFile(filename);
}

bool saveButtonClick(hgeGUIObject* sender) {
    save("conditions.xml");
}
bool loadButtonClick(hgeGUIObject* sender) {
    game->loadConditionPrototypes("conditions.xml");
    selectCondition(0);
}

bool FrameFunc() {
	float x, y;
	game->getHge()->Input_GetMousePos(&x, &y);
	float worldX = game->worldX(x);
	float worldY = game->worldY(y);

    if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && x > 190 && x < 200 && y > game->getConditionPrototypesCount() * 16 && y < 16 + game->getConditionPrototypesCount() * 16) {
        addCondition();
    }
    else {
        for (int i = 0; i < game->getConditionPrototypesCount(); i++) {
            if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && y > i * 16 && y < 16 + i * 16) {
                if (x > 0 && x < 190) {
                    selectCondition(i);
                }
                if (x > 190 && x < 200) {
                    removeCondition(i);
                }
            }
        }
    }

	return game->update(true);
}

bool RenderFunc() {
	game->startDraw();

	float x, y;
	game->getHge()->Input_GetMousePos(&x, &y);

    game->drawRect(1, 1, 200, 900, 0xFF000000, 0xFFFFFFFF);

    for (int i = 0; i < game->getConditionPrototypesCount(); i++) {
        DWORD color = 0xFF000000;
        if (i == selectedCondition) {
            color = 0xFF0000FF;
            game->drawRect(1, i * 16, 200, 16 + i * 16, color, 0);
        }
        if (y > i * 16 && y < 16 + i * 16) {
            if (x > 190 && x < 200) {
                color = 0xFFFF0000;
                game->drawRect(1, i * 16, 200, 16 + i * 16, color, 0);
            }
            else {
                color = 0xFF0000FF;
            }
        }
        arial12->SetColor(color);
        arial12->printf( 0, i * 16, HGETEXT_LEFT, "%d: %s", i, game->getConditionPrototype(i)->getName() );
        arial12->printf( 191, i * 16, HGETEXT_LEFT, "-");
    }

    DWORD color = 0xFF000000;
    if (x > 190 && x < 200 && y > game->getConditionPrototypesCount() * 16 && y < 16 + game->getConditionPrototypesCount() * 16) {
        color = 0xFF00AA00;
        game->drawRect(1, game->getConditionPrototypesCount() * 16, 200, 16 + game->getConditionPrototypesCount() * 16, color, 0);
    }
    arial12->SetColor(color);
    arial12->printf( 190, game->getConditionPrototypesCount() * 16, HGETEXT_LEFT, "+");


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
	hge->System_SetState(HGE_LOGFILE, "conditions_editor.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "SoD conditions editor");

	game = new Game(hge);


	if ( game->preload() ) {
		fnt = new hgeFont("font1.fnt");
		arial12 = new hgeFont("arial12.fnt");

        game->loadConditionPrototypes("conditions.xml");

		mainWindow = new GUIWindow(game, 500, 1300, 0, 300, 500);
		game->getGUI()->AddCtrl(mainWindow);
		mainWindow->AddCtrl(new hgeGUIMenuItem(501, fnt, 120, 60, "save", saveButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(502, fnt, 180, 60, "load", loadButtonClick));

		conditionWindow = new GUIWindow(game, 2, 200, 0, 400, 400);
		game->getGUI()->AddCtrl(conditionWindow);

		hgeGUIEditableLabel* nameInput = new hgeGUIEditableLabel(game, 3, arial12, 10.0f, 10.0f, 200.0f, 17.0f, "");
		nameInput->setOnChange(nameChange);
		conditionWindow->AddCtrl(nameInput);

		conditionWindow->AddCtrl( new hgeGUIMenuItem(11, arial12, 50, 30, "damage", conditionTypeClick) );
		conditionWindow->AddCtrl( new hgeGUIMenuItem(12, arial12, 100, 30, "stun", conditionTypeClick) );

		hgeGUIMenuItem* valueLabel = new hgeGUIMenuItem(101, arial12, 40, 100, "value", NULL);
		valueLabel->bEnabled = false;
		conditionWindow->AddCtrl(valueLabel);

		hgeGUIEditableLabel* valueInput = new hgeGUIEditableLabel(game, 102, arial12, 100.0f, 100.0f, 200.0f, 17.0f, "");
		valueInput->setOnChange(valueChange);
		conditionWindow->AddCtrl(valueInput);

		hgeGUIMenuItem* durationLabel = new hgeGUIMenuItem(103, arial12, 40, 120, "duration", NULL);
		durationLabel->bEnabled = false;
		conditionWindow->AddCtrl(durationLabel);

		hgeGUIEditableLabel* durationInput = new hgeGUIEditableLabel(game, 104, arial12, 100.0f, 120.0f, 200.0f, 17.0f, "");
		durationInput->setOnChange(durationChange);
		conditionWindow->AddCtrl(durationInput);

		hgeGUIMenuItem* intervalLabel = new hgeGUIMenuItem(105, arial12, 40, 140, "interval", NULL);
		intervalLabel->bEnabled = false;
		conditionWindow->AddCtrl(intervalLabel);

		hgeGUIEditableLabel* intervalInput = new hgeGUIEditableLabel(game, 106, arial12, 100.0f, 140.0f, 200.0f, 17.0f, "");
		intervalInput->setOnChange(intervalChange);
		conditionWindow->AddCtrl(intervalInput);

        selectCondition(0);
//        game->moveScreen(b2Vec2(-750.0f, - 700.0f));

		game->loop();
	}

	return 0;
}


