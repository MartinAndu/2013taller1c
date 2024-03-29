/*
 * NetworkPlayerController.cpp
 *
 *  Created on: 01/05/2013
 *      Author: tomas
 */

#include <networking/NetworkPlayerController.h>
using namespace std;

NetworkPlayerController::NetworkPlayerController(Player* player,MapData* map) {
	this->player = player;
	this->data = map;
}

void NetworkPlayerController::handleEvent(PlayerEvent* event){

	EventType type = event->getEventType();
	switch (type){
	case EVENT_NONE:
		break;
	case EVENT_RUNNING:
		this->toggleRunning();
		break;
	case EVENT_WALKING:
		this->toggleRunning();
		break;
	case EVENT_ATTACK:
		this->playerAttack();
		break;
	case EVENT_CANCEL_ATTACK:
		player->cancelAttack();
		break;
	case EVENT_BLOCK:
		this->playerBlock();
		break;
	case EVENT_CANCEL_BLOCK:
		this->playerCancelBlock();
		break;
	case EVENT_MOVE:
		this->movePlayer(event->getTileCoordinates());
		break;
	case EVENT_STOP:
		this->player->stop();
		break;
	case EVENT_DISCONECT:
		this->player->SetUnactive();
		break;
	case EVENT_CONECT:
		this->player->setActive();
		break;
	case EVENT_CAST_SPELL:
		this->player->castSpell();
		break;
	case EVENT_CANCEL_CAST:
		this->player->setCastingSpell(false);
		break;
	case EVENT_EARTHQUAKE_START:
		this->player->setMakingEarthquake(true);
		break;
	case EVENT_GOLEM:
		this->player->createGolem();
		break;
	case EVENT_CHANGE_WEAPON:
		this->player->changeWeapon();
		break;
	case EVENT_CAST_SHIELD:
		player->setUsingShieldSpell(!player->getUsingShieldSpell());
		break;
	case EVENT_FROZE_ENEMIES:
		player->frozeEnemies();
		break;
	case EVENT_CHANGE_STATE_CRYSTAL_BALL:
		player->setUsingCrystalBall(!player->getUsingCrystalBall());
		break;
	}

	player->setHasChanged(true);

}

void NetworkPlayerController::toggleRunning() {
	if (player->isRunning()) {
		player->setSpeedMagnitude(player->getSpeed()->getMagnitude() / 2);

	} else {
		player->setSpeedMagnitude(player->getSpeed()->getMagnitude() * 2);
	}
}

void NetworkPlayerController::movePlayer(Coordinates* tileCoord){
	if (player == NULL) return;

	if (tileCoord->getCol() >= 0 && tileCoord->getCol() < data->getNCols() &&
		tileCoord->getRow() >= 0 && tileCoord->getRow() < data->getNRows() ) {

		if (this->playerHasclickedAnEntity(tileCoord)){
			Entity* entityColliding = getEntityToCollideTo();

			if (entityColliding->isAttackable() && entityColliding != player) {
				if (player->getCurrentWeapon()->canAttackTo(entityColliding->getClassName())) {
					playerAttackTo(entityColliding);
					return;
				}
			}

		}

		Tile* toTile = new Tile(new Coordinates(tileCoord->getRow(), tileCoord->getCol()));
		data->moveMobileEntity(player, toTile);
		delete toTile;

	}

}


void NetworkPlayerController::handleEvents(vector<PlayerEvent*> events){

	for (vector<PlayerEvent*>::iterator it = events.begin() ; it != events.end() ; ++it ){
		handleEvent(*it);
		delete *it;
	}

	events.clear();

}

Player* NetworkPlayerController::getPlayer(){
	return this->player;
}

NetworkPlayerController::~NetworkPlayerController() {
	// TODO Auto-generated destructor stub
}
