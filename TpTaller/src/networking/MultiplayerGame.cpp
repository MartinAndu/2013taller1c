/*
 * MultiplayerGame.cpp
 *
 *  Created on: 11/05/2013
 *      Author: tomas
 */

#include <MultiplayerGame.h>
#include <model/map/MapData.h>
#include <AI/GolemAI.h>
#include <pthread.h>
#include <time.h>


using namespace std;

pthread_mutex_t entities_mutex;

MultiplayerGame::MultiplayerGame(PersistentConfiguration* configuration) {

	MapData* mapData = configuration->getMapData();
	nChests = 0;

	this->gameConfig = configuration->getAnimationConfiguration();
	this->fps = configuration->getAnimationConfiguration()->getFps();
	this->fpsUpdatingTimer = 0;
	this->tempFps = 0;
	Tile::setTileWidth(gameConfig->getTileWidth());
	Tile::setTileHeight(gameConfig->getTileHeight());

	vector<Item*> itemVector = configuration->getItems();
	vector<MobileEntity*> mobileEntityVector =
			configuration->getMobileEntityList();

	EntityViewMap* viewMap = configuration->getEntityViewMap();
	this->view = new MapView(mapData, NULL, viewMap);
	lastAddedView = 0;

	for (unsigned int i = 0; i < mobileEntityVector.size(); i++) {
		if (mobileEntityVector[i]->getName() != "flag") {
			ArtificialIntelligence* ia = new ArtificialIntelligence();
			ia->setMobileEntity(mobileEntityVector[i]);
			ias[i+1] = ia;
			mobileEntityVector[i]->setId(i+1);
			addMobileEntity(mobileEntityVector[i],
					mobileEntityVector[i]->getCoordinates());
		}
	}

	for (unsigned int i = 0; i < itemVector.size(); i++) {
		if (itemVector[i]->getClassName() == "Chest") nChests++;
		addEntity(itemVector[i], itemVector[i]->getCoordinates());
	}

	flag = configuration->getFlag();
	changeFlagPosition(mapData);

	chestTimer.start();

}

MenuEvent MultiplayerGame::run() {
	MapData* mapData = view->getMapData();

	mapData->cleanNewEntities();
	mapData->cleanNewMobileEntities();

	while (true) {
		int ticks = SDL_GetTicks();

		playersUpdate();
		updateEntities();
		updateMobs();
		addNewEntities();
		addNewMobileEntities();
		removeDeadEntities();
		removeDeadMobiles();
		updateGolems();
		addRandomChest();

		applyFPS(ticks);

	}

	return EXIT_EVENT;

}

void MultiplayerGame::addRandomChest() {
	MapData* mapData = view->getMapData();

	if (chestTimer.getTimeIntervalSinceStart() > CHEST_RESPAWN_TIME && nChests < MAX_CHESTS) {
		chestTimer.start();

		Tile* randomTile = mapData->getRandomTile();

		Chest* chest = new Chest();

		chest->setName("chest");
		Coordinates coord = randomTile->getCoordinates();
		chest->setCoordinates(coord.getRow(), coord.getCol());
		chest->setItemToContaing(ConfigurationReader::createRandomItem());

		mapData->addItem(coord.getRow(), coord.getCol(), chest);
		nChests++;
	}
}

void MultiplayerGame::changeFlagPosition(MapData* mapData) {

	int prevRow = flag->getCoordinates().getRow();
	int prevCol = flag->getCoordinates().getCol();

	Tile* newTile = mapData->getRandomTile();

	int row = newTile->getCoordinates().getRow();
	int col = newTile->getCoordinates().getCol();

	flag->setCoordinates(row, col);
	flag->setPos(Tile::computePosition(row, col + 1));
	flag->setTile(newTile);

	mapData->updateMobilePos(prevRow, prevCol, row, col, flag);

}

MobileEntity* MultiplayerGame::getFlag() {
	return flag;
}

