#ifndef GAME_H
#define GAME_H

#include <SoDlib.h>

const int COUNTER_TOTAL = 0;
const int COUNTER_UPDATE = 1;
const int COUNTER_EFFECTS_UPDATE = 2;
const int COUNTER_EFFECTS_ACTIONS = 3;
const int COUNTER_EFFECTS_SPAWN = 4;
const int COUNTER_EFFECTS_EVAL = 5;
const int COUNTER_DRAW   = 6;
const int COUNTER_DRAW_GAME   = 7;
const int COUNTER_DRAW_GL   = 8;
const int COUNTER_DRAW_CHARACTERS   = 9;
const int COUNTER_DRAW_MAP   = 10;
const int COUNTER_DRAW_EFFECTS   = 11;
const int COUNTER_DRAW_PERFORMANCE_INFO   = 12;
const int COUNTER_DRAW_END   = 13;

const int COUNTERS_COUNT = 14;

static const char* COUNTER_NAMES[COUNTERS_COUNT] = {
    "total",
    " update",
    "  effects",
    "   actions",
    "    spawn",
    "   eval",
    " draw",
    "  game",
    "   gl",
    "   characters",
    "   map",
    "   effects",
    "   performance info (this text and graph)",
    "  end"
};

static const DWORD COUNTER_COLORS[COUNTERS_COUNT] = {
    0xFF0000FF,
    0xFF00FF00,
    0xFF00FFFF,
    0xFFFF0000,
    0xFFFF00FF,
    0xFFFFFF00,
    0xFFFFFFFF,
    0xFF0000AA,
    0xFF00AA00,
    0xFF00AAAA,
    0xFFAA0000,
    0xFFAA00AA,
    0xFFAAAA00,
    0xFFAAAAAA
};

const int SETTING_MAX_EFFECTS = 20000;

class Game
{
	public:
		Game(HGE * hge);
		virtual ~Game();

		bool preload();
		void loop();

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
		void drawText(float x, float y, char* text);

		void 	DrawPolygon (const b2Vec2 *vertices, int32 vertexCount, DWORD color, DWORD bgcolor);
		void 	DrawPolygonScreen (const b2Vec2 *vertices, int32 vertexCount, DWORD color, DWORD bgcolor);
		void 	DrawCircle (const b2Vec2 &center, float32 radius, DWORD color, DWORD bgcolor);
		void 	DrawSegment (const b2Vec2 &p1, const b2Vec2 &p2, DWORD color);
		void 	DrawTransform (const b2Transform &xf);

		hgeAnimation* loadAnimation(char * fn); hgeAnimation* cloneAnimation(hgeAnimation* source);
		HTEXTURE      loadTexture(char* fn);
		Character*    loadPlayerCharacter(char * fn, b2Vec2 origin);
		Character*    loadNonPlayerCharacter(char * fn, b2Vec2 origin);
		b2Fixture*    loadFixture(b2Body* body, TiXmlElement* elem);

        void loadMap(char* fn);
		void addGroundLine(GroundLine* newGL); void addMapAnimation(MapAnimation* newMA); void addPlatform(Platform* newPlatform);

		void removeEffect(int index);


		bool loadEffectPrototypes(char* fn); int getEffectPrototypesCount(); EffectPrototype* getEffectPrototype(int index);
		void addEffectPrototype(), removeEffectPrototype(int index);

        bool loadConditionPrototypes(char* fn); int getConditionPrototypesCount(); ConditionPrototype* getConditionPrototype(int index);
        void addConditionPrototype(), removeConditionPrototype(int index);

		bool loadCharacterParamPrototypes(char* fn); int getCharacterParamPrototypesCount(); CharacterParamPrototype* getCharacterParamPrototype(int index);
        void addCharacterParamPrototype(), removeCharacterParamPrototype(int index);

        bool loadCharacterResourcePrototypes(char* fn); int getCharacterResourcePrototypesCount(); CharacterResourcePrototype* getCharacterResourcePrototype(int index);
        void addCharacterResourcePrototype(), removeCharacterResourcePrototype(int index);

