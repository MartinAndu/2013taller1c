#include <view/MapView.h>

#define MapMargin		40		// px
#define TilesScale  	1
#define CameraSpeed		15		// px

MapView::MapView(MapData* _data, SDL_Surface* screen) {
	this->screen = screen;
	data = _data;
	camera = new Position(data->GetNCols()/2, data->GetNRows()/2);

	SDL_Rect posTile = Tile::computePosition(data->GetNRows(), data->GetNCols());
	lastTilePos = new Position(posTile.x, posTile.y);

	DefineTexturePaths();
	GraphicalSetup();
	checkBoundaries();
}

Position* MapView::GetCamera() {
	return new Position( camera->getX(), camera->getY() );
}

void MapView::DefineTexturePaths() {
	/**
	 * TODO: Esto no tiene que estar (TextureHolder lo maneja)
	 */
	texturesPaths[MapData::NEUTRAL] = "texturesTiles/sand.png";
	texturesPaths[MapData::SOIL] = "texturesTiles/dirtTile.png";
	texturesPaths[MapData::WATER] = "texturesTiles/Water.png";
	texturesPaths[MapData::TREE] = "texturesTiles/stonebrick.png";
	backgroundPath = "./background.png";
}

void MapView::GraphicalSetup() {
	/**
	 * TODO: Esto no tiene que estar (TextureHolder lo maneja)
	 */
	for (int i = 0; i < MapData::AMOUNT_TYPES; i++) {
		tilesTextures[i] = IMG_Load(texturesPaths[i].c_str());
		if (tilesTextures[i] == NULL) {
			printf("Unable to load tile texture n°:%d: %s\n", i,
					SDL_GetError());
			exit(1);
		}
	}
	for (int i = 0; i < MapData::AMOUNT_TYPES; i++) {
		tilesTextures[i] = rotozoomSurfaceXY(tilesTextures[i], 0, TilesScale,
				TilesScale, 0);
		tilesTextures[i] = SDL_DisplayFormatAlpha(tilesTextures[i]);
	}
}

MapView::~MapView() {
}

void MapView::SetUpPersonajes() {
	// Setea la posicion por unica vez de los personajes (y en un futuro posiblemente
	// lo representable) en el mapa.
	SDL_Rect posTile;

	for (int col = 0; col < data->GetNCols(); col++) {
		for (int row = 0; row < data->GetNRows(); row++) {

			Personaje* personaje = data->GetPersonaje(row, col);
			if (personaje != NULL) {
				posTile = Tile::computePosition(row, col);
				posTile.x = camera->getX() + posTile.x;
				posTile.y = camera->getY() + posTile.y;

				personaje->setPos((float) posTile.x, (float) posTile.y);
			}
		}
	}
}

void MapView::Draw() {

//	CameraUpdate();

	//Personaje* personajes = NULL;
	SDL_Rect posTile;

	for (int col = 0; col < data->GetNCols(); col++) {

		for (int row = 0; row < data->GetNRows(); row++) {

			posTile = Tile::computePosition(row, col);
			posTile.x = camera->getX() + posTile.x;
			posTile.y = camera->getY() + posTile.y;
			/**
			 * TODO: El manejo de texturas se da aca (con el TextureHolder seteado).
			 */
//			std::string textureId = data->GetTileType(row, col);
//			SDL_Surface* textureImage = getTextureHolder().getImage(textureId);
//			textureImage = rotozoomSurfaceXY(textureImage, 0, TilesScale,
//					TilesScale, 0);
//			textureImage = SDL_DisplayFormatAlpha(textureImage);

			SDL_BlitSurface(tilesTextures[data->GetTileType(row, col)], NULL,
					screen, &posTile);

			TileData* tileData = data->GetTileData(row, col);

			Entity* entity = tileData->getNextEntity();
			while (entity!=NULL){
				/*Lucas: TODO!
					entity->draw();
				*/
				entity = tileData->getNextEntity();
			}


		}
	}

	// Luego se blitean todos los personajes despues de haber bliteado el piso
	// para que el piso no tape a los flacos.
	/*  if (personajes != NULL){
	 PersonajeVista* personajeVista =
	 new PersonajeVista(personajes, "resources/foo.png", screen);

	 personajeVista->UpdateCameraPos(cameraX, cameraY);
	 personajeVista->Mostrar();
	 }*/
}

void MapView::ClickOn(int x, int y, int button) {
	// Selecciona la casilla mas o menos bien, idealizandola como un cuadrado.
	SDL_Rect firstTile = Tile::computePosition(0, 0);
	firstTile.x = camera->getX() + firstTile.x;
	firstTile.y = camera->getY() + firstTile.y;

	int row = (y - firstTile.y) * 2 / firstTile.h;
	int col = (x - firstTile.x) / firstTile.w;

	printf("A\n");
	if (personaje != NULL) {
		Tile* toTile = new Tile(new Coordinates(row, col));
		data->movePersonaje(personaje, toTile);
		printf("B\n");
		//personaje->MoveTo(x - camera->getX(), y - camera->getY());
	}

	//printf("row: %d, col: %d\n", row, col);
}

void MapView::AssignPersonaje(Personaje* _personaje) {
	personaje = _personaje;
}

void MapView::Update() {

}

SDL_Surface* MapView::getDrawingSurface(){
	return screen;
}


void MapView::checkBoundaries(){
	if (camera->getX() > -MapMargin) camera->setX(-MapMargin);
	if (camera->getY() > -MapMargin) camera->setY(-MapMargin);
	if (camera->getX() < -lastTilePos->getX() + screen->w) camera->setX(-lastTilePos->getX() + screen->w);
	if (camera->getY() < -lastTilePos->getY() + screen->h) camera->setY(-lastTilePos->getY() + screen->h);
}


void MapView::moveCamera(CameraMove move){

	switch(move){
	case MOVE_UP:
		camera->setY( camera->getY() + CameraSpeed );
		break;
	case MOVE_DOWN:
		camera->setY( camera->getY() - CameraSpeed );
		break;
	case MOVE_LEFT:
		camera->setX( camera->getX() + CameraSpeed );
		break;
	case MOVE_RIGHT:
		camera->setX( camera->getX() - CameraSpeed );
		break;
	}

	checkBoundaries();

}
