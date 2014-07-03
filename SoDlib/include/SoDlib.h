#ifndef SODLIB_H_INCLUDED
#define SODLIB_H_INCLUDED

class Construction;
class CharacterActionCause;
class CharacterActionEffect;
class CharacterAction;
class CharacterHotSpot;
class AiPattern;
class Character;
class PlayerCharacter;
class NonPlayerCharacter;
class GroundLine;
class Platform;
class MapAnimation;
class DebugDraw;
class ConditionPrototype;
class Condition;
class EffectAction;
class EffectPrototype;
class Effect;
class CharacterParamPrototype;
class CharacterResourcePrototype;
class CharacterStatusPrototype;
class CharacterMoveType;
class CharacterParam;
class CharacterResource;
class Game;
class hgeGUIObject;
class hgeGUIMenuItem;
class hgeGUIEditableLabel;

typedef bool (*guiCallback)(hgeGUIObject* sender);

#include <string>
#include <time.h>

#include <hge.h>
#include <hgeanim.h>
#include <hgegui.h>
#include <hgeguictrls.h>
#include <hgecolor.h>

#include <Box2D/Box2D.h>

#include <tinyxml.h>

#include <muParser.h>

#include <Exception.h>
#include <Game.h>
//#include <Construction.h>
#include <CharacterParamPrototype.h>
#include <CharacterResourcePrototype.h>
#include <CharacterStatusPrototype.h>
#include <CharacterMoveType.h>
#include <CharacterParam.h>
#include <CharacterResource.h>
#include <ConditionPrototype.h>
#include <Condition.h>
#include <EffectAction.h>
#include <EffectPrototype.h>
#include <Effect.h>
#include <CharacterActionCause.h>
#include <CharacterActionEffect.h>
#include <CharacterAction.h>
#include <CharacterHotSpot.h>
#include <AiPattern.h>
#include <Character.h>
#include <NonPlayerCharacter.h>
#include <PlayerCharacter.h>
#include <GroundLine.h>
#include <Platform.h>
#include <MapAnimation.h>
#include <DebugDraw.h>

#include <GUIWindow.h>
#include <hgeGUIMenuItem.h>
#include <hgeGUIEditableLabel.h>

#endif // SODLIB_H_INCLUDED
