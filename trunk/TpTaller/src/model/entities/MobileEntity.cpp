/*
 * MobileEntity.cpp
 *
 *  Created on: May 25, 2013
 *      Author: mint
 */

#include <model/entities/MobileEntity.h>
using namespace std;

MobileEntity::MobileEntity() {
	speed = new Speed();
	initSpeed = new Speed();
	hasChanged = true;
	attacking = false;
	endPos = new Vector3(0, 0);
	this->speed = new Speed(0, new Vector2(0, 0));
	this->initSpeed = NULL;
	this->path = new list<Tile *>();
	this->currentTile = new Tile(new Coordinates(0, 0));
}

void MobileEntity::setPos(float x, float y, float z) {
	currentPos->setValues(x, y, z);

	endPos->setValues(currentPos->getX(), currentPos->getY());
}

void MobileEntity::moveTo(int x, int y, int z) {
	endPos = new Vector3(x, y, z);
}

bool MobileEntity::IsMoving() {
	return !(currentPos->isEqual(endPos));
}

string MobileEntity::getClassName() {
	return "MobileEntity";
}

bool MobileEntity::isRunning() {
	if (speed->getMagnitude() >= 2 * initSpeed->getMagnitude())
		return true;
	return false;
}

void MobileEntity::moveImmediately(Coordinates coords) {
	Coordinates* newCoords = new Coordinates(coords);
	Tile* tile = new Tile(newCoords);
	Position* pos = tile->getPosition();

	currentPos->setValues(pos->getX(), pos->getY(), pos->getZ());
	endPos->setValues(pos->getX(), pos->getY(), pos->getZ());
	delete tile;

	this->hasChanged = true;
}

void MobileEntity::update() {
	if (IsMoving() == false) {
		if (path->size() == 0)
			return;
		else
			loadNextPosition();
	}

	float relationSpeed = ((float) Tile::getTileHeight())
			/ ((float) Tile::getTileWidth());

	Vector3* moveDirection = new Vector3(endPos->getX() - currentPos->getX(),
			endPos->getY() - currentPos->getY(),
			endPos->getZ() - currentPos->getZ());

	if (moveDirection->getNorm() < getSpeed()->getMagnitude() + 1) {
		// Close enough to the end position to move in one step.
		currentPos->setValues(endPos->getX(), endPos->getY());
		if (path->size() == 0)
			return;
		else
			loadNextPosition();
	} else {
		moveDirection->normalize();
		moveDirection->multiplyBy(
				fabs(moveDirection->getY()) * (relationSpeed - 1) + 1);
		moveDirection->multiplyBy(getSpeed()->getMagnitude());
		currentPos->add(moveDirection);
	}
}

void MobileEntity::update(MobileEntityUpdate* update) {

	this->currentPos->setValues(update->getCurrentPos()->getX(),
			update->getCurrentPos()->getY(), update->getCurrentPos()->getZ());
	this->endPos->setValues(update->getEndPos()->getX(),
			update->getEndPos()->getY(), update->getEndPos()->getZ());
	this->speed->setMagnitude(update->getSpeed()->getMagnitude());
	this->speed->setDirection(update->getSpeed()->getDirection());
	this->attacking = update->isAttacking();
	this->currentTile = update->getTile();

	Coordinates currentTileCoords = this->currentTile->getCoordinates();
	Coordinates nextTileCoords = update->getNextTile()->getCoordinates();
	if ((!currentTileCoords.isEqual(nextTileCoords)) && (this->path->empty())) {
		this->path->push_front(update->getNextTile());
	}

}

MobileEntityUpdate* MobileEntity::generateMobileEntityUpdate() {

	if (!this->hasChanged)
		return NULL;

	MobileEntityUpdate* update = new MobileEntityUpdate();

	update->setName(this->name);

	update->setCurrentPos(this->currentPos);
	update->setEndPos(this->endPos);
	update->setSpeed(this->speed);
	update->setAttacking(this->attacking);
	update->setTile(this->currentTile);
	update->setInitCoordinates(this->coord);
	if (!this->path->empty()) {
		update->setNextTile(this->path->front());
	} else {
		update->setNextTile(this->currentTile);
	}

	return update;
}

void MobileEntity::loadNextPosition() {
	if (path->empty())
		return;
	Tile* tile = path->front();
	path->remove(tile);

	Position* tilePos = tile->getPosition();
	moveTo(tilePos->getX(), tilePos->getY());

	setTile(tile);
}

Vector2* MobileEntity::getMovementDirection() {
	Vector2* moveDirection = new Vector2(endPos->getX() - currentPos->getX(),
			endPos->getY() - currentPos->getY());
	Vector2* v = new Vector2(0, 0);

	if (moveDirection->isEqual(v)) {
		delete v;
		return moveDirection;
	}

	delete v;

	moveDirection->normalize();

	return moveDirection;
}

