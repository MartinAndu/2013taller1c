/* 
 * File:   PersonajeVista.cpp
 * Author: damian
 * 
 * Created on 23 de marzo de 2013, 12:25
 */

#include <view/entities/PlayerView.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_ttf.h>
#include <view/sound/SoundEffectHandler.h>
#include <model/Logs/Logs.h>
#include <cmath>
#include <string>

//Posicion de los pies del personaje respecto de la base de la imagen
#define OFFSET_Y	15
#define ANIMATION_CHANGE_DELAY 1
#define STANDING_ANIMATION_LOCATION_IN_IMAGE_FILE 16
#define DEFAULT_CHARACTER_ID	"characterDefault"
#define NUMBER_OF_STANDING_FRAMES 3
#define STEP_SOUND "resources/sound/player/steps.ogg"
#define ATTACK_SOUND "resources/sound/player/sword.ogg"

PlayerView::PlayerView()
//Llamamos al constructor de la superclase
:
		MobileEntityView() {
	camPos = new Position(0, 0);
	marco = 0;
	animationChangeRate = 0;
	numberOfClips = 0;
	movable = true;
	direction = DOWN;
	wasStanding = true;
	attacking = false;
	player = NULL;
	nameImage = NULL;
	currentSprite = DOWN;
	lastDirection = M_PI * 1 / 2;
	chatView = NULL;

}

PlayerView::PlayerView(PlayerView* otherPlayer) :
		MobileEntityView(otherPlayer) {
	imageHeight = otherPlayer->getImageHeight();
	imageWidth = otherPlayer->getImageWidth();
	delay = otherPlayer->getDelay();
	fps = otherPlayer->getFps();
	camPos = new Position(0, 0);
	marco = 0;
	animationChangeRate = 0;
	numberOfClips = otherPlayer->getNClips();
	setNumberOfRepeats(otherPlayer->getNumberOfRepeats());
	movable = true;
	direction = DOWN;
	wasStanding = true;
	attacking = false;
	player = NULL;
	nameImage = NULL;
	currentSprite = DOWN;
	lastDirection = M_PI * 1 / 2;
	textureHolder = otherPlayer->getTextureHolder();
	this->setName(otherPlayer->getName());
	chatView = NULL;
}

list<PlayerEvent*> PlayerView::getPlayerViewEvents(){
	return this->events;
}
void PlayerView::showFrame(SDL_Surface* screen, SDL_Rect* clip, bool drawFog) {
	SDL_Rect offset, offsetFog;

	if (drawFog)
		return;

	Vector3* position = player->getCurrentPos();
	float x = position->getX();
	float y = position->getY();
	offset.x = offsetFog.x = (int) x + camPos->getX()
			- this->anchorPixel->getX();
	int h = Tile::computePositionTile(0, 0).h;
	offset.y = offsetFog.y = (int) y + camPos->getY()
			- this->anchorPixel->getY() - h / 2;
	offset.w = offsetFog.w = clip->w;
	offset.h = offsetFog.h = clip->h;

	SDL_BlitSurface(this->image, clip, screen, &offset);

	if (player->playerIsActive() == false) {
		SDL_BlitSurface(fogImage, clip, screen, &offsetFog);
	}

	SDL_Rect offsetNombre;
	offsetNombre.x = (int) x + camPos->getX() - nameImage->w / 2;
	offsetNombre.y = (int) y + camPos->getY() - this->anchorPixel->getY()
			- h / 2 - 20;
	offsetNombre.w = nameImage->w;
	offsetNombre.h = nameImage->h;
	SDL_BlitSurface(nameImage, NULL, screen, &offsetNombre);
}

void PlayerView::draw(SDL_Surface* screen, Position* cam, bool drawFog) {

	UpdateCameraPos(cam);
	Show(screen, drawFog);

}

void PlayerView::UpdateCameraPos(Position* _camPos) {
	delete camPos;
	camPos = new Position(_camPos->getX(), _camPos->getY());
}

void PlayerView::setPersonaje(Player* personaje) {
	this->player = personaje;
	Vector2* anchorPixel = new Vector2(clip.w / 2, OFFSET_Y);
	player->getBase()->setAnchorPixel(anchorPixel);

}

void initSounds() {
	SoundEffectHandler::loadSound(string("walk"), STEP_SOUND);
	SoundEffectHandler::loadSound(string("attack"), ATTACK_SOUND);

}

FoggedSprite PlayerView::loadFoggedSprite(const char* modifier) {
	string id = string(modifier);
	FoggedSprite sprite;
	sprite.image = textureHolder->getTexture(name + id);
	sprite.foggedImage = textureHolder->getFogTexture(name + id);
	sprite.numberOfClips = computeNumberOfClips(sprite.image);
	return sprite;

}

