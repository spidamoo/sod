#include "CharacterActionEffect.h"



CharacterActionEffect::CharacterActionEffect(float type, float param)
{
	this->type = type;
	this->param = param;
	this->param2 = 0;
}

CharacterActionEffect::CharacterActionEffect(TiXmlElement* elem)
{
	printf("effect\n");

	type = atof(elem->Attribute("type"));
	param = atof(elem->Attribute("param"));
	if (elem->Attribute("param2")) param2 = atof(elem->Attribute("param2"));
}

CharacterActionEffect::~CharacterActionEffect()
{
	//dtor
}

void CharacterActionEffect::save(TiXmlElement* elem)
{
	elem->SetAttribute("type", type);
	elem->SetDoubleAttribute("param", param);
	elem->SetDoubleAttribute("param2", param2);
}

void CharacterActionEffect::perform(Game* game, Character* character)
{
	switch (type) {
		case ACTIONEFFECT_TYPE_TURN:
			character->turn();
			break;
		case ACTIONEFFECT_TYPE_CHANGEANIM:
			character->setAnim(param);
			break;
		case ACTIONEFFECT_TYPE_RUN:
			character->run(param);
			break;
		case ACTIONEFFECT_TYPE_JUMP:
			character->jump(b2Vec2(param2, -param));
			break;
	}
}