void MultiplayerGame::createGolem(Player* player) {
	MapData* mapData = view->getMapData();
	Golem* golem = new Golem();

	Coordinates playerCoor = player->getCoordinates();
	Tile* playerTile = player->getTile();
	Coordinates* golemCoords;

	while (true) {
		golemCoords = new Coordinates(playerCoor.getRow() + rand() % 10 - 5,
									  playerCoor.getCol() + rand() % 10 - 5);
		// Repite hasta encontrar un tile valido que pertenezca al mapa
		if (golemCoords->isEqual(playerCoor) ||
			golemCoords->getRow() < 0 || golemCoords->getRow() >= mapData->getNRows() ||
			golemCoords->getCol() < 0 || golemCoords->getCol() >= mapData->getNCols() ) {
				delete golemCoords;
				continue;
		}
		break;
	}
	Tile* aproxTile = new Tile(golemCoords);

	Tile* golemTile = mapData->getValidTile(playerTile, aproxTile);

	Coordinates coor = aproxTile->getCoordinates();

	golem->setCoordinates(coor.getRow(), coor.getCol());
	golem->setPos(Tile::computePosition(coor.getRow(), coor.getCol()));
	golem->setTeam(player->getTeam());
	golem->setName("dragon");
	golem->setSpeed(player->getSpeed());
	golem->setOwner(player);
	golem->setTile(golemTile);
	this->createGolemIa(golem);
	golemsMap[player->getName()] = golem;
	golem->setId(addMobileEntity(golem, coor));

}

void MultiplayerGame::createGolemIa(Golem* golem) {
	ArtificialIntelligence* ia = new GolemAI();
	ia->setMobileEntity(golem);
	int newId = lastAddedView + 1;
	this->ias[newId] = ia;
	lastAddedView++;
	golem->setId(newId);
}

void MultiplayerGame::updateGolems(){
	for (map<string,Golem*>::iterator it = golemsMap.begin() ; it != golemsMap.end() ; ++it){
		if (it->second->isDead()){
			removeMobileEntity(it->second->getId());
		}
	}
}

void MultiplayerGame::addEventsToHandle(string playerName,
		vector<PlayerEvent*> events) {

	controllers[playerName]->handleEvents(events);
}

void MultiplayerGame::addNewPlayer(Player* player, Coordinates* coordiantes) {

	NetworkPlayerController* controller = new NetworkPlayerController(player,
			view->getMapData());
	controllers.insert(
			pair<string, NetworkPlayerController*>(player->getName(),
					controller));
	players.push_back(player);

	view->getMapData()->addPlayer(coordiantes->getRow(), coordiantes->getCol(),
			player);

	Coordinates coords = player->getCoordinates();
	mobilesCoords[player] = coords;
}

void MultiplayerGame::updateEntities() {
	map<int, Entity*>::iterator iter;
	for (iter = entities.begin(); iter != entities.end(); ++iter) {
		(iter->second)->updateDamageTaken();
	}
}

void MultiplayerGame::updateMobs() {
	MapData* mapa = view->getMapData();

	map<int, MobileEntity*>::iterator mobIter;
	for (mobIter = mobileEntities.begin(); mobIter != mobileEntities.end();
			++mobIter) {
		MobileEntity* mob = mobIter->second;
		if(mob){
			mob->update(mapa);
			mob->updateDamageTaken();
		}
	}

	map<int, ArtificialIntelligence*>::iterator iaIter;
	for (iaIter = ias.begin(); iaIter != ias.end(); ++iaIter) {
		(iaIter->second)->update(mapa);
	}

}

void MultiplayerGame::playersUpdate() {

	for (list<Player*>::iterator player = players.begin();
			player != players.end(); ++player) {
		(*player)->update(view->getMapData());
		(*player)->updateDamageTaken();
		if ((*player)->hasGolem()
				&& golemsMap.find((*player)->getName()) == golemsMap.end()) {
			createGolem(*player);
		}
	}

}

