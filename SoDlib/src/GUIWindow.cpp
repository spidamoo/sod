#include "GUIWindow.h"

GUIWindow::GUIWindow(Game* _game, int _id, float _x, float _y, float _w, float _h)
{
	x = _x;
	y = _y;
	w = _w;
	h = _h;
	game = _game;
	gui = _game->getGUI();

	id = _id;
	rect.Set(x, y, x + w, y + h);
	//printf("%f %f %f %f\n", x, y, w, h);

	bStatic=false; bVisible=true; bEnabled=true;
}

GUIWindow::~GUIWindow()
{
	//dtor
}

void GUIWindow::AddCtrl(hgeGUIObject* ctrl)
{
	controls.push_back(ctrl);
	hgeRect oldRect = ctrl->rect;
	ctrl->rect.Set(oldRect.x1 + x, oldRect.y1 + y, oldRect.x2 + x, oldRect.y2 + y);
	gui->AddCtrl(ctrl);
}


void GUIWindow::Render()
{

	game->drawRect(x, y, x + w, y + h, 0xFF333333, 0xFFDDDDDD);
//	std::list<hgeGUIObject*>::iterator it;
//	for (it = controls.begin() ; it != controls.end(); it++ ) {
//		(*it)->Render();
//	}
}

void GUIWindow::Hide()
{
	std::list<hgeGUIObject*>::iterator it;
	for (it = controls.begin() ; it != controls.end(); it++ ) {
		(*it)->Hide();
	}
	bVisible = false;
	bEnabled = false;
}
void GUIWindow::Show()
{
	std::list<hgeGUIObject*>::iterator it;
	for (it = controls.begin() ; it != controls.end(); it++ ) {
		(*it)->Show();
	}
	bVisible = true;
	bEnabled = true;
}

void GUIWindow::Move(float x, float y)
{
	std::list<hgeGUIObject*>::iterator it;
	for (it = controls.begin() ; it != controls.end(); it++ ) {
        float ox = (*it)->rect.x1 - rect.x1;
        float oy = (*it)->rect.y1 - rect.y1;
        (*it)->Move(x + ox, y + oy);
	}
	this->x = x;
	this->y = y;
	rect.Set(x, y, x + w, y + h);
}

