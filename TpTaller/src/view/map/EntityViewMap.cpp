/*
 * EntityViewMap.cpp
 *
 *  Created on: 05/04/2013
 *      Author: tomas
 */

#include <view/EntityViewMap.h>
#include <model/map/Tile.h>
#include <string>

#include <iostream>

using namespace std;
/* ************************************************************************** */
/* ****************            ENTITYVIEWMAP					************* */
/* ************************************************************************** */

EntityViewMap::EntityViewMap(MapData* _mapData) {
	this->mapData = _mapData;
	this->rows = mapData->getNRows();
	this->cols = mapData->getNCols();

	for (int i = 0; i < cols; i++) {
		list<EntityView*> EntityList;
		vector<list<EntityView*> > row(rows, EntityList);
		map.push_back(row);
	}

	entityHolder = NULL;
}

void EntityViewMap::positionEntityView(EntityView* entity,
		Coordinates coordinates) {
	int row = coordinates.getRow();
	int col = coordinates.getCol();

	map.at(col).at(row).push_back(entity);
	if (entity->isMovable()) {
		movableEntities.push_back(entity);
	}
}

void EntityViewMap::removeEntityView(EntityView* view){
	int row = view->getEntity()->getCoordinates().getRow();
	int col = view->getEntity()->getCoordinates().getCol();
	for (list<EntityView*>::iterator it = map.at(col).at(row).begin() ; it != map.at(col).at(row).end() ; ++it){
		if ((*it)->getEntity()->getName() == view->getEntity()->getName()){
			map.at(col).at(row).erase(it);
			break;
		}
	}
	for (list<EntityView*>::iterator it = movableEntities.begin() ; it != movableEntities.end() ; ++it){
		if ((*it)->getEntity()->getName() == view->getEntity()->getName()){
			movableEntities.erase(it);
			break;
		}
	}
}

int EntityViewMap::getNCols() {
	return cols;
}

int EntityViewMap::getNRows() {
	return rows;
}

list<EntityView*> EntityViewMap::getListAtRowAndCol(int row, int col) {
	return map.at(col).at(row);
}

void EntityViewMap::updateMovablePos() {

	for (list<EntityView*>::iterator it = movableEntities.begin();
			it != movableEntities.end(); ++it) {

		EntityView* entityView = *it;
		Entity* entity = entityView->getEntity();

		int initRow = entity->getCoordinates().getRow();
		int initCol = entity->getCoordinates().getCol();
		Vector3* entityPos = entity->getCurrentPos();
		Coordinates* c = Tile::getTileCoordinates(entityPos->getX(),
				entityPos->getY());
		int currentRow = c->getRow();
		int currentCol = c->getCol();

		if (initRow == currentRow && initCol == currentCol)
			continue;

		list<EntityView*> coord = getListAtRowAndCol(initRow, initCol);
		//if (!coord.empty())
			coord.remove(entityView);
		map[initCol][initRow] = coord;
		entity->setCoordinates(currentRow, currentCol);
		map.at(currentCol).at(currentRow).push_back(entityView);

		mapData->updatePlayerPos(initRow, initCol, currentRow, currentCol,
															(Player*) entity);

		delete c;
	}

}

void EntityViewMap::assingEntitiesView(EntityHolder* entityHolder) {
	this->entityHolder = entityHolder;
}

void EntityViewMap::drawViews(SDL_Surface* screen, Position* cam,
		std::map<string, int> visibleTiles) {

	updateMovablePos();

	for (int col = visibleTiles["StartCol"]; col < visibleTiles["EndCol"];
			col++) {

		for (int row = visibleTiles["StartRow"]; row < visibleTiles["EndRow"];
				row++) {

			TileData* tileData = mapData->getTileData(row, col);
			bool drawFog = !(tileData->getIsVisible());

			if (tileData->getWasVisible()) {
				list<EntityView*> aList = map.at(col).at(row);
				if (!aList.empty()) {
					list<EntityView*>::iterator it;
					for (it = aList.begin(); it != aList.end(); ++it) {
						EntityView* view = *it;
						if (!view)
							continue;
						view->draw(screen, cam, drawFog);
					}
				}
			}
		}

	}

}

EntityViewMap::~EntityViewMap() {

	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			list<EntityView*> aList = map.at(i).at(j);
			if (!aList.empty()) {
				list<EntityView*>::iterator it;
				for (it = aList.begin(); it != aList.end(); ++it) {
					EntityView* view = *it;
					if (!view)
						continue;
					delete view;
				}
			}
		}
	}
}
