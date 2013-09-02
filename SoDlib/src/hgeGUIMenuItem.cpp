#include "hgeGUIMenuItem.h"

hgeGUIMenuItem::hgeGUIMenuItem(int _id, hgeFont *_fnt,
                float _x, float _y,
                float _delay, char *_title, menuCallback _clickFunc)
{
  id = _id;
  fnt = _fnt;
  delay = _delay;
  title = _title;


  color.SetHWColor(0xFFFFE060);
  shadow.SetHWColor(0x30000000);
  offset = 0.0f; timer = -1.0f; timer2 = -1.0f;

  bStatic=false; bVisible=true; bEnabled=true;

  float w=fnt->GetStringWidth(title);
  rect.Set(_x - w / 2, _y, _x + w / 2, _y + fnt->GetHeight());

  clickFunc = _clickFunc;
}


void hgeGUIMenuItem::Render()
{
  fnt->SetColor(shadow.GetHWColor());
  fnt->Render(rect.x1 + offset + 3, rect.y1 + 3, HGETEXT_LEFT, title);
  fnt->SetColor(color.GetHWColor());
  fnt->Render(rect.x1 - offset, rect.y1 - offset, HGETEXT_LEFT, title);
}

void hgeGUIMenuItem::Update(float dt)
{
  if(timer2 != -1.0f)
  {
    timer2 += dt;
    if(timer2 >= delay + 0.1f)
    {
      color = scolor2 + dcolor2;
      shadow = sshadow + dshadow;
      offset = 0.0f;
      timer2 = -1.0f;
    }
    else
    {
      if(timer2 < delay) { color=scolor2; shadow=sshadow; }
      else {
        color=scolor2+dcolor2*(timer2-delay)*10;
        shadow=sshadow+dshadow*(timer2-delay)*10;
      }
    }
  }
  else if(timer != -1.0f)
  {
    timer+=dt;
    if(timer >= 0.2f)
    {
      color = scolor + dcolor;
      offset = soffset + doffset;
      timer = -1.0f;
    }
    else
    {
      color=scolor+dcolor*timer*5;
      offset=soffset+doffset*timer*5;
    }
  }
}

void hgeGUIMenuItem::Enter()
{
  hgeColor tcolor2;

  scolor2.SetHWColor(0x00FFE060);
  tcolor2.SetHWColor(0xFFFFE060);
  dcolor2 = tcolor2 - scolor2;

  sshadow.SetHWColor(0x00000000);
  tcolor2.SetHWColor(0x30000000);
  dshadow = tcolor2 - sshadow;

  timer2 = 0.0f;
}

void hgeGUIMenuItem::Leave()
{
  hgeColor tcolor2;

  scolor2.SetHWColor(0xFFFFE060);
  tcolor2.SetHWColor(0x00FFE060);
  dcolor2 = tcolor2 - scolor2;

  sshadow.SetHWColor(0x30000000);
  tcolor2.SetHWColor(0x00000000);
  dshadow = tcolor2 - sshadow;

  timer2 = 0.0f;
}

bool hgeGUIMenuItem::IsDone()
{
  if(timer2 == -1.0f) return true;
  else return false;
}

void hgeGUIMenuItem::Focus(bool bFocused)
{
  hgeColor tcolor;

  if(bFocused)
  {
    scolor.SetHWColor(0xFFFFE060);
    tcolor.SetHWColor(0xFFFFFFFF);
    soffset = 0;
    doffset = 1;
  } else {
    scolor.SetHWColor(0xFFFFFFFF);
    tcolor.SetHWColor(0xFFFFE060);
    soffset = 1;
    doffset = -1;
  }

  dcolor = tcolor - scolor;
  timer = 0.0f;
}

void hgeGUIMenuItem::MouseOver(bool bOver)
{
  if(bOver) gui->SetFocus(id);
}

bool hgeGUIMenuItem::MouseLButton(bool bDown)
{
  if (!bDown) {
  	clickFunc(this);
    offset = 1;
    return true;
  } else {
    offset = 0;
    return false;
  }
}

bool hgeGUIMenuItem::KeyClick(int key, int chr)
{
  if(key==HGEK_ENTER)
  {
    MouseLButton(true);
    return MouseLButton(false);
  }

  return false;
}

char* hgeGUIMenuItem::getTitle()
{

    return title;
}
