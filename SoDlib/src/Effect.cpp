#include "Effect.h"

Effect::Effect(Game* game, EffectPrototype* prototype) {
	this->game = game;
	this->prototype = prototype;

    if (prototype->getAnimationsCount() > 0) {
        setAnimation(
            game->loadAnimation(
                (char*)prototype->getAnimation(
                    game->getHge()->Random_Int(1, prototype->getAnimationsCount()) - 1
                )
            ),
            prototype->getBlendMode()
        );
    }
    else {
        animation = NULL;
    }

	actionTimes = new float[prototype->getActionsCount()];
	actionInteractions = new int*[prototype->getActionsCount()];
	for ( int i = 0; i < prototype->getActionsCount(); i++ ) {
        actionTimes[i] = -prototype->getAction(i)->getStartTime();
        switch ( prototype->getAction(i)->getType() ) {
            case EFFECTACTION_TYPE_INFLICT_CONDITION:
                actionInteractions[i] = new int[game->getCharactersCount()];
                for (int j = 0; j < game->getCharactersCount(); j++) {
                    actionInteractions[i][j] = 0;
                }
                break;
        }
    }

    owner = NULL;
}

Effect::~Effect() {
	delete animation;
	delete actionTimes;
}

void Effect::setOwner(Character* owner) {
    this->owner = owner;
}
Character* Effect::getOwner() {
    return owner;
}

void Effect::initialize() {
    time = prototype->evalStartExpression(EFFECT_FUNCTION_TIME);
    componentSpeed.x = prototype->evalStartExpression(EFFECT_FUNCTION_XSPEED);
    componentSpeed.y = prototype->evalStartExpression(EFFECT_FUNCTION_YSPEED);
    angle = prototype->evalStartExpression(EFFECT_FUNCTION_ANGLE);

    r = prototype->evalStartExpression(EFFECT_FUNCTION_R);
    g = prototype->evalStartExpression(EFFECT_FUNCTION_G);
    b = prototype->evalStartExpression(EFFECT_FUNCTION_B);
    a = prototype->evalStartExpression(EFFECT_FUNCTION_A);

    scale = prototype->evalStartExpression(EFFECT_FUNCTION_SCALE);
}

void Effect::draw(bool schematicMode)
{
    if (schematicMode) {
        game->drawRect( game->screenX(position.x), game->screenY(position.y), 3, 3, 0, 0xFFFFAA00, 0xAAFFAA00 );
    }
    else {
        if (animation) {
            animation->RenderEx( game->screenX(position.x), game->screenY(position.y), angle, scale, scale );
        }
    }
}

