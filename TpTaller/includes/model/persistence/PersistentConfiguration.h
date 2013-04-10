/*
 * PersistentConfiguration.h
 *
 *  Created on: Apr 1, 2013
 *      Author: gonchub
 */

#ifndef PERSISTENTCONFIGURATION_H_
#define PERSISTENTCONFIGURATION_H_

#include <model/entities/Entity.h>
#include <model/entities/player/Player.h>
#include <model/map/TextureHolder.h>
#include <model/map/MapData.h>
#include <view/configuration/GameConfiguration.h>
#include <view/EntityViewMap.h>

#include <vector>

class PersistentConfiguration {
public:
	PersistentConfiguration();
	virtual ~PersistentConfiguration();
	TextureHolder* getTextureHolder();
	void setTextureHolder(TextureHolder* textureHolder);
	std::vector<Player*> getPersonajeList();
	void setPersonajeList(std::vector<Player*> entityList);
	MapData* getMapData();
	void setMapData(MapData* mapData);
	GameConfiguration* getAnimationConfiguration();
	void setAnimationConfiguration(GameConfiguration* animationConfig);
	EntityViewMap* getEntityViewMap();
	void setEntityViewMap(EntityViewMap* entityViewMap);

private:
	std::vector<Player*> entityList;
	TextureHolder* textureHolder;
	MapData* mapData;
	GameConfiguration* animationConfiguration;
	EntityViewMap* entityViewMap;
};

#endif /* PERSISTENTCONFIGURATION_H_ */
