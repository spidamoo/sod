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
GUIWindow* characterParamWindow;
GUIWindow* characterResourceWindow;
GUIWindow* characterStatusWindow;
GUIWindow* characterMoveTypeWindow;

int selectedCondition = -1;
int selectedCharacterParam = -1;
int selectedCharacterResource = -1;
int selectedCharacterStatus = -1;
int selectedCharacterMoveType = -1;

void interpretParam() {
    char buffer[64];
    switch ( game->getConditionPrototype(selectedCondition)->getType() ) {
        case CONDITION_TYPE_ADD_RESOURCE:
        case CONDITION_TYPE_ADD_FULL_RESOURCE:
        case CONDITION_TYPE_BOOST_RESOURCE:
        case CONDITION_TYPE_REGEN_RESOURCE:
            sprintf(
                buffer,
                "resource #%d: %s",
                game->getConditionPrototype(selectedCondition)->getParam(),
                game->getCharacterResourcePrototype( game->getConditionPrototype(selectedCondition)->getParam() )->getName()
            );
            break;
        case CONDITION_TYPE_ADD_PARAM:
            sprintf(
                buffer,
                "param #%d: %s",
                game->getConditionPrototype(selectedCondition)->getParam(),
                game->getCharacterParamPrototype( game->getConditionPrototype(selectedCondition)->getParam() )->getName()
            );
            break;
        case CONDITION_TYPE_ADD_STATUS:
        case CONDITION_TYPE_REMOVE_STATUS:
            sprintf(
                buffer,
                "status #%d: %s",
                game->getConditionPrototype(selectedCondition)->getParam(),
                game->getCharacterStatusPrototype( game->getConditionPrototype(selectedCondition)->getParam() )->getName()
            );
            break;
        case CONDITION_TYPE_CHANGE_MOVETYPE_SPEED:
            sprintf(
                buffer,
                "status #%d: %s",
                game->getConditionPrototype(selectedCondition)->getParam(),
                game->getCharacterMoveType( game->getConditionPrototype(selectedCondition)->getParam() )->getName()
            );
            break;
    }
    ( (hgeGUIMenuItem*)game->getGUI()->GetCtrl(6) )->setTitle( buffer );
}

void selectCondition(int index) {
    selectedCondition = index;
    if (index < 0 || index >= game->getConditionPrototypesCount())
        return;

    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(3) )->setTitle( game->getConditionPrototype(index)->getName() );

    char buffer[16];
    sprintf(buffer, "%i", game->getConditionPrototype(index)->getParam());
    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(5) )->setTitle( buffer );

    interpretParam();

    for (int i = 11; i <= 18; i++) {
        game->getGUI()->GetCtrl(i)->selected = false;
    }

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
        index--;
    }

    selectCondition(index);
}

void selectCharacterParam(int index) {
    selectedCharacterParam = index;
    if (index < 0 || index >= game->getCharacterParamPrototypesCount())
        return;

    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(203) )->setTitle( game->getCharacterParamPrototype(index)->getName() );

    char buffer[16];
    sprintf( buffer, "%.2f", game->getCharacterParamPrototype(index)->getDefaultNormalValue() );
    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(302) )->setTitle( buffer );

}
void addCharacterParam() {
    game->addCharacterParamPrototype();
    selectCharacterParam(game->getCharacterParamPrototypesCount() - 1);
}
void removeCharacterParam(int index) {
    game->removeCharacterParamPrototype(index);
    if (index >= game->getCharacterParamPrototypesCount()) {
        index--;
    }

    selectCharacterParam(index);
}

void displayEvent(int index) {
    int ctrlIndex = 510 + 5 * index;

    game->getGUI()->GetCtrl(ctrlIndex + 1)->bVisible = true;
    game->getGUI()->GetCtrl(ctrlIndex + 2)->Show();
    game->getGUI()->GetCtrl(ctrlIndex + 3)->bVisible = true;
    game->getGUI()->GetCtrl(ctrlIndex + 4)->Show();
    game->getGUI()->GetCtrl(ctrlIndex + 5)->Show();

    char buffer[16];
    sprintf( buffer, "%.2f", game->getCharacterResourcePrototype(selectedCharacterResource)->getEventPercent(index) );
    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(ctrlIndex + 2) )->setTitle( buffer );
    sprintf( buffer, "%d", game->getCharacterResourcePrototype(selectedCharacterResource)->getEventCondition(index) );
    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(ctrlIndex + 4) )->setTitle( buffer );
}
void hideEvent(int index) {
    int ctrlIndex = 510 + 5 * index;

    game->getGUI()->GetCtrl(ctrlIndex + 1)->Hide();
    game->getGUI()->GetCtrl(ctrlIndex + 2)->Hide();
    game->getGUI()->GetCtrl(ctrlIndex + 3)->Hide();
    game->getGUI()->GetCtrl(ctrlIndex + 4)->Hide();
    game->getGUI()->GetCtrl(ctrlIndex + 5)->Leave();
    game->getGUI()->GetCtrl(ctrlIndex + 5)->Hide();
}
void selectCharacterResource(int index) {
    selectedCharacterResource = index;
    if (index < 0 || index >= game->getCharacterResourcePrototypesCount())
        return;

    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(403) )->setTitle( game->getCharacterResourcePrototype(index)->getName() );

    char buffer[16];
    sprintf( buffer, "%.2f", game->getCharacterResourcePrototype(index)->getDefaultFullValue() );
    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(502) )->setTitle( buffer );

    sprintf( buffer, "%.2f", game->getCharacterResourcePrototype(index)->getDefaultRegen() );
    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(504) )->setTitle( buffer );