void Effect::update(float dt)
{
    clock_t st = clock();

    prototype->setParam(EFFECT_PARAM_X, position.x);
    prototype->setParam(EFFECT_PARAM_Y, position.y);
    prototype->setParam(EFFECT_PARAM_XSPEED, componentSpeed.x);
    prototype->setParam(EFFECT_PARAM_YSPEED, componentSpeed.y);
    prototype->setParam(EFFECT_PARAM_TIME, time);
    prototype->setParam(EFFECT_PARAM_ANGLE, angle);
    prototype->setParam(EFFECT_PARAM_SCALE, scale);
    prototype->setParam(EFFECT_PARAM_DT, dt);

    if (animation) {
        if ( prototype->getExpressionExists(EFFECT_FUNCTION_R) ) {
            r = prototype->evalExpression(EFFECT_FUNCTION_R);
        }
        if ( prototype->getExpressionExists(EFFECT_FUNCTION_G) ) {
            g = prototype->evalExpression(EFFECT_FUNCTION_G);
        }
        if ( prototype->getExpressionExists(EFFECT_FUNCTION_B) ) {
            b = prototype->evalExpression(EFFECT_FUNCTION_B);
        }
        if ( prototype->getExpressionExists(EFFECT_FUNCTION_A) ) {
            a = prototype->evalExpression(EFFECT_FUNCTION_A);
        }
        animation->SetColor( (a << 24) + (r << 16) + (g << 8) + b  );

        if ( prototype->getExpressionExists(EFFECT_FUNCTION_SCALE) ) {
            scale = prototype->evalExpression(EFFECT_FUNCTION_SCALE);
        }

        if ( prototype->getExpressionExists(EFFECT_FUNCTION_ANGLE) ) {
            angle = prototype->evalExpression(EFFECT_FUNCTION_ANGLE);
    //                printf("angle evaluated to %f \n", angle);
        }
    }

    switch ( prototype->getPositionType() ) {
        case EFFECT_POSITION_TYPE_STATIC:
            break;
        case EFFECT_POSITION_TYPE_DYNAMIC:
            if ( prototype->getExpressionExists(EFFECT_FUNCTION_SPEED) ) {
                speed = prototype->evalExpression(EFFECT_FUNCTION_SPEED);
//                printf("angle and speed evaluated to respectively %f and %f\n", angle, speed);
            }
            if ( prototype->getExpressionExists(EFFECT_FUNCTION_XSPEED) && prototype->getExpressionExists(EFFECT_FUNCTION_YSPEED) ) {
                componentSpeed.x = prototype->evalExpression(EFFECT_FUNCTION_XSPEED);
                componentSpeed.y = prototype->evalExpression(EFFECT_FUNCTION_YSPEED);
//                printf("x and y speed evaluated to respectively %f and %f\n", componentSpeed.x, componentSpeed.y);
            } else {
//                componentSpeed.x = speed * cos(angle);
//                componentSpeed.y = speed * sin(angle);
//                printf("x and y speed defaulted to respectively %f and %f\n", componentSpeed.x, componentSpeed.y);
            }
            if ( prototype->getExpressionExists(EFFECT_FUNCTION_X) && prototype->getExpressionExists(EFFECT_FUNCTION_Y) ) {
                position.x = prototype->evalExpression(EFFECT_FUNCTION_X);
                position.y = prototype->evalExpression(EFFECT_FUNCTION_Y);
//                printf("x and y evaluated to respectively %f and %f\n", position.x, position.y);
            } else {
                position.x += componentSpeed.x * dt;
                position.y += componentSpeed.y * dt;
//                printf("x and y defaulted to respectively %f and %f\n", position.x, position.y);
            }
            break;
        case EFFECT_POSITION_TYPE_HOTSPOT:
            if (owner) {
                position.x = owner->getHotSpotX(hotSpotIndex);
                position.y = owner->getHotSpotY(hotSpotIndex);
            }
            break;
    }

    clock_t et = clock();
    game->updateCounter(COUNTER_EFFECTS_EVAL, (float)(et - st));

    st = clock();
    for ( int i = 0; i < prototype->getActionsCount(); i++ ) {
        actionTimes[i] += dt;
        //printf("action %d time is %f\n", i, actionTimes[i]);
        EffectAction* action = prototype->getAction(i);
        while ( actionTimes[i] > action->getInterval() ) {
            //printf("time to rock\n");
            bool conditionSatisfied = false;
            switch ( action->getCondition() ) {
                case EFFECTACTION_CONDITION_NONE:
                    conditionSatisfied = true;
                    break;
            }

            if (conditionSatisfied) {
                switch ( action->getType() ) {
                    case EFFECTACTION_TYPE_DESTRUCT:
                        time = -1;
                        break;
                    case EFFECTACTION_TYPE_SPAWN_EFFECT:
                        //printf("effect spawns effect\n");
                        game->addEffect( game->getEffectPrototype( action->getParam() )->spawnEffect(this) );
                        break;
                    case EFFECTACTION_TYPE_INFLICT_CONDITION:
                        //printf("trying to apply condition\n");
                        for (int j = 0; j < game->getCharactersCount(); j++) {
                            if ( !owner ||
                                ( (action->getTargets() & EFFECTACTION_TARGET_FRIEND) && owner->getType() == game->getCharacter(j)->getType() )
                                || ( (action->getTargets() & EFFECTACTION_TARGET_ENEMY) && owner->getType() != game->getCharacter(j)->getType() )
                            ) {
                                //printf("character %d counts as target\n", i);
                                if ( characterCrosses( game->getCharacter(j) ) && actionInteractions[i][j] < action->getMaxInteractions() ) {
                                    game->getCharacter(j)->addCondition(
                                        game->getConditionPrototype( action->getParam() )->spawnCondition( owner, game->getCharacter(j) )
                                    );

                                    actionInteractions[i][j]++;
                                }
                            }
                        }
                        break;
                }
            }

            actionTimes[i] -= action->getInterval();
        }
    }

    et = clock();
    game->updateCounter(COUNTER_EFFECTS_ACTIONS, (float)(et - st));

    time -= dt;
}

float Effect::getTime()
{
    return time;
}

void Effect::setPosition(b2Vec2 position) {
    this->position = position;
}
b2Vec2 Effect::getPosition() {
    return position;
}
void Effect::setHotSpotIndex(int index) {
    hotSpotIndex = index;
}

void Effect::setAnimation(hgeAnimation* animation, int blendMode) {
    this->animation = animation;
    this->animation->SetBlendMode(blendMode);
}

bool Effect::characterCrosses(Character* character)
{
    //printf("position %f %f ", position.x, position.y);
    switch ( prototype->getAreaType() ) {
        case EFFECT_AREA_TYPE_POINT:
            if (
                fabs( position.x - character->getX() ) < character->getHalfWidth()
                && fabs( position.y - character->getY() ) < character->getHalfHeight()
            ) {
//                printf("%f - %f < %f and %f - %f < %f\n", position.x, character->getX(), character->getHalfWidth(), position.y, character->getY(), character->getHalfHeight());
                return true;
            }
            break;
    }

//    printf("does not cross character\n");
    return false;
}
