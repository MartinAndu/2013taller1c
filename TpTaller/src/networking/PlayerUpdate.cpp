/*
 * PlayerUpdate.cpp
 *
 *  Created on: 08/05/2013
 *      Author: tomas
 */

#include <networking/PlayerUpdate.h>

using namespace std;

PlayerUpdate::PlayerUpdate() :
		MobileEntityUpdate() {
	this->currentPos = new Vector3();
	this->endPos = new Vector3();
	this->speed = new Speed();
	this->attacking = false;
	this->blocking = false;
	this->active = true;
	this->currentTile = new Tile();
	this->nextTile = new Tile();
	this->initCoords = new Coordinates();
	this->chat = NULL;
	this->invulnerable = false;
	this->makingEarthquake = false;
	this->golem = false;
	this->usingCrystalBall = false;
	this->mapItem = false;
	this->crystalBall = false;
}

void PlayerUpdate::setChat(Chat* chat) {
	this->chat = chat;
}

bool PlayerUpdate::isBlocking() {
	return blocking;
}

void PlayerUpdate::setBlocking(bool blocking) {
	this->blocking = blocking;
}

bool PlayerUpdate::isActive() {
	return active;
}

void PlayerUpdate::setActive(bool active) {
	this->active = active;
}

void PlayerUpdate::setMapItem(bool mapItem) {
	this->mapItem = mapItem;
}

bool PlayerUpdate::getMapItem() {
	return mapItem;
}

void PlayerUpdate::setMakingEarthquake(bool makingEarthquake) {
	this->makingEarthquake = makingEarthquake;
}

bool PlayerUpdate::getMakingEarthquake() {
	return makingEarthquake;
}

void PlayerUpdate::setUsingCrystalBall(bool usingCrystalBall) {
	this->usingCrystalBall = usingCrystalBall;
}

bool PlayerUpdate::getUsingCrystalBall() {
	return usingCrystalBall;
}

void PlayerUpdate::setCrystalBall(bool crystalBall) {
	this->crystalBall = crystalBall;
}

bool PlayerUpdate::getCrystalBall() {
	return crystalBall;
}

void PlayerUpdate::setFrostWandWeapon(bool frostWandWeapon) {
	this->frostWandWeapon = frostWandWeapon;
}

bool PlayerUpdate::getFrostWandWeapon() {
	return frostWandWeapon;
}

void PlayerUpdate::setInvulnerable(bool inv) {
	this->invulnerable = inv;
}

bool PlayerUpdate::getIsInvulnerable() {
	return invulnerable;
}

void PlayerUpdate::setSelectedWeapon(int selectedWeapon) {
	this->selectedWeapon = selectedWeapon;
}

int PlayerUpdate::getSelectedWeapon() {
	return selectedWeapon;
}

//Operator to transform the object into a stream.
ostream& operator <<(std::ostream& out, const PlayerUpdate& update) {

	out << update.name
		<< " " << *update.currentPos
		<< " " << *update.endPos
		<< " " << *update.speed
		<< " " << update.attacking
		<< " " << update.blocking
		<< " " << update.active
		<< " " << *update.currentTile
		<< " " << *update.nextTile
		<< " " << update.life
		<< " " << update.magic
		<< " " << update.lastAttackingDirection
		<< " " << update.team
		<< " " << update.castingSpell
		<< " " << update.viewRange
		<< " " << update.frozen
		<< " " << update.makingEarthquake
		<< " " << update.invulnerable
		<< " " << update.golem
		<< " " << update.mapItem
		<< " " << update.usingCrystalBall
		<< " " << update.crystalBall
		<< " " << update.frostWandWeapon
		<< " " << update.selectedWeapon;

	return out;
}

//Operator to load an object from a stream
istream& operator >>(std::istream& in, PlayerUpdate& update) {

	string name;
	in >> name;
	update.setName(name);
	Vector3 vect;
	// 1ro currentPos
	in >> vect;
	update.setCurrentPos(&vect);
	// luego endPos
	in >> vect;
	update.setEndPos(&vect);
	Speed speed;
	in >> speed;
	update.setSpeed(&speed);
	bool attacking;
	in >> attacking;
	update.setAttacking(attacking);
	bool blocking;
	in >> blocking;
	update.setBlocking(blocking);
	bool active;
	in >> active;
	update.setActive(active);
	Tile tile;
	in >> tile;
	update.setTile(&tile);
	in >> tile;
	update.setNextTile(&tile);
	int life, magic;
	in >> life;
	update.setLife(life);
	in >> magic;
	update.setMagic(magic);
	in >> update.lastAttackingDirection;
	int team;
	in >> team;
	update.setTeam(team);
	int castingSp;
	in >> castingSp;
	update.setCastingSpell(castingSp);
	int view;
	in >> view;
	update.setViewRange(view);
	in >> update.frozen;
	in >> update.makingEarthquake;
	in >> update.invulnerable;
	in >> update.golem;
	in >> update.mapItem;
	in >> update.usingCrystalBall;
	in >> update.crystalBall;
	in >> update.frostWandWeapon;
	in >> update.selectedWeapon;
	return in;
}

PlayerUpdate::~PlayerUpdate() {
}