//    for (int i = 0; i < 16; i++) {
//        if (i < game->getCharacterResourcePrototype(index)->getEventsCount()) {
//            displayEvent(i);
//        }
//        else {
//            hideEvent(i);
//        }
//    }
//    float y = 170.0f + 20.0f * game->getCharacterResourcePrototype(index)->getEventsCount();
//    game->getGUI()->GetCtrl(510)->Move(780.0f, y);
//    game->getGUI()->GetCtrl(510)->Show();
}
void addCharacterResource() {
    game->addCharacterResourcePrototype();
    selectCharacterResource(game->getCharacterResourcePrototypesCount() - 1);
}
void removeCharacterResource(int index) {
    game->removeCharacterResourcePrototype(index);
    if (index >= game->getCharacterResourcePrototypesCount()) {
        index--;
    }

    selectCharacterResource(index);
}

void selectCharacterStatus(int index) {
    selectedCharacterStatus = index;
    if (index < 0 || index >= game->getCharacterStatusPrototypesCount())
        return;

    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(603) )->setTitle( game->getCharacterStatusPrototype(index)->getName() );
}
void addCharacterStatus() {
    game->addCharacterStatusPrototype();
    selectCharacterStatus(game->getCharacterStatusPrototypesCount() - 1);
}
void removeCharacterStatus(int index) {
    game->removeCharacterStatusPrototype(index);
    if (index >= game->getCharacterStatusPrototypesCount()) {
        index--;
    }

    selectCharacterStatus(index);
}

void selectCharacterMoveType(int index) {
    selectedCharacterMoveType = index;
    if (index < 0 || index >= game->getCharacterMoveTypesCount())
        return;

    ( (hgeGUIEditableLabel*)game->getGUI()->GetCtrl(803) )->setTitle( game->getCharacterMoveType(index)->getName() );
}
void addCharacterMoveType() {
    game->addCharacterMoveType();
    selectCharacterMoveType(game->getCharacterMoveTypesCount() - 1);
}
void removeCharacterMoveType(int index) {
    game->removeCharacterMoveType(index);
    if (index >= game->getCharacterMoveTypesCount()) {
        index--;
    }

    selectCharacterMoveType(index);
}

bool nameChange(hgeGUIObject* sender) {
    game->getConditionPrototype(selectedCondition)->setName( ((hgeGUIEditableLabel*)sender)->getTitle() );
}
bool paramChange(hgeGUIObject* sender) {
    game->getConditionPrototype(selectedCondition)->setParam( atoi( ((hgeGUIEditableLabel*)sender)->getTitle() ) );
    interpretParam();
}