void MultiplayerGame::applyFPS(int timer) {
	timer = SDL_GetTicks() - timer;
	float elapsedMiliseconds = timer;
	unsigned int FPS = this->gameConfig->getFps();

	float delay = (float) 1000 / FPS;

	if (delay - elapsedMiliseconds > 0) {
		SDL_Delay(delay - elapsedMiliseconds);
		fps = (float) 1000 / delay;

	} else
		fps = (1000 / elapsedMiliseconds) + 5;
}

vector<ChatUpdate*> MultiplayerGame::getChatUpdates() {
	vector<ChatUpdate*> updates;

	for (list<Player*>::iterator player = players.begin();
			player != players.end(); ++player) {
		ChatUpdate* update = (*player)->generateChatUpdate();
		if (update) {
			updates.push_back(update);
		}
	}

	return updates;
}

vector<PlayerUpdate*> MultiplayerGame::getPlayersUpdates() {

	vector<PlayerUpdate*> updates;

	for (list<Player*>::iterator player = players.begin();
			player != players.end(); ++player) {
		PlayerUpdate* update = (*player)->generatePlayerUpdate();
		if (update)
			updates.push_back(update);
	}

	return updates;

}

vector<MobileEntityUpdate*> MultiplayerGame::getMobileEntitiesUpdates() {

	vector<MobileEntityUpdate*> updates;
	for (map<int, MobileEntity*>::iterator it = mobileEntities.begin();
			it != mobileEntities.end(); ++it) {
		MobileEntityUpdate* update = (it->second)->generateMobileEntityUpdate(
				it->first);
		if (update)
			updates.push_back(update);
	}

	return updates;

}

list<Player*> MultiplayerGame::getPlayers() {
	return this->players;
}

map<int, MobileEntityInfo*> MultiplayerGame::getMobileEntityInfo() {

	map<int, MobileEntityInfo*> infos;

	for (map<int, MobileEntity*>::iterator it = mobileEntities.begin();
			it != mobileEntities.end(); ++it) {
		MobileEntity* ent = it->second;
		MobileEntityInfo* info = new MobileEntityInfo();
		info->setId(it->first);
		info->setName(ent->getName());
		info->setEntity(ent);
		int col = ent->getCoordinates().getCol();
		int row = ent->getCoordinates().getRow();
		Coordinates* tmp = new Coordinates(row, col);
		info->setInitCoordinates(tmp);
		infos[it->first] = info;
	}

	return infos;

}

vector<int> MultiplayerGame::getDeletedMobileEntities() {
	return deletedMobileEntities;
}

map<int, EntityInfo*> MultiplayerGame::getEntityInfo() {

	map<int, EntityInfo*> infos;

	for (map<int, Entity*>::iterator it = entities.begin();
			it != entities.end(); ++it) {
		Entity* ent = it->second;
		EntityInfo* info = new EntityInfo();
		info->setId(it->first);
		info->setName(ent->getName());
		info->setClassName(ent->getClassName());
		info->setEntity(ent);
		int col = ent->getCoordinates().getCol();
		int row = ent->getCoordinates().getRow();
		Coordinates* tmp = new Coordinates(row, col);
		info->setInitCoordinates(tmp);
		infos[it->first] = info;
	}

	return infos;

}

vector<int> MultiplayerGame::getDeletedEntities() {
	return deletedEntities;
}

void MultiplayerGame::addNewMobileEntities() {
	MapData* mapData = view->getMapData();
	vector<MobileEntity*> newMobiles = mapData->getnewMobileEntities();
	mapData->cleanNewMobileEntities();
	if(newMobiles.size()>0)
		cout << "hay "<<newMobiles.size()<< " Mobiles nuevas "<<endl;
	std::vector<MobileEntity *>::const_iterator iter;
	for (iter = newMobiles.begin(); iter != newMobiles.end(); ++iter) {

		MobileEntity* current = *iter;

		Coordinates coords = current->getCoordinates();

		addMobileEntity(current, coords);
	}
}

