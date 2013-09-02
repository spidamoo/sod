#include "CharacterAction.h"

CharacterAction::CharacterAction()
{
	//ctor
}

CharacterAction::CharacterAction(TiXmlElement* elem)
{
	TiXmlElement* causesRoot = elem->FirstChildElement("on");
	causesCount = atoi(causesRoot->Attribute("causes"));
	causes = new CharacterActionCause*[causesCount];
	int i = 0;
	TiXmlElement* causeElem = causesRoot->FirstChildElement("cause");
	while (causeElem) {
		causes[i] = new CharacterActionCause(causeElem);
		i++;
		causeElem = causeElem->NextSiblingElement("cause");
	}

	TiXmlElement* effectsRoot = elem->FirstChildElement("do");
	effectsCount = atoi(effectsRoot->Attribute("effects"));
	effects = new CharacterActionEffect*[effectsCount];
	i = 0;
	TiXmlElement* effectElem = effectsRoot->FirstChildElement("effect");
	while (effectElem) {
		effects[i] = new CharacterActionEffect(effectElem);
		i++;
		effectElem = effectElem->NextSiblingElement("effect");
	}
}

CharacterAction::~CharacterAction()
{
	//dtor
}

void CharacterAction::save(TiXmlElement* elem)
{
	TiXmlElement* causesRoot = new TiXmlElement( "on" );
	causesRoot->SetAttribute("causes", causesCount);
	for (int i = 0; i < causesCount; i++) {
		TiXmlElement* causeElem = new TiXmlElement( "cause" );
		causes[i]->save(causeElem);
		causesRoot->LinkEndChild( causeElem );
	}
	elem->LinkEndChild(causesRoot);

	TiXmlElement* effectsRoot = new TiXmlElement( "do" );
	effectsRoot->SetAttribute("effects", effectsCount);
	for (int i = 0; i < effectsCount; i++) {
		TiXmlElement* effectElem = new TiXmlElement( "effect" );
		effects[i]->save(effectElem);
		effectsRoot->LinkEndChild( effectElem );
	}
	elem->LinkEndChild(effectsRoot);
}

bool CharacterAction::take(Game* game, Character* character)
{
	bool fulfilled = true;
	for (int i = 0; i < causesCount; i++) {
		fulfilled &= causes[i]->test(game, character);
	}

	if (fulfilled) {
		for (int i = 0; i < effectsCount; i++) {
			effects[i]->perform(game, character);
		}
		return true;
	}
	return false;
}