map<string, FoggedSprite> PlayerView::loadSwordImages() {
	map<string, FoggedSprite> swordMap;
	FoggedSprite walkingSprite, blockingSprite, standingSprite, attackSprite,
			castSprite, hitSprite, dieSprite;

	walkingSprite = loadFoggedSprite(WALKING_MODIFIER);
	swordMap[string("walking")] = walkingSprite;

	standingSprite = loadFoggedSprite(IDLE_MODIFIER);
	swordMap[string("standing")] = standingSprite;

	attackSprite = loadFoggedSprite(ATTACK_MODIFIER);
	swordMap[string("attacking")] = attackSprite;

	blockingSprite = loadFoggedSprite(IDLE_BLOCKING_MODIFIER);
	swordMap[string("blocking")] = blockingSprite;

	castSprite = loadFoggedSprite(CAST_SPELL_MODIFIER);
	swordMap[string("cast")] = castSprite;

	hitSprite = loadFoggedSprite(HIT_MODIFIER);
	swordMap[string("hit")] = hitSprite;

	dieSprite = loadFoggedSprite(DIE_MODIFIER);
	swordMap[string("die")] = dieSprite;

	return swordMap;

}

map<string, FoggedSprite> PlayerView::loadBowImages() {
	map<string, FoggedSprite> bowMap;
	FoggedSprite walkingSprite, blockingSprite, standingSprite, attackSprite,
			castSprite, hitSprite, dieSprite;

	walkingSprite = loadFoggedSprite(BOW_WALKING_MODIFIER);
	bowMap[string("walking")] = walkingSprite;

	standingSprite = loadFoggedSprite(BOW_IDLE_MODIFIER);
	bowMap[string("standing")] = standingSprite;

	attackSprite = loadFoggedSprite(BOW_ATTACK_MODIFIER);
	bowMap[string("attacking")] = attackSprite;

	blockingSprite = loadFoggedSprite(IDLE_BLOCKING_MODIFIER);
	bowMap[string("blocking")] = blockingSprite;

	castSprite = loadFoggedSprite(BOW_CAST_SPELL_MODIFIER);
	bowMap[string("cast")] = castSprite;

	hitSprite = loadFoggedSprite(BOW_HIT_MODIFIER);
	bowMap[string("hit")] = hitSprite;

	dieSprite = loadFoggedSprite(BOW_DIE_MODIFIER);
	bowMap[string("die")] = dieSprite;

	return bowMap;

}

void PlayerView::loadPlayerImage() {

	weaponViewMap[string("sword")] = loadSwordImages();

	weaponViewMap[string("bow")] = loadBowImages();

	initSounds();
}

void PlayerView::setEntity(Entity* entity) {
	//TODO: Error check (si no es un personaje)
	Player* aux = (Player*) entity;
	player = aux;
}

void PlayerView::showStandingAnimation(SpriteType sprite, SDL_Surface* fondo,
		bool drawFog) {

	SDL_Rect clipToDraw;
	clipToDraw.x = imageWidth * currentClip * scaleWidth;

	clipToDraw.y = imageHeight * sprite;
	clipToDraw.w = imageWidth * scaleWidth;

	clipToDraw.h = imageHeight * scaleHeight;

	showFrame(fondo, &clipToDraw, drawFog);

	timeSinceLastAnimation = timer.getTimeSinceLastAnimation();

	//TODO - deberia ser numberOfClips-1 pero parece q esta mal la imagen ?¿

	//Apply delay
	if (currentClip < (numberOfClips - 1)
			&& timeSinceLastAnimation >= delay * 1000) {
		//Apply FPS cap
		if (animationRateTimer.getTimeSinceLastAnimation() >= 1000 / fps) {
			currentClip++;
			animationRateTimer.start();
		}

	} else {
		if (timeSinceLastAnimation >= delay * 1000)
			timer.start();
		currentClip = 0;
	}

}

