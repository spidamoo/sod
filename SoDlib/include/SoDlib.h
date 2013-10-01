#ifndef SODLIB_H_INCLUDED
#define SODLIB_H_INCLUDED

class Construction;
class CharacterActionCause;
class CharacterActionEffect;
class CharacterAction;
class CharacterHotSpot;
class Character;
class PlayerCharacter;
class NonPlayerCharacter;
class GroundLine;
class Platform;
class MapAnimation;
class DebugDraw;
class Game;
class hgeGUIMenuItem;

typedef bool (*menuCallback)(hgeGUIMenuItem* sender);

#include <hge.h>
#include <hgeanim.h>
#include <hgegui.h>
#include <hgeguictrls.h>
#include <hgecolor.h>

#include <Box2D/Box2D.h>

#include <tinyxml.h>

#include <Exception.h>
#include <Game.h>
#include <Construction.h>
#include <EffectPrototype.h>
#include <HotSpotEffectPrototype.h>
#include <Effect.h>
#include <RectangularEffect.h>
#include <HotSpotEffect.h>
#include <CharacterActionCause.h>
#include <CharacterActionEffect.h>
#include <CharacterAction.h>
#include <CharacterHotSpot.h>
#include <Character.h>
#include <NonPlayerCharacter.h>
#include <PlayerCharacter.h>
#include <GroundLine.h>
#include <Platform.h>
#include <MapAnimation.h>
#include <DebugDraw.h>

#include <GUIWindow.h>
#include <hgeGUIMenuItem.h>

#endif // SODLIB_H_INCLUDED
