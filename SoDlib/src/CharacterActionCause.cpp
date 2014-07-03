#include "CharacterActionCause.h"

CharacterActionCause::CharacterActionCause(int _type, float _param)
{
	type = _type;
	param = _param;
	param2 = 0.0f;
}

CharacterActionCause::CharacterActionCause(TiXmlElement* elem)
{
	printf("cause\n");
	type = atoi(elem->Attribute("type"));
	param = atof(elem->Attribute("param"));
	param2 = atof(elem->Attribute("param2"));
}

CharacterActionCause::~CharacterActionCause()
{
	//dtor
}

void CharacterActionCause::save(TiXmlElement* elem)
{
	elem->SetAttribute("type", type);
	elem->SetDoubleAttribute("param", param);
	elem->SetDoubleAttribute("param2", param2);
}

bool CharacterActionCause::test(Game* game, Character* character) {
	switch (type) {
		case ACTIONCAUSE_TYPE_KEYHIT:
			return ((PlayerCharacter*)character)->wasKeyHit(param);
			break;
		case ACTIONCAUSE_TYPE_KEYRELEASED:
			return !((PlayerCharacter*)character)->isKeyDown(param);
			break;
		case ACTIONCAUSE_TYPE_KEYDOWN:
			return ((PlayerCharacter*)character)->isKeyDown(param);
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
        case ACTIONCAUSE_TYPE_ANIM_TIME_LESS:
            return ( character->getAnimTime() <= param );
            break;
        case ACTIONCAUSE_TYPE_ANIM_TIME_IS:
            if (!used && character->getAnimTime() >= param) {
                used = true;
                return true;
            }
            return false;
            break;
        case ACTIONCAUSE_TYPE_ANGLE_MORE:
            return ( character->getAngle() > param );
            break;
        case ACTIONCAUSE_TYPE_ANGLE_LESS:
            return ( character->getAngle() < param );
            break;
	}
	return false;
}

int CharacterActionCause::getType() {
    return type;
}
float CharacterActionCause::getParam() {
    return param;
}
float CharacterActionCause::getParam(int index) {
    switch (index) {
        case 2:
            return param2;
        default:
            return param;
    }
}
void CharacterActionCause::setType(int _type) {
    type = _type;
}
void CharacterActionCause::setParam(float _param) {
    param = _param;
}
void CharacterActionCause::setParam(int index, float _param) {
    switch (index) {
        case 2:
            param2 = _param;
            break;
        default:
            param = _param;
            break;
    }
}

void CharacterActionCause::setUsed(bool _used) {
    used = _used;
}
