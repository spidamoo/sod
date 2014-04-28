#ifndef HGEGUIEDITABLELABEL_H
#define HGEGUIEDITABLELABEL_H

#include <SoDlib.h>


class hgeGUIEditableLabel : public hgeGUIObject {
    public:
        hgeGUIEditableLabel(Game* game, int id, hgeFont *fnt,
              float x, float y, float w, float h, char *title);

        virtual void  Render();
        virtual void  Update(float dt);

        virtual void  Enter();
        virtual void  Leave();
        virtual void  Focus(bool bFocused);
        virtual void  MouseOver(bool bOver);

        virtual bool  MouseLButton(bool bDown);
        virtual bool  KeyClick(int key, int chr);

        char* getTitle();
        void setTitle(const char* title);
        int getTitleLen();

        void Hide();
        void Show();

        void setOnChange(guiCallback func);

    private:
        void insertChar(char c);
        void removeChar();

        hgeFont       *fnt;
        float         time;
        char          *title;
        int titleLen;

        bool isFocused;
        bool isEdited;

        guiCallback  onChange;

        Game* game;
};

#endif // HGEGUIEDITABLELABEL_H
