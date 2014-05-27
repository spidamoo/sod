#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include <SoDlib.h>
#include <list>

class GUIWindow : public hgeGUIObject
{
	public:
		GUIWindow(Game* game, int id, float x, float y, float w, float h);
		virtual ~GUIWindow();

		void AddCtrl(hgeGUIObject* ctrl);
		void DelCtrl(int index);
		void Render();

		void Hide();
		void Show();
		void Move(float x, float y);
	protected:
		std::list<hgeGUIObject*> controls;

		float x, y, w, h;
		Game* game;
	private:
};

#endif // GUIWINDOW_H
