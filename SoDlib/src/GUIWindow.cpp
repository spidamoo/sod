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

	game->drawRect(x, y, x + w, y + h, 0xFF333333, 0xFFBBBBBB);
//	std::list<hgeGUIObject*>::iterator it;
//	for (it = controls.begin() ; it != controls.end(); it++ ) {
//		(*it)->Render();
//	}
}

void GUIWindow::Hide()
{
	std::list<hgeGUIObject*>::iterator it;
	for (it = controls.begin() ; it != controls.end(); it++ ) {
		(*it)->bVisible = false;
		(*it)->bEnabled = false;
	}
	bVisible = false;
	bEnabled = false;
}
void GUIWindow::Show()
{
	std::list<hgeGUIObject*>::iterator it;
	for (it = controls.begin() ; it != controls.end(); it++ ) {
		(*it)->bVisible = true;
		(*it)->bEnabled = true;
	}
	bVisible = true;
	bEnabled = true;
}

void GUIWindow::Move(float x, float y)
{
	std::list<hgeGUIObject*>::iterator it;
	for (it = controls.begin() ; it != controls.end(); it++ ) {
		hgeRect oldRect = (*it)->rect;
		// (*it)->rect.Set(oldRect.x1 - rect.x1 + x, oldRect.y1 - rect.y1 + y, oldRect.x2 - rect.x2 + x, oldRect.y2 - rect.y2 + y);
		(*it)->rect.Set((oldRect.x1 - rect.x1) + x, (oldRect.y1 - rect.y1) + y, (oldRect.x2 - oldRect.x1) + x, (oldRect.y2 - oldRect.y1) + y);
	}
	this->x = x;
	this->y = y;
	rect.Set(x, y, x + w, y + h);
}

