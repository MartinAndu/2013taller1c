/*
 * Tile.h
 *
 *  Created on: Apr 1, 2013
 *      Author: gonchub
 */

#ifndef TILE_H_
#define TILE_H_

#include <model/map/Tile.h>
#include <model/entityProperties/Position.h>
#include <model/entityProperties/Coordinates.h>
#include <SDL/SDL.h>
#include <string>
#include <math.h>

using namespace std;

class Tile {
public:
	Tile();
	Tile(Tile* tile);
	Tile(Position* position, std::string textureId);
	Tile(Coordinates* _coordinates);
	Tile(Coordinates _coordinates);

	Position* getPosition();
	void setPosition(int x, int y);
	void setPosition(int x, int y, int z);
	void setPosition(Position* newPosition);
	Coordinates getCoordinates();
	void setCoordinates(int _row, int _col);
	void setCoordinates(Coordinates coords);
	std::string getTextureIdentifier();
	void setTextureIdentifier(std::string textureId);
	void setFScore(float _fScore);
	float getFScore();
	bool isEqual(Tile* otherTile);
	bool isNeighbor(Tile* otherTile);
	int getHashValue();
	static Position* computePosition(int row, int col, bool toTileZero = false);
	static SDL_Rect computePositionTile(int row, int col, bool toTileZero =
			false);
	static Coordinates* getTileCoordinates(int x, int y);
	static void setTileWidth(unsigned int _tileWidth);
	static void setTileHeight(unsigned int _tileHeight);
	static unsigned int getTileWidth();
	static unsigned int getTileHeight();

	static unsigned int tileWidth;
	static unsigned int tileHeight;

	//Operator to transform the object into a stream.
	friend ostream& operator <<(std::ostream&, const Tile&);

	//Operator to load an object from a stream
	friend istream& operator >>(std::istream&, Tile&);

	Tile& operator=(Tile &other);

	virtual ~Tile();
private:
	void updatePosition();

	static SDL_Rect getSquaredMapTilePos(int row, int col);
	static SDL_Rect getDiamondShapeMapTilePos(int row, int col);
	static Position* getDiamondShapeMapPos(int row, int col);

	Position* position;
	Coordinates* coordinates;
	std::string textureIdentifier;
	float fScore;	// for path finding
};

#endif /* TILE_H_ */
