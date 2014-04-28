#include <iostream>
#include <hge.h>
#include <hgesprite.h>
#include <hgefont.h>
#include <math.h>       /* cos */
#include <windows.h>

using namespace std;


HGE* hge = 0;

hgeFont*    font;

hgeQuad quads[6];

float time = 0;

DWORD bgcolor = 0xFF000000;
DWORD colors[6] = {
    0xFFFFFFFF,
    0xFFFFFF00,
    0xFFFF0000,
    0xFF000000,
    0xFF0000FF,
    0xFF00FFFF
};

bool FrameFunc() {
    if ( hge->Input_GetKeyState(HGEK_ESCAPE) ) return true;

    time += hge->Timer_GetDelta();

    if (time > 24.0f) {
        time = 0.0f;
    }

    return false;
}

bool RenderFunc() {
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(bgcolor);

	for (int i = 0; i < 6; i++) {
        float x = i * 400 + time * 200;
        while (x > 1000) {
            x -= 2400;
        }
        quads[i].v[0].x = x; quads[i].v[0].y = 600;
        quads[i].v[1].x = x + 100; quads[i].v[1].y = 0;
        quads[i].v[2].x = x + 500; quads[i].v[2].y = 0;
        quads[i].v[3].x = x + 400; quads[i].v[3].y = 600;
        hge->Gfx_RenderQuad(&quads[i]);
    }


	font->SetColor(0xFFFFFFFF);
	font->Render(30, 30, HGETEXT_LEFT, "Quick brown fox jumps over the lazy dog.\n—ъешь еще этих м€гких французских булок, да выпей чаю.\n0123456789+-=-!@#$%^&*()");

	hge->Gfx_EndScene();

	return false;
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR, int) {


	hge = hgeCreate(HGE_VERSION);


	hge->System_SetState(HGE_FRAMEFUNC,    FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC,   RenderFunc);
	hge->System_SetState(HGE_TITLE,        "font test");
	hge->System_SetState(HGE_WINDOWED,     true);
	hge->System_SetState(HGE_SCREENWIDTH,  800);
	hge->System_SetState(HGE_SCREENHEIGHT, 600);
	hge->System_SetState(HGE_USESOUND,     false);
	hge->System_SetState(HGE_HIDEMOUSE,    false);
	hge->System_SetState(HGE_FPS,          60);
	hge->System_SetState(HGE_SHOWSPLASH,   false);

	if ( hge->System_Initiate() ) {
        font = new hgeFont("font.fnt");

        for (int i = 0; i < 5; i++) {
            quads[i].v[0].col = colors[i];
            quads[i].v[1].col = colors[i];
            quads[i].v[2].col = colors[i + 1];
            quads[i].v[3].col = colors[i + 1];
        }
        quads[5].v[0].col = colors[5];
        quads[5].v[1].col = colors[5];
        quads[5].v[2].col = colors[0];
        quads[5].v[3].col = colors[0];

		hge->System_Start();
	}
	else {
		MessageBox(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}

	hge->System_Shutdown();
	hge->Release();

	return 0;
}