void PlayerView::Show(SDL_Surface* fondo, bool drawFog) {
	//cosas del sound
	string walkID = string("walk");
	string attackID = string("attack");
	if (this->image == NULL)
		loadPlayerImage();

	if (marco >= numberOfClips) {
		marco = 0;
		if (player->isAttacking()) {
			player->cancelAttack();
			attacking = false;
			player->addEvent(new PlayerEvent(EVENT_CANCEL_ATTACK));
			SoundEffectHandler::stopSound(attackID);
		}
		if (player->isBlocking())
			marco = spriteMap[string("blocking")].numberOfClips - 1;
	}

	Vector2* movementDirection = this->player->getMovementDirection();
	float direction;

	Vector2* v = new Vector2(0, 0);
	if (movementDirection->isEqual(v))
		direction = lastDirection;
	else
		direction = movementDirection->getAngle();
	delete v;

	SpriteType sprite = DOWN;

	const float step = M_PI * 1 / 8;

	if (step * 15 < direction || direction < step)
		sprite = RIGHT;
	else if (step < direction && direction < step * 3)
		sprite = DOWN_RIGHT;
	else if (step * 3 < direction && direction < step * 5)
		sprite = DOWN;
	else if (step * 5 < direction && direction < step * 7)
		sprite = DOWN_LEFT;
	else if (step * 7 < direction && direction < step * 9)
		sprite = LEFT;
	else if (step * 9 < direction && direction < step * 11)
		sprite = UP_LEFT;
	else if (step * 11 < direction && direction < step * 13)
		sprite = UP;
	else if (step * 13 < direction && direction < step * 15)
		sprite = UP_RIGHT;

	//TODO - Aca se elige el map que corresponda con el arma equipada
	/*
	 *	spriteMap=selectWeaponView(this->player);
	 *
	 */
	spriteMap = weaponViewMap[string("sword")];

	FoggedSprite spriteToBeShown;
	if (player->isAttacking()) {
		attacking = true;
		//Si se estaba moviendo, reseteamos el marco para que no quede un # de clip invalido
		if (player->IsMoving()) {
			marco = 0;
			player->stop();
		}
		if (!SoundEffectHandler::isSoundPlaying(attackID))
			SoundEffectHandler::playSound(attackID);
		spriteToBeShown = spriteMap[string("attacking")];
	}

	if (player->isBlocking()) {
		SoundEffectHandler::stopSound(attackID);
		marco = 0;
		player->stop();
		spriteToBeShown = spriteMap[string("blocking")];
	}

	if (player->IsMoving()) {
		SoundEffectHandler::stopSound(attackID);
		if (!SoundEffectHandler::isSoundPlaying(walkID))
			SoundEffectHandler::playSound(walkID);
		spriteToBeShown = spriteMap[string("walking")];
	} else
		SoundEffectHandler::stopSound(walkID);

	if (!player->IsMoving() && !player->isAttacking()
			&& !player->isBlocking()) {
		if (!wasStanding) {
			timer.start();
			wasStanding = true;
		}
		spriteToBeShown = spriteMap[string("standing")];
		image = spriteToBeShown.image;
		fogImage = spriteToBeShown.foggedImage;
		numberOfClips = spriteToBeShown.numberOfClips;
		lastDirection = direction;
		showStandingAnimation(sprite, fondo, drawFog);
		return;
	}

	wasStanding = false;
	currentSprite = sprite;
	image = spriteToBeShown.image;
	fogImage = spriteToBeShown.foggedImage;
	numberOfClips = spriteToBeShown.numberOfClips;
	lastDirection = direction;
	playAnimation((SpriteType) currentSprite, fondo, drawFog);

}

PlayerView::~PlayerView() {
	//libera la memoria que pide para La imagen
}

int PlayerView::getNClips() {
	return this->numberOfClips;
}

void PlayerView::setNClips(int clips) {
	this->numberOfClips = clips;
}

Player* PlayerView::getPersonaje() {
	return this->player;
}

Player* PlayerView::getEntity() {
	return player;
}

void PlayerView::setName(std::string name) {
	this->name = name;
	SDL_Color color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	TTF_Font* font = TTF_OpenFont("resources/fonts/Baramond.ttf", 28);
	if (nameImage)
		SDL_FreeSurface(nameImage);
	nameImage = TTF_RenderText_Solid(font, name.c_str(), color);
	if (!nameImage || !font)
		Logs::logErrorMessage(
				"Error al cargar la fuente para el nombre del personaje");

}

void PlayerView::setShowableName(string name) {
	SDL_Color color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	TTF_Font* font = TTF_OpenFont("resources/fonts/Baramond.ttf", 28);
	if (nameImage)
		SDL_FreeSurface(nameImage);
	nameImage = TTF_RenderText_Solid(font, name.c_str(), color);
	if (!nameImage || !font)
		Logs::logErrorMessage(
				"Error al cargar la fuente para el nombre del personaje");
}

void PlayerView::playAnimation(SpriteType sprite, SDL_Surface* screen,
		bool drawFog) {
	SDL_Rect clipToDraw;
	clipToDraw.x = imageWidth * marco * scaleWidth;
	clipToDraw.y = imageHeight * sprite;
	clipToDraw.w = imageWidth * scaleWidth;
	clipToDraw.h = imageHeight * scaleHeight;

	showFrame(screen, &clipToDraw, drawFog);

	if (animationChangeRate == ANIMATION_CHANGE_DELAY) {
		this->marco++;
		animationChangeRate = 0; // Move to the next marco in the animation
	} else
		animationChangeRate++;

}

