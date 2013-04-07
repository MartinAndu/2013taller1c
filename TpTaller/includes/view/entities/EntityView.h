/*
 * EntityView.h
 *
 *  Created on: Mar 26, 2013
 *      Author: fede
 *      Tuneada by: Lucas
 */

#ifndef ENTITYVIEW_H
#define ENTITYVIEW_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <model/entities/Entity.h>
#include <model/Vector2.h>
#include <model/entityProperties/Position.h>

#include <string>

using namespace std;
class EntityView {
public:
	EntityView();
	void setImagePath(std::string image_path);
	std::string getImagePath();
	virtual void setEntity(Entity* entity);
	virtual Entity* getEntity();
	virtual ~EntityView();
	Vector2* getAnchorPixel();
	void setAnchorPixel(Vector2* anchorPixel);

	virtual void draw(SDL_Surface* screen, Position* camera);
protected:
	SDL_Surface* load_image(std::string urlImagen);
	Entity* entity;
	SDL_Surface* image;
	Vector2* anchorPixel;
	string imagePath;
};


#endif /* ENTITYVIEW_H */