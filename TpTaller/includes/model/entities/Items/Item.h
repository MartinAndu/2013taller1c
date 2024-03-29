/*
 * Item.h
 *
 *  Created on: Jun 1, 2013
 *      Author: gonchub
 */

#ifndef ITEM_H_
#define ITEM_H_

#include <model/entities/Entity.h>
#include <model/entities/MobileEntity.h>

class Player;

using namespace std;

class Item: public Entity {
public:
	Item();
	Item(Item* item);
	virtual string getClassName();
	virtual void applyEffects(Entity& entity);
	virtual void applyEffects(MobileEntity& entity);
	virtual void collideTo(MobileEntity& mobileEntity);
	virtual void collideTo(Player& mobileEntity);
	virtual ~Item();
};

#endif /* ITEM_H_ */
