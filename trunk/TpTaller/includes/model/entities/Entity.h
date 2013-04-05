/*
 * Entity.h
 *
 *  Created on: Mar 24, 2013
 *      Author: gonchub
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include <model/Vector2.h>
#include <model/entityProperties/Position.h>
#include <model/entityProperties/Speed.h>
#include <model/entityProperties/Power.h>
#include <model/entityProperties/Base.h>

#include <string>
#include <vector>
#include <iostream>
namespace std {
	class Entity {
	public:
		Entity();
		virtual void setPos(float x, float y);
		Vector2* GetCurrentPos();
		virtual string getClassName();
		virtual ~Entity();
		Base* getBase();
		void setBase(Base* base);
	protected:
		Vector2* currentPos;
		Base* base;
	};
}
#endif /* ENTITY_H_ */
