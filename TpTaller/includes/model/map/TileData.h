/*
 * TileData.h
 *
 *  Created on: Apr 3, 2013
 *      Author: lucas
 */

#ifndef TILEDATA_H_
#define TILEDATA_H_

#include <model/entities/Entity.h>
#include <model/entities/personaje/Personaje.h>
#include <vector>

namespace std {

class TileData {
public:
	TileData();
	virtual ~TileData();

	//Getters
	std::string getType();
	Personaje* getPersonaje();
	Entity* getNextEntity();
	int getNumberOfEntitiesOnTile();

	//Setters
	void setType(std::string type);
	void setPersonaje(Personaje* newPersonaje);

	void addEntity(Entity* entity);

private:
	std::string tileType;
	Personaje* personaje;
	std::list<Entity*> entities;
	std::list<Entity*>::iterator iterator;
};

} /* namespace std */
#endif /* TILEDATA_H_ */