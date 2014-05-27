#include "CharacterActionEffect.h"



CharacterActionEffect::CharacterActionEffect(float type, float param) {
	this->type = type;
	this->param = param;
	this->param2 = 0;
}

CharacterActionEffect::CharacterActionEffect(TiXmlElement* elem) {
	printf("effect\n");

	type = atof(elem->Attribute("type"));
	param = atof(elem->Attribute("param"));
	if (elem->Attribute("param2")) param2 = atof(elem->Attribute("param2"));
}

CharacterActionEffect::~CharacterActionEffect() {
	//dtor
}

void CharacterActionEffect::save(TiXmlElement* elem) {
	elem->SetAttribute("type", type);
	elem->SetDoubleAttribute("param", param);
	elem->SetDoubleAttribute("param2", param2);
}

void CharacterActionEffect::perform(Game* game, Character* character) {
	switch (type) {
		case ACTIONEFFECT_TYPE_TURN:
			character->turn();
			break;
		case ACTIONEFFECT_TYPE_CHANGEMOVE:
			character->setMove(param);
			break;
		case ACTIONEFFECT_TYPE_RUN:
			character->run(param);
			break;
		case ACTIONEFFECT_TYPE_JUMP:
			character->jump(b2Vec2(param2, -param));
			break;
        case ACTIONEFFECT_TYPE_SPAWN_EFFECT:
            game->addEffect( game->getEffectPrototype(param)->spawnEffect(character) );
            //printf("effect spawn\n");
            break;
	}
}

int CharacterActionEffect::getType() {
    return type;
}
void CharacterActionEffect::setType(int _type) {
    type = _type;
}

float CharacterActionEffect::getParam(int index) {
    if (index == 2)
        return param2;
    return param;
}
void CharacterActionEffect::setParam(float _param, int index) {
    if (index == 2)
        param2 = _param;
    else
        param = _param;
}