        bool loadCharacterStatusPrototypes(char* fn); int getCharacterStatusPrototypesCount(); CharacterStatusPrototype* getCharacterStatusPrototype(int index);
        void addCharacterStatusPrototype(), removeCharacterStatusPrototype(int index);

        bool loadCharacterMoveTypes(char* fn); int getCharacterMoveTypesCount(); CharacterMoveType* getCharacterMoveType(int index);
        void addCharacterMoveType(), removeCharacterMoveType(int index);

		void addEffect(Effect* newEffect);

		int getCharactersCount();
		Character* getCharacter(int index);

		int getGroundLinesCount();
		GroundLine* getGroundLine(int index);

		int getMapAnimationsCount();
		MapAnimation* getMapAnimation(int index);

		HGE* getHge();
		b2World* getWorld();

		hgeGUI* getGUI();

		int getScreenWidth(), getScreenHeight();
		float getViewportWidth(), getViewportHeight(), getHalfViewportWidth(), getHalfViewportHeight();
		float getMapWidth(), getMapHeight(), getHalfMapWidth(), getHalfMapHeight();

		float worldX(float screenX); float worldX(float screenX, float ratio);
		float worldY(float screenY); float worldY(float screenY, float ratio);
        float screenX(float worldX); float screenX(float worldX, float ratio);
		float screenY(float worldY); float screenY(float worldY, float ratio);
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

		void updateCounter(int ident, float value);

		static Game* getInstance();
	protected:
	    static Game* instance;

		b2World* world;
		HGE* hge;
		hgeGUI* gui;

		HTEXTURE   cursorTex;
		hgeSprite*  cursorSprite;
		hgeQuad whiteQuad;
		hgeTriple whiteTriple;
		hgeFont* defaultFont;

		GroundLine** groundLines; int groundLinesCount;
		MapAnimation** mapAnimations; int mapAnimationsCount;
		int backLayersCount, frontLayersCount; float* backLayerRatios; float* frontLayerRatios;
		int** backLayersMapAnimations; int** frontLayersMapAnimations; int* backLayersMapAnimationsCounts; int* frontLayersMapAnimationsCounts;

		Platform** platforms; int platformsCount;
		Character** characters; int charactersCount;
		Effect** effects; int effectsCount;
		EffectPrototype** effectPrototypes; int effectPrototypesCount;
		ConditionPrototype** conditionPrototypes; int conditionPrototypesCount;
		CharacterParamPrototype** characterParamPrototypes; int characterParamPrototypesCount;
		CharacterResourcePrototype** characterResourcePrototypes; int characterResourcePrototypesCount;
		CharacterStatusPrototype** characterStatusPrototypes; int characterStatusPrototypesCount;
		CharacterMoveType** characterMoveTypes; int characterMoveTypesCount;

		hgeAnimation** animations; char** animationNames; int animationsCount;
		HTEXTURE* textures; char** textureNames; int texturesCount;

		float32 timeStep;
		int32 velocityIterations;
		int32 positionIterations;

		int screenWidth, screenHeight; float viewportWidth, viewportHeight, halfViewportWidth, halfViewportHeight;
		float mapWidth, mapHeight, halfMapWidth, halfMapHeight;

		b2Vec2 cameraPos;
		float mouseX, mouseY;
		float worldMouseX, worldMouseY;
		float dt;

		bool schematicDrawMode;
		int drawPerformanceInfo;
		DebugDraw* debugDraw;

		float pixelsPerMeter, scaleFactor;
		b2Vec2 gravity;

		float* counters; float* displayedCounters; float timer;
	private:
};

b2Vec2 intersection(b2Vec2 p1, b2Vec2 p2, b2Vec2 p3, b2Vec2 p4) ;
float distanceToSegment(float x1, float y1, float x2, float y2, float pointX, float pointY);
float frand(float from, float to);

char* copyString(const char* original);
bool compareStrings(char* first, char* second);
int countSpaces(const char* str);
char* getFileName(const char* path);
#endif // GAME_H

