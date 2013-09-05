#ifndef GAME_H
#define GAME_H

#include <SoDlib.h>

class Game
{
	public:
		Game(HGE * hge);
		virtual ~Game();

		bool preload();
		void loop();

		bool loadAnimationNames(char* fn);
		int getAnimationsCount();
		char* getAnimationName(int index);

		bool update();
		bool update(bool physics);
		void updateWorld(float dt);
		void updateGui(float dt);
		bool updateControls();
		bool draw();
		void startDraw();
		void endDraw();
		void drawGame();

		void drawLine(float x1, float y1, float x2, float y2, DWORD color);
		void drawLine(b2Vec2 p1, b2Vec2 p2, DWORD color);
		void drawRect(float left, float top, float right, float bottom, DWORD color, DWORD bgcolor);
		void drawRect(float x, float y, float hw, float hh, float angle, DWORD color, DWORD bgcolor);
		void drawArc(float x, float y, float r, float start, float end, DWORD color, DWORD bgcolor);
		void drawCircle(float x, float y, float r, DWORD color);
		void drawCircle(float x, float y, float r, DWORD color, DWORD bgcolor);
		void drawPoly(b2PolygonShape* poly, b2Transform transform, b2Vec2 origin, float scale, DWORD color, DWORD bgcolor);

		void 	DrawPolygon (const b2Vec2 *vertices, int32 vertexCount, DWORD color, DWORD bgcolor);
		void 	DrawPolygonScreen (const b2Vec2 *vertices, int32 vertexCount, DWORD color, DWORD bgcolor);
		void 	DrawCircle (const b2Vec2 &center, float32 radius, DWORD color, DWORD bgcolor);
		void 	DrawSegment (const b2Vec2 &p1, const b2Vec2 &p2, DWORD color);
		void 	DrawTransform (const b2Transform &xf);

		hgeAnimation* loadAnimation(char * fn);
		Character* loadPlayerCharacter(char * fn, b2Vec2 origin);
		Character* loadNonPlayerCharacter(char * fn, b2Vec2 origin);
		b2Fixture* loadFixture(b2Body* body, TiXmlElement* elem);
		void addGroundLine(GroundLine* newGL); void addMapAnimation(MapAnimation* newMA);
		void loadMap(char* fn);

		int getCharactersCount();
		Character* getCharacter(int index);

		int getGroundLinesCount();
		GroundLine* getGroundLine(int index);

		HGE* getHge();
		b2World* getWorld();

		hgeGUI* getGUI();

		int getScreenWidth(), getScreenHeight();
		float getWorldScreenWidth(), getWorldScreenHeight();
		float getMapWidth(), getMapHeight();

		float worldX(float screenX);
		float worldY(float screenY);
        float screenX(float worldX);
		float screenY(float worldY);
		b2Vec2 screenPos(b2Vec2 worldPos);

		float getScaleFactor(), getFullScale();
		float getPixelsPerMeter();

		void setScale(float scale);
		void moveCamera(b2Vec2 diff);
		void setCamera(b2Vec2 pos);
		void moveScreen(b2Vec2 diff);
		b2Vec2 getCameraPos();
		float getWorldMouseX(), getWorldMouseY();

		b2Vec2 getGravity();
		float getTimeStep();

	protected:
		b2World* world;
		HGE* hge;
		hgeGUI* gui;

		HTEXTURE   cursorTex;
		hgeSprite*  cursorSprite;
		hgeQuad whiteQuad;
		hgeTriple whiteTriple;

		GroundLine** groundLines; int groundLinesCount;
		MapAnimation** mapAnimations; int mapAnimationsCount;
		char** animationNames;
		int animationsCount;
		Character** characters;
		int charactersCount;

		float32 timeStep;
		int32 velocityIterations;
		int32 positionIterations;

		int screenWidth, screenHeight;
		float mapWidth, mapHeight;

		b2Vec2 cameraPos;
		float mouseX, mouseY;
		float worldMouseX, worldMouseY;
		float dt;

		bool schematicDrawMode;
		DebugDraw* debugDraw;

		float pixelsPerMeter, scaleFactor;
		b2Vec2 gravity;
	private:
};

b2Vec2 intersection(b2Vec2 p1, b2Vec2 p2, b2Vec2 p3, b2Vec2 p4) ;
#endif // GAME_H
