#include "hgeGUIEditableLabel.h"

hgeGUIEditableLabel::hgeGUIEditableLabel(Game* _game, int _id, hgeFont *_fnt,
    float _x, float _y, float _w, float _h,
    char *_title
) {
    game = _game;
    id = _id;
    fnt = _fnt;
    title = _title;
    isFocused = false;
    isEdited = false;

    bStatic=false; bVisible=true; bEnabled=true;

    rect.Set(_x, _y, _x + _w, _y + _h);

    setTitle(_title);

    time = 0.0f;

    onChange = NULL;
}


void hgeGUIEditableLabel::Render() {
    game->drawRect(rect.x1, rect.y1, rect.x2, rect.y2, 0xFF555555, 0xFFFAFAFA);
    DWORD color = 0xFF000000;
    if (isFocused && !isEdited) {
        color = 0xFF0000AA;
    }
    fnt->SetColor(color);
    if (isEdited) {
        if (time < 0.5f)
            fnt->Render(rect.x1 + fnt->GetStringWidth(title), rect.y1, HGETEXT_LEFT, "\x8");
    }
    fnt->Render(rect.x1 + 1.0f, rect.y1 + 1.0f, HGETEXT_LEFT, title);
}

void hgeGUIEditableLabel::Update(float dt) {
    if (isEdited) {
        char c = hge->Input_GetChar();
        if (c) {
            time = 0.0f;
            if (c == 8) {
                removeChar();
            }
            else if (c == 13) {
                isEdited = false;
            }
            else {
                insertChar(c);
            }
        }
        time += dt;
        if (time > 1.0f) {
            time -= 1.0f;
        }
    }
}

void hgeGUIEditableLabel::Enter() {
    isFocused = true;
}

void hgeGUIEditableLabel::Leave() {
    isFocused = false;
    isEdited = false;
}

void hgeGUIEditableLabel::Focus(bool bFocused) {
    isFocused = bFocused;
}

void hgeGUIEditableLabel::MouseOver(bool bOver) {
    if (bOver) {
        gui->SetFocus(id);
    }
    else {
        gui->SetFocus(0);
    }
}

bool hgeGUIEditableLabel::MouseLButton(bool bDown) {
    if (!bDown) {
        isEdited = true;
        return true;
    }
    else {
        return false;
    }
}

bool hgeGUIEditableLabel::KeyClick(int key, int chr) {
    if(key == HGEK_ENTER) {
        isEdited = !isEdited;
    }

    return false;
}

char* hgeGUIEditableLabel::getTitle() {
    return title;
}
void hgeGUIEditableLabel::setTitle(const char* _title) {
    if (title)
        delete title;
    title = copyString(_title);
    titleLen = strlen(title);
}
int hgeGUIEditableLabel::getTitleLen() {
    return titleLen;
}

void hgeGUIEditableLabel::Hide()
{
	bVisible = false;
	bEnabled = false;
}
void hgeGUIEditableLabel::Show()
{
	bVisible = true;
	bEnabled = true;
}

void hgeGUIEditableLabel::insertChar(char c) {
    title[titleLen] = c;
    titleLen++;
    title[titleLen] = '\0';
    if (onChange) {
        onChange(this);
    }
}

void hgeGUIEditableLabel::removeChar() {
    if (titleLen <= 0)
        return;
    titleLen--;
    title[titleLen] = '\0';
    if (onChange) {
        onChange(this);
    }
}

void hgeGUIEditableLabel::setOnChange(guiCallback func) {
    onChange = func;
}
