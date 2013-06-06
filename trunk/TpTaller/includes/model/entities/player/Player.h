/* 
 * File:   Personaje.h
 * Author: damian
 *
 * Created on 23 de marzo de 2013, 12:13
 */
#ifndef PERSONAJE_H
#define	PERSONAJE_H

#include <stdio.h>
#include <iostream>
#include <list>
#include <model/map/Tile.h>
#include <model/entities/Entity.h>
#include <model/Chat.h>
#include <model/entities/MobileEntity.h>
#include <model/Vector2.h>	// Capas hay q volarlo
#include <model/Vector3.h>
#include <networking/PlayerUpdate.h>
#include <networking/ChatUpdate.h>
#include <model/Weapons/Weapon.h>
#include <model/Weapons/Sword.h>
#include <model/entities/Items/Item.h>
#include <model/entities/Items/LifeHeart.h>

#define QUAKE_SPELL_ID "quake"
#define NONE_SPELL_ID "none"

using namespace std;

class Player: public MobileEntity {
public:
	Player();
	Player(std::string name, Position* position, Speed* speed,
			std::vector<Power*> powers);
	virtual ~Player();
	string getClassName();

	void updateFromServer(PlayerUpdate* update);
	PlayerUpdate* generatePlayerUpdate();
	std::vector<Power*> getPowers();
	void setPowers(std::vector<Power*> powers);
	void setViewRange(int _viewRange);
	int getViewRange();

	bool isMainPlayer();
	void setAsMainPlayer();


	void block();
	void setBlock(bool blocking);
	void cancelBlock();
	bool isBlocking();

	bool playerIsActive();
	void setActive();
	void SetUnactive();

	virtual void collideTo(Entity& entity);
	virtual void reverseCollide(Entity& entity);
	virtual void attack(Entity& entity);


	//Operator to transform the object into a stream.
	friend ostream& operator <<(std::ostream&, const Player&);

	//Operator to load an object from a stream
	friend istream& operator >>(std::istream&, Player&);

	Player& operator=(const Player &other);

	Chat* getChat();
	void setChat(Chat* chat);
	ChatUpdate* generateChatUpdate();
	Weapon* getCurrentWeapon();
private:
	Chat* chat;

	int viewRange;
	bool mainPlayer;

	std::vector<Power*> powers;
	list<Weapon*>* weapons;

	bool blocking;

	bool isActive;

};

#endif	/* PERSONAJE_H */