bool conditionTypeClick(hgeGUIObject* sender) {
    for (int i = 11; i <= 18; i++) {
        game->getGUI()->GetCtrl(i)->selected = false;
    }

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

bool characterParamNameChange(hgeGUIObject* sender) {
    game->getCharacterParamPrototype(selectedCharacterParam)->setName( ((hgeGUIEditableLabel*)sender)->getTitle() );
}
bool defaultNormalValueChange(hgeGUIObject* sender) {
    game->getCharacterParamPrototype(selectedCharacterParam)->setDefaultNormalValue( atof( ((hgeGUIEditableLabel*)sender)->getTitle() ) );
}

bool characterResourceNameChange(hgeGUIObject* sender) {
    game->getCharacterResourcePrototype(selectedCharacterResource)->setName( ((hgeGUIEditableLabel*)sender)->getTitle() );
}
bool defaultFullValueChange(hgeGUIObject* sender) {
    game->getCharacterResourcePrototype(selectedCharacterResource)->setDefaultFullValue( atof( ((hgeGUIEditableLabel*)sender)->getTitle() ) );
}
bool defaultRegenChange(hgeGUIObject* sender) {
    game->getCharacterResourcePrototype(selectedCharacterResource)->setDefaultRegen( atof( ((hgeGUIEditableLabel*)sender)->getTitle() ) );
}
bool percentChange(hgeGUIObject* sender) {
    int index = (sender->id - 512) / 5;
    game->getCharacterResourcePrototype(selectedCharacterResource)->setEventPercent( index, atof( ((hgeGUIEditableLabel*)sender)->getTitle() ) );
}
bool conditionChange(hgeGUIObject* sender) {
    int index = (sender->id - 514) / 5;
    game->getCharacterResourcePrototype(selectedCharacterResource)->setEventCondition( index, atoi( ((hgeGUIEditableLabel*)sender)->getTitle() ) );
}
bool removeEventClick(hgeGUIObject* sender) {
    int index = (sender->id - 515) / 5;

    game->getCharacterResourcePrototype(selectedCharacterResource)->removeEvent(index);
    hideEvent(index);

    float y = 170.0f + 20.0f * game->getCharacterResourcePrototype(selectedCharacterResource)->getEventsCount();
    game->getGUI()->GetCtrl(510)->Move(780.0f, y);
}
bool addEventClick(hgeGUIObject* sender) {
    if (selectedCharacterResource < 0 || selectedCharacterResource >= game->getCharacterResourcePrototypesCount())
        return true;

    game->getCharacterResourcePrototype(selectedCharacterResource)->addEvent();

    float y = 170.0f + 20.0f * game->getCharacterResourcePrototype(selectedCharacterResource)->getEventsCount();

    displayEvent( game->getCharacterResourcePrototype(selectedCharacterResource)->getEventsCount() - 1 );

    sender->Move(780.0f, y);
}

bool characterStatusNameChange(hgeGUIObject* sender) {
    game->getCharacterStatusPrototype(selectedCharacterStatus)->setName( ((hgeGUIEditableLabel*)sender)->getTitle() );
}

bool characterMoveTypeNameChange(hgeGUIObject* sender) {
    game->getCharacterMoveType(selectedCharacterMoveType)->setName( ((hgeGUIEditableLabel*)sender)->getTitle() );
}

void saveConditions(const char* filename) {
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
void saveCharacterParams(const char* filename) {
    TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

	TiXmlElement* root = new TiXmlElement( "character_param_prototypes" );
	root->SetAttribute("count", game->getCharacterParamPrototypesCount());

	for (int i = 0; i < game->getCharacterParamPrototypesCount(); i++) {
        TiXmlElement* element = new TiXmlElement( "prototype" );
        game->getCharacterParamPrototype(i)->saveToXml(element);
        root->LinkEndChild(element);
    }
    doc.LinkEndChild( root );
	doc.SaveFile(filename);
}
void saveCharacterResources(const char* filename) {
    TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

	TiXmlElement* root = new TiXmlElement( "character_resource_prototypes" );
	root->SetAttribute("count", game->getCharacterResourcePrototypesCount());

	for (int i = 0; i < game->getCharacterResourcePrototypesCount(); i++) {
        TiXmlElement* element = new TiXmlElement( "prototype" );
        game->getCharacterResourcePrototype(i)->saveToXml(element);
        root->LinkEndChild(element);
    }
    doc.LinkEndChild( root );
	doc.SaveFile(filename);
}
void saveCharacterStatuses(const char* filename) {
    TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

	TiXmlElement* root = new TiXmlElement( "character_status_prototypes" );
	root->SetAttribute("count", game->getCharacterStatusPrototypesCount());

	for (int i = 0; i < game->getCharacterStatusPrototypesCount(); i++) {
        TiXmlElement* element = new TiXmlElement( "prototype" );
        game->getCharacterStatusPrototype(i)->saveToXml(element);
        root->LinkEndChild(element);
    }
    doc.LinkEndChild( root );
	doc.SaveFile(filename);
}
void saveCharacterMoveTypes(const char* filename) {
    TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

	TiXmlElement* root = new TiXmlElement( "character_move_types" );
	root->SetAttribute("count", game->getCharacterMoveTypesCount());

	for (int i = 0; i < game->getCharacterMoveTypesCount(); i++) {
        TiXmlElement* element = new TiXmlElement( "prototype" );
        game->getCharacterMoveType(i)->saveToXml(element);
        root->LinkEndChild(element);
    }
    doc.LinkEndChild( root );
	doc.SaveFile(filename);
}

bool saveButtonClick(hgeGUIObject* sender) {
    saveConditions("conditions.xml");
    saveCharacterParams("character_params.xml");
    saveCharacterResources("character_resources.xml");
    saveCharacterStatuses("character_statuses.xml");
    saveCharacterMoveTypes("move_types.xml");
}
bool loadButtonClick(hgeGUIObject* sender) {
    game->loadConditionPrototypes("conditions.xml");
    game->loadCharacterParamPrototypes("character_params.xml");
    game->loadCharacterResourcePrototypes("character_resources.xml");
    game->loadCharacterStatusPrototypes("character_statuses.xml");
    game->loadCharacterMoveTypes("move_types.xml");
    selectCondition(0);
    selectCharacterParam(0);
    selectCharacterResource(0);
    selectCharacterStatus(0);
    selectCharacterMoveType(0);
}

bool FrameFunc() {
	float x, y;
	game->getHge()->Input_GetMousePos(&x, &y);
	float worldX = game->worldX(x);
	float worldY = game->worldY(y);

    /// conditions
    //{
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
    //}

    /// character params
    //{
    if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && x > 190 && x < 200
        && y > 450 + game->getCharacterParamPrototypesCount() * 16 && y < 466 + game->getCharacterParamPrototypesCount() * 16
    ) {
        addCharacterParam();
    }
    else {
        for (int i = 0; i < game->getCharacterParamPrototypesCount(); i++) {
            if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && y > 450 + i * 16 && y < 466 + i * 16) {
                if (x > 0 && x < 190) {
                    selectCharacterParam(i);
                }
                if (x > 190 && x < 200) {
                    removeCharacterParam(i);
                }
            }
        }
    }
    //}

    /// character resources
    //{
    if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && x > 690 && x < 700
        && y > game->getCharacterResourcePrototypesCount() * 16 && y < 16 + game->getCharacterResourcePrototypesCount() * 16
    ) {
        addCharacterResource();
    }
    else {
        for (int i = 0; i < game->getCharacterResourcePrototypesCount(); i++) {
            if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && y > i * 16 && y < 16 + i * 16) {
                if (x > 500 && x < 690) {
                    selectCharacterResource(i);
                }
                if (x > 690 && x < 700) {
                    removeCharacterResource(i);
                }
            }
        }
    }
    //}

    /// character statuses
    //{
    if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && x > 690.0f && x < 700.0f
        && y > 450.0f + game->getCharacterStatusPrototypesCount() * 16.0f && y < 466.0f + game->getCharacterStatusPrototypesCount() * 16.0f
    ) {
        addCharacterStatus();
    }
    else {
        for (int i = 0; i < game->getCharacterStatusPrototypesCount(); i++) {
            if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && y > 450.0f + i * 16.0f && y < 466.0f + i * 16.0f) {
                if (x > 500.0f && x < 690.0f) {
                    selectCharacterStatus(i);
                }
                if (x > 690.0f && x < 700.0f) {
                    removeCharacterStatus(i);
                }
            }
        }
    }
    //}

    /// character move types
    //{
    if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && x > 1190.0f && x < 1200.0f
        && y > game->getCharacterMoveTypesCount() * 16.0f && y < 16.0f + game->getCharacterMoveTypesCount() * 16.0f
    ) {
        addCharacterMoveType();
    }
    else {
        for (int i = 0; i < game->getCharacterMoveTypesCount(); i++) {
            if (game->getHge()->Input_KeyDown(HGEK_LBUTTON) && y > i * 16.0f && y < 16.0f + i * 16.0f) {
                if (x > 1000.0f && x < 1190.0f) {
                    selectCharacterMoveType(i);
                }
                if (x > 1190.0f && x < 1200.0f) {
                    removeCharacterMoveType(i);
                }
            }
        }
    }
    //}


	return game->update(true);
}

