#include "hgeGUIMenuItem.h"

hgeGUIMenuItem::hgeGUIMenuItem(int _id, hgeFont *_fnt,
    float _x, float _y,
    char *_title, guiCallback _clickFunc
) {
    id = _id;
    fnt = _fnt;
    title = _title;
    isFocused = false;
    isPressed = false;

    bStatic=false; bVisible=true; bEnabled=true;

    float w=fnt->GetStringWidth(title);
    rect.Set(std::floor(_x - w / 2), _y, std::ceil(_x + w / 2), _y + fnt->GetHeight());

    clickFunc = _clickFunc;

    selected = false;
}


void hgeGUIMenuItem::Render() {
    DWORD color = 0xFF000000;
    if (selected) {
        color = 0xFFFF0000;
    }
    if (isFocused) {
        color = 0xFF0000AA;
        if (isPressed) {
            color = 0xFF00AA00;
        }
    }

    fnt->SetColor(color);
    fnt->Render(rect.x1, rect.y1, HGETEXT_LEFT, title);

}

void hgeGUIMenuItem::Update(float dt) {

}

void hgeGUIMenuItem::Enter() {
    isFocused = true;
}

void hgeGUIMenuItem::Leave() {
    isFocused = false;
}

void hgeGUIMenuItem::Focus(bool bFocused) {
    isFocused = bFocused;
}

void hgeGUIMenuItem::MouseOver(bool bOver) {
    if (bOver) {
        gui->SetFocus(id);
    }
    else {
        gui->SetFocus(0);
    }
}

bool hgeGUIMenuItem::MouseLButton(bool bDown) {
    if (!bDown) {
        clickFunc(this);
        isPressed = false;
        return true;
    }
    else {
        isPressed = true;
        return false;
    }
}

bool hgeGUIMenuItem::KeyClick(int key, int chr) {
    if(key == HGEK_ENTER) {
        MouseLButton(true);
        return MouseLButton(false);
    }

    return false;
}

char* hgeGUIMenuItem::getTitle() {

    return title;
}
