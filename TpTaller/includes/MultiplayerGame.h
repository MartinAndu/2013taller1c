/*
 * MultiplayerGame.h
 *
 *  Created on: 11/05/2013
 *      Author: tomas
 */

#ifndef MULTIPLAYERGAME_H_
#define MULTIPLAYERGAME_H_

#include <list>
#include <map>

#include <Game.h>
#include <model/Chat.h>
#include <model/persistence/PersistentConfiguration.h>
#include <model/entities/enemies/Mob.h>
#include <networking/MobileEntityInfo.h>
#include <networking/MobUpdate.h>
#include <networking/PlayerEvent.h>
#include <networking/NetworkPlayerController.h>
#include <AI/ArtificialIntelligence.h>
#include <networking/PlayerUpdate.h>
#include <networking/ChatUpdate.h>
#include <view/MapView.h>
#include <map>

using namespace std;

class MultiplayerGame {
public:
	MultiplayerGame(PersistentConfiguration* configuration);
	virtual MenuEvent run();
	virtual void addNewPlayer(Player* player, Coordinates* coordiantes);
	void addEventsToHandle(string playerName, vector<PlayerEvent*> events);
	void updatePlayersCoordinates();
	vector<PlayerUpdate*> getPlayersUpdates();
	vector<MobileEntityUpdate*> getMobileEntitiesUpdates();
	vector<ChatUpdate*> getChatUpdates();
	list<Player*> getPlayers();
	map<int,MobileEntityInfo*> getMobileEntityInfo();
	virtual ~MultiplayerGame();
	void deliverMessage(ChatMessage* msj);
	void updateMobs();

private:

	MapView* view;

	map< string, NetworkPlayerController*> controllers;

	list<Player*> players;
	list<ArtificialIntelligence*> ias;
	map<Player*, Coordinates> playersCoords;

	map<int,MobileEntity*> mobileEntities;
	map<int,MobileEntityView*> mobEntView;
	int lastAddedView;

	void playersUpdate();

	int fps;
	int tempFps;
	int fpsUpdatingTimer;
	GameConfiguration* gameConfig;

	void applyFPS(int timer);
};

#endif /* MULTIPLAYERGAME_H_ */
