#ifndef HGEGUIMENUITEM_H
#define HGEGUIMENUITEM_H

#include <SoDlib.h>

class hgeGUIMenuItem : public hgeGUIObject
{
public:
  hgeGUIMenuItem(int id, hgeFont *fnt,
          float x, float y, float delay, char *title, menuCallback clickFunc);

  virtual void  Render();
  virtual void  Update(float dt);

  virtual void  Enter();
  virtual void  Leave();
  virtual bool  IsDone();
  virtual void  Focus(bool bFocused);
  virtual void  MouseOver(bool bOver);

  virtual bool  MouseLButton(bool bDown);
  virtual bool  KeyClick(int key, int chr);

  char* getTitle();

private:
  hgeFont       *fnt;
  float         delay;
  char          *title;

  hgeColor      scolor, dcolor, scolor2, dcolor2, color;
  hgeColor      sshadow, dshadow, shadow;
  float         soffset, doffset, offset;
  float         timer, timer2;

  menuCallback  clickFunc;
};
#endif // HGEGUIMENUITEM_H