void MultiplayerGame::addNewEntities() {
	MapData* mapData = view->getMapData();

	vector<Entity*> newEntities = mapData->getNewEntities();
	mapData->cleanNewEntities();

	std::vector<Entity *>::const_iterator iter;
	for (iter = newEntities.begin(); iter != newEntities.end(); ++iter) {
		Entity* current = *iter;

		addEntity(current, current->getCoordinates());
	}
}

int MultiplayerGame::getEntityId(Entity* entity) {
	map<int, Entity*>::iterator it;
	for (it = entities.begin(); it != entities.end(); ++it) {
		Entity* current = it->second;

		if (current == entity)
		{

			return it->first;
		}
	}

	std::cout << "Entity no encontrada" << std::endl;
	return -1;
}

int MultiplayerGame::getMobileId(MobileEntity* entity) {
	map<int, MobileEntity*>::iterator it;
	for (it = mobileEntities.begin(); it != mobileEntities.end(); ++it) {
		MobileEntity* current = it->second;

		if (current == entity)
		{
			return it->first;
		}
	}

	std::cout << "MobileEntity no encontrada" << std::endl;
	return -1;
}

void MultiplayerGame::removeDeadEntities() {
	MapData* mapData = view->getMapData();

	list<Entity*> deadEntities = mapData->getDeadEntities();

	list<Entity *>::const_iterator iter;
	for (iter = deadEntities.begin(); iter != deadEntities.end(); ++iter) {
		Entity* current = *iter;

		removeEntity(getEntityId(current));
	}
}

void MultiplayerGame::removeDeadMobiles() {
	MapData* mapData = view->getMapData();

	list<MobileEntity*> deadEntities = mapData->getDeadMobiles();

	list<MobileEntity *>::const_iterator iter;
	for (iter = deadEntities.begin(); iter != deadEntities.end(); ++iter) {
		MobileEntity* current = *iter;

		removeMobileEntity(current->getId());
	}
}

int MultiplayerGame::addMobileEntity(MobileEntity* entity,
		Coordinates coordiantes) {

	entity->setCoordinates(coordiantes.getRow(), coordiantes.getCol());
	int x = Tile::computePosition(coordiantes.getRow(), coordiantes.getCol(),
			true)->getX();
	int y = Tile::computePosition(coordiantes.getRow(), coordiantes.getCol(),
			true)->getY();
	entity->setPos(x, y, 0);
	int newId = lastAddedView + 1;
	mobileEntities[newId] = entity;
	entity->setId(newId);
	lastAddedView = newId;

	return newId;
}

void MultiplayerGame::removeMobileEntity(int id) {
	if (mobileEntities.count(id) != 0) {
		if (ias.count(id) != 0) {
		//	delete ias[id];
			ias.erase(id);
		}
		mobileEntities.erase(id);


		deletedMobileEntities.push_back(id);
	}
}

int MultiplayerGame::addEntity(Entity* entity, Coordinates coordiantes) {

	entity->setCoordinates(coordiantes.getRow(), coordiantes.getCol());
	int newId = lastAddedView + 1;
	entities[newId] = entity;
	lastAddedView = newId;

//	view->getMapData()->addItem(coordiantes.getRow(), coordiantes.getCol(),(Item*)entity);

	return newId;
}

void MultiplayerGame::removeEntity(int id) {
	if (entities.count(id) != 0) {
		Entity* ent = entities[id];
		if (ent->getClassName() == "Chest"){
			nChests--;
		}
		entities.erase(id);
		//TODO: SACARLO TAMBIEN DE MAP DATA

		deletedEntities.push_back(id);
	}
}

MultiplayerGame::~MultiplayerGame() {
	// TODO Auto-generated destructor stub
}