MobileEntity::MobileEntity(string name, Position* position, Speed* speed) {
	this->speed = speed;
	this->name = name;
	this->path = new list<Tile *>();
	currentTile = new Tile(new Coordinates(0, 0));
	this->currentPos = new Vector3(0, 0, 0);
	this->base = new Base();
	endPos = new Vector3(0, 0, 0);
	endPos->setValues(currentPos->getX(), currentPos->getY());
	attacking = false;
	hasChanged = false;
	initSpeed = 0;
}

MobileEntity::~MobileEntity() {
	delete currentPos;
	delete endPos;
	delete this->speed;
	if (initSpeed != NULL)
		delete initSpeed;
	if (currentTile)
		delete currentTile;
}

void MobileEntity::setPosition(Position* position) {
	Position* pos = Tile::computePosition(position->getX(), position->getY());
	currentPos->setValues(pos->getX(), pos->getY());
	delete pos;
}

Speed* MobileEntity::getSpeed() {
	return speed;
}

void MobileEntity::setSpeed(Speed* speed) {
	if (this->speed)
		delete this->speed;
	this->speed = speed;
	if (initSpeed == NULL)
		initSpeed = new Speed(speed->getMagnitude(), new Vector2(0, 0));
}

void MobileEntity::setInitSpeed(Speed* initSpeed) {
	this->initSpeed = initSpeed;
}

void MobileEntity::assignPath(list<Tile *> *_path) {
	if (path)
		delete path;
	this->path = _path;

	loadNextPosition();
}

void MobileEntity::setTile(Tile* _tile) {
	if (this->currentTile)
		delete this->currentTile;
	currentTile = _tile;

	// Las coordinates se actualizan en EntityViewMap
}

Tile* MobileEntity::getTile() {
	// Devuelve una copia del tile
	Tile* retval = new Tile();
	retval->setCoordinates(currentTile->getCoordinates());

	return retval;
}

void MobileEntity::setSpeedMagnitude(int mag) {
	speed->setMagnitude(mag);
}

bool MobileEntity::isAttacking() {
	return attacking;
}

void MobileEntity::attack() {
	attacking = true;
}

void MobileEntity::setAttack(bool attacking) {
	this->attacking = attacking;
}

void MobileEntity::cancelAttack() {
	attacking = false;
	hasChanged = true;
}

void MobileEntity::emptyPath() {
	std::list<Tile *>::iterator iter;
	for (iter = path->begin(); iter != path->end(); ++iter) {
		Tile* tile = *iter;

		delete tile;
	}

	delete path;
	this->path = new list<Tile *>();
}

void MobileEntity::stop() {
	emptyPath();
}

MobileEntity& MobileEntity::operator=(const MobileEntity &other) {

	this->name = other.name;
	*(this->currentPos) = *(other.currentPos);
	*(this->endPos) = *(other.endPos);
	*(this->speed) = *(other.speed);
	*(this->initSpeed) = *(other.initSpeed);
	*(this->base) = *(other.base);
	*(this->currentTile) = *(other.currentTile);
	*(this->path) = *(other.path);
	return *this;
}

//Operator to transform the object into a stream.
ostream& operator <<(std::ostream& out, const MobileEntity& MobileEntity) {
	out << MobileEntity.name << " " << *(MobileEntity.currentPos) << " "
			<< *(MobileEntity.endPos) << " " << *(MobileEntity.speed) << " "
			<< *(MobileEntity.initSpeed) << " " << *(MobileEntity.base) << " ";

	out << *(MobileEntity.currentTile) << " ";
	return out;
}

//Operator to load an object from a stream
istream& operator >>(std::istream& in, MobileEntity& MobileEntity) {
	string name;
	in >> name;
	MobileEntity.setName(name);
	Vector3 pos;
	in >> pos;
	MobileEntity.setPos(pos.getX(), pos.getY(), pos.getZ());
	in >> pos;
	MobileEntity.moveTo(pos.getX(), pos.getY(), pos.getZ());
	Speed* speed = new Speed();
	in >> *speed;
	MobileEntity.setSpeed(speed);
	Speed* initSpeed = new Speed();
	in >> *initSpeed;
	MobileEntity.setInitSpeed(initSpeed);
	Base* base = new Base();
	in >> *base;
	MobileEntity.setBase(base);
	Tile* tile = new Tile();
	in >> *tile;
	MobileEntity.setTile(tile);
	return in;
}

Vector3* MobileEntity::getEndPos() {
	return endPos;
}

void MobileEntity::setEndPos(float x, float y, float z) {
	endPos->setValues(x, y, z);
}

void MobileEntity::setHasChanged(bool change) {
	hasChanged = change;
}

bool MobileEntity::getHasChanged() {
	return hasChanged;
}

