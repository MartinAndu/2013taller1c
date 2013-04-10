#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <model/map/MapData.h>
#include <model/entities/player/Player.h>
#include <view/entities/PlayerView.h>
#include <model/map/TextureHolder.h>
#include <view/EntityViewMap.h>
#include <view/MapCameraView.h>
#include <view/timer/Timer.h>

#include <string>
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>

using namespace std;

class MapView {
public:
	MapView(MapData* data, SDL_Surface* screen, EntityViewMap* viewMap);
	virtual ~MapView();

	SDL_Surface* getDrawingSurface();

	void draw(Position* cam);
	void movePlayer(int x, int y);
	void Update();
	void SetUpPersonajes();
	void AssignPersonaje(Player* _personaje);
	void SetUpEntity(EntityView* entityView, Coordinates coor);
	MapCameraView* getCamera();
	list<EntityView *> *entitiesView;
	TextureHolder* getTextureHolder();
	void setTextureHolder(TextureHolder* textureHolder);
	void setRunningPlayer();
private:
	TextureHolder* textureHolder;
	MapData* data;
	SDL_Surface* screen;
	EntityViewMap* viewMap;

	string texturesPaths[MapData::AMOUNT_TYPES];
	SDL_Surface* tilesTextures[MapData::AMOUNT_TYPES];
	string backgroundPath;
	Player* personaje;

	MapCameraView* camera;
	Timer timer;
};

#endif  /* MAP_H */