#include "CharacterActionCause.h"

CharacterActionCause::CharacterActionCause(int type, float param)
{
	this->type = type;
	this->param = param;
}

CharacterActionCause::CharacterActionCause(TiXmlElement* elem)
{
	printf("cause\n");
	type = atoi(elem->Attribute("type"));
	param = atof(elem->Attribute("param"));
}

CharacterActionCause::~CharacterActionCause()
{
	//dtor
}

void CharacterActionCause::save(TiXmlElement* elem)
{
	elem->SetAttribute("type", type);
	elem->SetDoubleAttribute("param", param);
}

bool CharacterActionCause::test(Game* game, Character* character) {
	switch (type) {
		case ACTIONCAUSE_TYPE_KEYHIT:
			return ((PlayerCharacter*)character)->isKeyHit(param);
			break;
		case ACTIONCAUSE_TYPE_KEYRELEASED:
			return !((PlayerCharacter*)character)->isKeyDown(param);
			break;
		case ACTIONCAUSE_TYPE_KEYDOWN:
			return ((PlayerCharacter*)character)->isKeyDown(param);
			break;
		case ACTIONCAUSE_TYPE_NPC_TARGETSIDE:
			return ((NonPlayerCharacter*)character)->getTargetSide() == param;
			break;
		case ACTIONCAUSE_TYPE_NPC_TARGETCLOSER:
			return ((NonPlayerCharacter*)character)->getTargetDistance() < param;
			break;
		case ACTIONCAUSE_TYPE_NPC_TARGETFARTHER:
			return (((NonPlayerCharacter*)character)->getTargetDistance() > param);
			break;
		case ACTIONCAUSE_TYPE_ONGROUND:
			if (param == 0) {
				return (character->getOnGround() > -1);
			} else {
				return (character->getOnGround() == -1);
			}
			break;
        case ACTIONCAUSE_TYPE_ANIM_TIME_PASSED:
            return ( character->getAnimTime() >= param );
            break;
        case ACTIONCAUSE_TYPE_ANIM_TIME_IS:
            return ( character->getAnimTime() == param );
            break;
	}
}

int CharacterActionCause::getType() {
    return type;
}
float CharacterActionCause::getParam() {
    return param;
}
void CharacterActionCause::setType(int _type) {
    type = _type;
}
void CharacterActionCause::setParam(float _param) {
    param = _param;
}
