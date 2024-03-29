/*
 * Game.h
 *
 *  Created on: 22/03/2013
 *      Author: tomas
 */

#ifndef GAME_H_
#define GAME_H_

//STL
#include <ctime>

//pthread
#include <pthread.h>

//SDL
#include <SDL/SDL_events.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <Events.h>

//Ours
#include <model/persistence/PersistentConfiguration.h>
#include <model/entities/enemies/Mob.h>
#include <model/Chat.h>
#include <controller/MapController.h>
#include <controller/MapCameraController.h>
#include <controller/PlayerController.h>
#include <controller/ChatController.h>
#include <networking/MobUpdate.h>
#include <view/configuration/GameConfiguration.h>
#include <view/MapView.h>
#include <view/MapCameraView.h>
#include <view/TextHandler.h>
#include <view/networking/ChatWindowsView.h>
#include <view/Animation.h>
#include <AI/ArtificialIntelligence.h>

using namespace std;
class Game {
public:
        Game(PersistentConfiguration* configuration, bool multiplayer);
        virtual MenuEvent run();

        virtual void addNewPlayer(Player* player, PlayerView* view, Coordinates* coords);
        virtual void addNewMobileEntity(MobileEntity* entity, MobileEntityView* view, Coordinates* coords, int id);
        void addNewEntity(Entity* entity, EntityView* view, Coordinates* coords, int id);
        void removeMobileEntity(int id);
        void removeEntity(int id);
        void handleMobileEntityUpdates(vector<MobileEntityUpdate*> mobileEntityUpdates);
        PlayerView* getPlayerView();

        MapCameraView* getMapCameraView();
        MapData* getMapData();
        ItemViewHolder* getIterViewHolder();
        MobileEntityViewHolder* getMobileEntityViewHolder();
        void setChat(Chat* chat);
        list<PlayerEvent*> getEvents();
        void cleanEvents();
        Chat* getChat();
        bool isActive();
        void setActive();
        void setInactive();

        virtual ~Game();
protected:

        MapView* mapView;
        int fps;
        int tempFps;
        int fpsUpdatingTimer;
        GameConfiguration* gameConfig;
private:
        Mix_Music* musica;

        SDL_Surface* screen;
        SDL_Rect posFondo;
        SDL_Surface* fondo;

        MapData* mapData;
        ItemViewHolder* vh;
        MobileEntityViewHolder* mobEntViewHolder;
        ChatController* chatController ;
        ChatWindowsView* chatView;
        Chat* chat;
        Player* personaje;
        PlayerView* personajeVista;

        list<Player*> otherPlayers;
        list<PlayerView*> otherPlayerViews;
        list<ArtificialIntelligence*> ias;
        MapController* mapController;
        MapCameraController* cameraController;

        bool multiplayer;

        void applyFPS(int timer);
        void initMusic();
        void initScreen();
        void draw();
        void setUpCharacters(MapView* map,MapData* mapData,EntityViewMap* viewMap);
        void setUpEntities(MapView* map,MapData* mapData);
        void playersUpdate();
        SDL_Rect updateMousePosition();
        void setAnimationStartup();

        //FPS Crap

        TextHandler* textHandler;

        bool openAudio;
        bool active;
        SDL_Surface* pointer;
        SDL_Surface* pointerClicked;
        Animation clickAnimation;
    	SDL_Rect clickedLocation;



    	map<int,MobileEntity*> mobileEntities;
    	map<int,MobileEntityView*> mobileEntitiesView;
    	map<int,Entity*> entities;
    	map<int,EntityView*> entitiesView;
};

#endif /* GAME_H_ */
