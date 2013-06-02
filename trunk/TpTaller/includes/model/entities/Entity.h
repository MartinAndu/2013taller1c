/*
 * Entity.h
 *
 *  Created on: Mar 24, 2013
 *      Author: gonchub
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include <model/Vector2.h>	// Capas hay que volarlo
#include <model/Vector3.h>
#include <model/map/Tile.h>
#include <model/entityProperties/Coordinates.h>
#include <model/entityProperties/Position.h>
#include <model/entityProperties/Speed.h>
#include <model/entityProperties/Power.h>
#include <model/entityProperties/Base.h>
#include <list>
#include <string>
#include <vector>
#include <iostream>

class MobileEntity;
class Item;
class LifeHeart;

using namespace std;
class Entity {
public:
	Entity();
	Entity(Entity* entity);
	void setPos(float x, float y, float z = 0);
	Vector3* getCurrentPos();
	virtual string getClassName();
	virtual void applyDamage(int damage);
	virtual ~Entity();
	Base* getBase();
	void setBase(Base* base);
	virtual std::string getName();
	virtual void setName(std::string name);
	Coordinates getCoordinates();
	void setCoordinates(int row, int col);
	void setBaseSizes(int width, int height);

	list<Entity*> getVisibleEnemies();
	virtual int getLife();
	virtual int getMagic();
	virtual void setLife(int life);
	virtual void setMagic(int magic);
	void setTile(Tile* _tile);
	virtual Tile* getTile();
	bool isMobile();

	void collideTo(MobileEntity& entity);
	void collideTo(Item& item);
	void collideTo(LifeHeart& heart);

	// Mission manager.
	bool isDead();
	int getTeam();
	int getKilledBy();
	void setTeam(int team);
	void setKilledBy(int team);
	bool getPositionInitialized();
protected:
	Vector3* currentPos;
	Base* base;
	std::string name;
	Coordinates* coord;
	int life;
	int magic;
	bool mobile;
	list<Entity*> enemies;
	Tile* currentTile;

	// Mission manager.
	int team;
	int killedBy;
private:
	bool positionInitialized;
};

#endif /* ENTITY_H_ */
