#ifndef HGEGUIMENUITEM_H
#define HGEGUIMENUITEM_H

#include <SoDlib.h>

class hgeGUIMenuItem : public hgeGUIObject
{
    public:
        hgeGUIMenuItem(int id, hgeFont *fnt, float x, float y, char *title, guiCallback clickFunc);

        virtual void  Render();
        virtual void  Update(float dt);

        virtual void  Enter();
        virtual void  Leave();
        virtual void  Focus(bool bFocused);
        virtual void  MouseOver(bool bOver);

        virtual bool  MouseLButton(bool bDown);
        virtual bool  KeyClick(int key, int chr);

        char* getTitle();

    private:
        hgeFont       *fnt;
        char          *title;

        bool isFocused;
        bool isPressed;

        guiCallback  clickFunc;
};
#endif // HGEGUIMENUITEM_H