bool RenderFunc() {
	game->startDraw();

	float x, y;
	game->getHge()->Input_GetMousePos(&x, &y);

    /// conditions
    //{
    game->drawRect(1.0f, 1.0f, 200.0f, 450.0f, 0xFF000000, 0xFFFFFFFF);

    for (int i = 0; i < game->getConditionPrototypesCount(); i++) {
        DWORD color = 0xFF000000;
        if (i == selectedCondition) {
            color = 0xFF0000FF;
            game->drawRect(1, i * 16, 200, 16 + i * 16, color, 0);
        }
        if (y > i * 16.0f && y < 16.0f + i * 16.0f) {
            if (x > 190.0f && x < 200.0f) {
                color = 0xFFFF0000;
                game->drawRect(1, i * 16, 200, 16 + i * 16, color, 0);
            }
            else if (x > 0.0f && x < 200.0f) {
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
    //}

    /// character params
    //{
    game->drawRect(1.0f, 451.0f, 200.0f, 900.0f, 0xFF000000, 0xFFFFFFFF);

    for (int i = 0; i < game->getCharacterParamPrototypesCount(); i++) {
        DWORD color = 0xFF000000;
        if (i == selectedCharacterParam) {
            color = 0xFF0000FF;
            game->drawRect(1, 450 + i * 16, 200, 466 + i * 16, color, 0);
        }
        if (y > 450.0f + i * 16.0f && y < 466.0f + i * 16.0f) {
            if (x > 190.0f && x < 200.0f) {
                color = 0xFFFF0000;
                game->drawRect(1, 450 + i * 16, 200, 466 + i * 16, color, 0);
            }
            else if (x > 0.0f && x < 190.0f) {
                color = 0xFF0000FF;
            }
        }
        arial12->SetColor(color);
        arial12->printf( 0, 450 + i * 16, HGETEXT_LEFT, "%d: %s", i, game->getCharacterParamPrototype(i)->getName() );
        arial12->printf( 191, 450 + i * 16, HGETEXT_LEFT, "-");
    }

    color = 0xFF000000;
    if (x > 190 && x < 200 && y > 450 + game->getCharacterParamPrototypesCount() * 16 && y < 466 + game->getCharacterParamPrototypesCount() * 16) {
        color = 0xFF00AA00;
        game->drawRect(1, 450 + game->getCharacterParamPrototypesCount() * 16, 200, 466 + game->getCharacterParamPrototypesCount() * 16, color, 0);
    }
    arial12->SetColor(color);
    arial12->printf( 190, 450 + game->getCharacterParamPrototypesCount() * 16, HGETEXT_LEFT, "+");
    //}

    /// character resources
    //{
    game->drawRect(501.0f, 1.0f, 700.0f, 450.0f, 0xFF000000, 0xFFFFFFFF);

    for (int i = 0; i < game->getCharacterResourcePrototypesCount(); i++) {
        DWORD color = 0xFF000000;
        if (i == selectedCharacterResource) {
            color = 0xFF0000FF;
            game->drawRect(501.0f, i * 16.0f, 700.0f, 16.0f + i * 16.0f, color, 0);
        }
        if (y > i * 16.0f && y < 16.0f + i * 16.0f) {
            if (x > 690.0f && x < 700.0f) {
                color = 0xFFFF0000;
                game->drawRect(501.0f, i * 16.0f, 700.0f, 16.0f + i * 16.0f, color, 0);
            }
            else if (x > 500.0f && x < 690.0f) {
                color = 0xFF0000FF;
            }
        }
        arial12->SetColor(color);
        arial12->printf( 500.0f, i * 16.0f, HGETEXT_LEFT, "%d: %s", i, game->getCharacterResourcePrototype(i)->getName() );
        arial12->printf( 691.0f, i * 16, HGETEXT_LEFT, "-");
    }

    color = 0xFF000000;
    if (x > 690.0f && x < 700.0f && y > game->getCharacterResourcePrototypesCount() * 16.0f && y < 16.0f + game->getCharacterResourcePrototypesCount() * 16.0f) {
        color = 0xFF00AA00;
        game->drawRect(501.0f, game->getCharacterResourcePrototypesCount() * 16.0f, 700.0f, 16.0f + game->getCharacterResourcePrototypesCount() * 16.0f, color, 0);
    }
    arial12->SetColor(color);
    arial12->printf( 690.0f, game->getCharacterResourcePrototypesCount() * 16.0f, HGETEXT_LEFT, "+");
    //}

    /// character statuses
    //{
    game->drawRect(501.0f, 451.0f, 700.0f, 900.0f, 0xFF000000, 0xFFFFFFFF);

    for (int i = 0; i < game->getCharacterStatusPrototypesCount(); i++) {
        DWORD color = 0xFF000000;
        if (i == selectedCharacterStatus) {
            color = 0xFF0000FF;
            game->drawRect(501.0f, 450.0f + i * 16.0f, 700.0f, 466.0f + i * 16.0f, color, 0);
        }
        if (y > 450.0f + i * 16.0f && y < 466.0f + i * 16.0f) {
            if (x > 690.0f && x < 700.0f) {
                color = 0xFFFF0000;
                game->drawRect(501.0f, 450.0f + i * 16.0f, 700.0f, 466.0f + i * 16.0f, color, 0);
            }
            else if (x > 500.0f && x < 690.0f) {
                color = 0xFF0000FF;
            }
        }
        arial12->SetColor(color);
        arial12->printf( 500.0f, 450.0f + i * 16.0f, HGETEXT_LEFT, "%d: %s", i, game->getCharacterStatusPrototype(i)->getName() );
        arial12->printf( 691.0f, 450.0f + i * 16.0f, HGETEXT_LEFT, "-");
    }

    color = 0xFF000000;
    if (x > 690.0f && x < 700.0f
        && y > 450.0f + game->getCharacterStatusPrototypesCount() * 16.0f && y < 466.0f + game->getCharacterStatusPrototypesCount() * 16.0f
    ) {
        color = 0xFF00AA00;
        game->drawRect(501.0f, 450.0f + game->getCharacterStatusPrototypesCount() * 16.0f, 700.0f, 466.0f + game->getCharacterStatusPrototypesCount() * 16.0f, color, 0);
    }
    arial12->SetColor(color);
    arial12->printf( 690.0f, 450.0f + game->getCharacterStatusPrototypesCount() * 16.0f, HGETEXT_LEFT, "+");
    //}

    /// character move types
    //{
    game->drawRect(1001.0f, 1.0f, 1200.0f, 450.0f, 0xFF000000, 0xFFFFFFFF);

    for (int i = 0; i < game->getCharacterMoveTypesCount(); i++) {
        DWORD color = 0xFF000000;
        if (i == selectedCharacterMoveType) {
            color = 0xFF0000FF;
            game->drawRect(1001.0f, i * 16.0f, 1200.0f, 16.0f + i * 16.0f, color, 0);
        }
        if (y > i * 16.0f && y < 16.0f + i * 16.0f) {
            if (x > 1190.0f && x < 1200.0f) {
                color = 0xFFFF0000;
                game->drawRect(1001.0f, i * 16.0f, 1200.0f, 16.0f + i * 16.0f, color, 0);
            }
            else if (x > 1000.0f && x < 1190.0f) {
                color = 0xFF0000FF;
            }
        }
        arial12->SetColor(color);
        arial12->printf( 1000.0f, i * 16.0f, HGETEXT_LEFT, "%d: %s", i, game->getCharacterMoveType(i)->getName() );
        arial12->printf( 1191.0f, i * 16, HGETEXT_LEFT, "-");
    }

    color = 0xFF000000;
    if (x > 1190.0f && x < 1200.0f && y > game->getCharacterMoveTypesCount() * 16.0f && y < 16.0f + game->getCharacterMoveTypesCount() * 16.0f) {
        color = 0xFF00AA00;
        game->drawRect(1001.0f, game->getCharacterMoveTypesCount() * 16.0f, 1200.0f, 16.0f + game->getCharacterMoveTypesCount() * 16.0f, color, 0);
    }
    arial12->SetColor(color);
    arial12->printf( 1190.0f, game->getCharacterMoveTypesCount() * 16.0f, HGETEXT_LEFT, "+");
    //}


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

        game->loadCharacterParamPrototypes("character_params.xml");
        game->loadCharacterResourcePrototypes("character_resources.xml");
        game->loadCharacterStatusPrototypes("character_statuses.xml");
        game->loadCharacterMoveTypes("move_types.xml");
		game->loadEffectPrototypes("effects.xml");
		game->loadConditionPrototypes("conditions.xml");

		mainWindow = new GUIWindow(game, 1500, 1501.0f, 0.0f, 100.0f, 901.0f);
		game->getGUI()->AddCtrl(mainWindow);
		mainWindow->AddCtrl(new hgeGUIMenuItem(1501, fnt, 50.0f, 10.0f, "save", saveButtonClick));
		mainWindow->AddCtrl(new hgeGUIMenuItem(1502, fnt, 50.0f, 40.0f, "load", loadButtonClick));

        /// conditions
        //{
		conditionWindow = new GUIWindow(game, 1, 200.0f, 1.0f, 300.0f, 449.0f);
		game->getGUI()->AddCtrl(conditionWindow);

		hgeGUIMenuItem* conditionsLabel = new hgeGUIMenuItem(2, fnt, 150.0f, 5.0f, "conditions", NULL);
		conditionsLabel->bEnabled = false;
		conditionWindow->AddCtrl(conditionsLabel);

		hgeGUIEditableLabel* nameInput = new hgeGUIEditableLabel(game, 3, arial12, 10.0f, 40.0f, 200.0f, 17.0f, "");
		nameInput->setOnChange(nameChange);
		conditionWindow->AddCtrl(nameInput);

		hgeGUIMenuItem* paramLabel = new hgeGUIMenuItem(4, arial12, 30.0, 60.0f, "param", NULL);
		paramLabel->bEnabled = false;
		conditionWindow->AddCtrl(paramLabel);

		hgeGUIEditableLabel* paramInput = new hgeGUIEditableLabel(game, 5, arial12, 60.0f, 60.0f, 30.0f, 17.0f, "");
		paramInput->setOnChange(paramChange);
		conditionWindow->AddCtrl(paramInput);

		hgeGUIMenuItem* paramValueLabel = new hgeGUIMenuItem(6, arial12, 100.0, 60.0f, "", NULL);
		paramValueLabel->bEnabled = false;
		conditionWindow->AddCtrl(paramValueLabel);

		conditionWindow->AddCtrl( new hgeGUIMenuItem(11, arial12, 50.0f, 80.0f, "resource add", conditionTypeClick) );
		conditionWindow->AddCtrl( new hgeGUIMenuItem(12, arial12, 120.0f, 80.0f, "add full", conditionTypeClick) );
		conditionWindow->AddCtrl( new hgeGUIMenuItem(13, arial12, 190.0f, 80.0f, "boost", conditionTypeClick) );
		conditionWindow->AddCtrl( new hgeGUIMenuItem(14, arial12, 260.0f, 80.0f, "add regen", conditionTypeClick) );

		conditionWindow->AddCtrl( new hgeGUIMenuItem(15, arial12, 50.0f, 100.0f, "add param", conditionTypeClick) );

		conditionWindow->AddCtrl( new hgeGUIMenuItem(16, arial12, 50.0f, 120.0f, "add status", conditionTypeClick) );
		conditionWindow->AddCtrl( new hgeGUIMenuItem(17, arial12, 150.0f, 120.0f, "remove status", conditionTypeClick) );

		conditionWindow->AddCtrl( new hgeGUIMenuItem(18, arial12, 50.0f, 140.0f, "change speed", conditionTypeClick) );

		hgeGUIMenuItem* valueLabel = new hgeGUIMenuItem(101, arial12, 40.0f, 170.0f, "value", NULL);
		valueLabel->bEnabled = false;
		conditionWindow->AddCtrl(valueLabel);

		hgeGUIEditableLabel* valueInput = new hgeGUIEditableLabel(game, 102, arial12, 80.0f, 170.0f, 200.0f, 17.0f, "");
		valueInput->setOnChange(valueChange);
		conditionWindow->AddCtrl(valueInput);

		hgeGUIMenuItem* durationLabel = new hgeGUIMenuItem(103, arial12, 40.0f, 190.0f, "duration", NULL);
		durationLabel->bEnabled = false;
		conditionWindow->AddCtrl(durationLabel);

		hgeGUIEditableLabel* durationInput = new hgeGUIEditableLabel(game, 104, arial12, 80.0f, 190.0f, 200.0f, 17.0f, "");
		durationInput->setOnChange(durationChange);
		conditionWindow->AddCtrl(durationInput);

		hgeGUIMenuItem* intervalLabel = new hgeGUIMenuItem(105, arial12, 40.0f, 210.0f, "interval", NULL);
		intervalLabel->bEnabled = false;
		conditionWindow->AddCtrl(intervalLabel);

		hgeGUIEditableLabel* intervalInput = new hgeGUIEditableLabel(game, 106, arial12, 80.0f, 210.0f, 200.0f, 17.0f, "");
		intervalInput->setOnChange(intervalChange);
		conditionWindow->AddCtrl(intervalInput);

        //}

        /// character params
        //{
		characterParamWindow = new GUIWindow(game, 201, 200.0f, 451.0f, 300.0f, 449.0f);
		game->getGUI()->AddCtrl(characterParamWindow);

		hgeGUIMenuItem* characterParamsLabel = new hgeGUIMenuItem(202, fnt, 150.0f, 5.0f, "character params", NULL);
		characterParamsLabel->bEnabled = false;
		characterParamWindow->AddCtrl(characterParamsLabel);

		nameInput = new hgeGUIEditableLabel(game, 203, arial12, 10.0f, 40.0f, 200.0f, 17.0f, "");
		nameInput->setOnChange(characterParamNameChange);
		characterParamWindow->AddCtrl(nameInput);

		hgeGUIMenuItem* defaultNormalValueLabel = new hgeGUIMenuItem(301, arial12, 80.0f, 100.0f, "default normal value", NULL);
		defaultNormalValueLabel->bEnabled = false;
		characterParamWindow->AddCtrl(defaultNormalValueLabel);

		hgeGUIEditableLabel* defaultNormalValueInput = new hgeGUIEditableLabel(game, 302, arial12, 160.0f, 100.0f, 100.0f, 17.0f, "");
		defaultNormalValueInput->setOnChange(defaultNormalValueChange);
		characterParamWindow->AddCtrl(defaultNormalValueInput);

        //}

        /// character resources
        //{
		characterResourceWindow = new GUIWindow(game, 401, 700.0f, 1.0f, 300.0f, 449.0f);
		game->getGUI()->AddCtrl(characterResourceWindow);

		hgeGUIMenuItem* characterResourcesLabel = new hgeGUIMenuItem(402, fnt, 150.0f, 5.0f, "character resources", NULL);
		characterResourcesLabel->bEnabled = false;
		characterResourceWindow->AddCtrl(characterResourcesLabel);

		nameInput = new hgeGUIEditableLabel(game, 403, arial12, 10.0f, 40.0f, 200.0f, 17.0f, "");
		nameInput->setOnChange(characterResourceNameChange);
		characterResourceWindow->AddCtrl(nameInput);

		hgeGUIMenuItem* defaultFullValueLabel = new hgeGUIMenuItem(501, arial12, 80.0f, 100.0f, "default full value", NULL);
		defaultFullValueLabel->bEnabled = false;
		characterResourceWindow->AddCtrl(defaultFullValueLabel);

		hgeGUIEditableLabel* defaultFullValueInput = new hgeGUIEditableLabel(game, 502, arial12, 160.0f, 100.0f, 100.0f, 17.0f, "");
		defaultFullValueInput->setOnChange(defaultFullValueChange);
		characterResourceWindow->AddCtrl(defaultFullValueInput);

		hgeGUIMenuItem* defaultRegenLabel = new hgeGUIMenuItem(503, arial12, 80.0f, 120.0f, "default regen", NULL);
		defaultRegenLabel->bEnabled = false;
		characterResourceWindow->AddCtrl(defaultRegenLabel);

		hgeGUIEditableLabel* defaultRegenInput = new hgeGUIEditableLabel(game, 504, arial12, 160.0f, 120.0f, 100.0f, 17.0f, "");
		defaultRegenInput->setOnChange(defaultRegenChange);
		characterResourceWindow->AddCtrl(defaultRegenInput);

//		characterResourceWindow->AddCtrl(new hgeGUIMenuItem(510, arial12, 105.0f, 190.0f, "add event", addEventClick));
//		game->getGUI()->GetCtrl(510)->Hide();

		for (int i = 0; i < 16; i++) {
            int index = 510 + i * 5;
            float y = 170.0f + 20.0f * i;
            hgeGUIMenuItem* percentLabel = new hgeGUIMenuItem(index + 1, arial12, 35.0f, y, "level", NULL);
            percentLabel->bEnabled = false;
            characterResourceWindow->AddCtrl(percentLabel);
            percentLabel->Hide();

            hgeGUIEditableLabel* percentInput = new hgeGUIEditableLabel(game, index + 2, arial12, 70.0f, y, 30.0f, 17.0f, "");
            percentInput->setOnChange(percentChange);
            characterResourceWindow->AddCtrl(percentInput);
            percentInput->Hide();

            hgeGUIMenuItem* conditionLabel = new hgeGUIMenuItem(index + 3, arial12, 135.0f, y, "condition", NULL);
            conditionLabel->bEnabled = false;
            characterResourceWindow->AddCtrl(conditionLabel);
            conditionLabel->Hide();

            hgeGUIEditableLabel* conditionInput = new hgeGUIEditableLabel(game, index + 4, arial12, 170.0f, y, 30.0f, 17.0f, "");
            conditionInput->setOnChange(conditionChange);
            characterResourceWindow->AddCtrl(conditionInput);
            conditionInput->Hide();

            hgeGUIMenuItem* removeButton = new hgeGUIMenuItem(index + 5, arial12, 240.0f, y, "remove", removeEventClick);
            characterResourceWindow->AddCtrl(removeButton);
            removeButton->Hide();
		}

        //}

        /// character statuses
        //{
		characterStatusWindow = new GUIWindow(game, 601, 700.0f, 451.0f, 300.0f, 449.0f);
		game->getGUI()->AddCtrl(characterStatusWindow);

		hgeGUIMenuItem* characterStatusesLabel = new hgeGUIMenuItem(602, fnt, 150.0f, 5.0f, "character statuses", NULL);
		characterStatusesLabel->bEnabled = false;
		characterStatusWindow->AddCtrl(characterStatusesLabel);

		nameInput = new hgeGUIEditableLabel(game, 603, arial12, 10.0f, 40.0f, 200.0f, 17.0f, "");
		nameInput->setOnChange(characterStatusNameChange);
		characterStatusWindow->AddCtrl(nameInput);

        //}

        /// character move types
        //{
		characterMoveTypeWindow = new GUIWindow(game, 801, 1200.0f, 1.0f, 300.0f, 449.0f);
		game->getGUI()->AddCtrl(characterMoveTypeWindow);

		hgeGUIMenuItem* characterMoveTypesLabel = new hgeGUIMenuItem(802, fnt, 150.0f, 5.0f, "character move types", NULL);
		characterMoveTypesLabel->bEnabled = false;
		characterMoveTypeWindow->AddCtrl(characterMoveTypesLabel);

		nameInput = new hgeGUIEditableLabel(game, 803, arial12, 10.0f, 40.0f, 200.0f, 17.0f, "");
		nameInput->setOnChange(characterMoveTypeNameChange);
		characterMoveTypeWindow->AddCtrl(nameInput);

        //}

		game->loop();
	}

	return 0;
}


