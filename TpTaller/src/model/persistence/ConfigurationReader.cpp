/* 
 * File:   ConfigurationReader.cpp
 * Author: gonchub (el niño parser)
 * 
 * Created on March 21, 2013, 9:25 PM
 */

#include <model/persistence/ConfigurationReader.h>

using namespace std;

/**
 * YAML Configuration file position.
 */
#define PLAYERVIEWS_POSITION 0
#define ENTITYVIEWS_POSITION 1
#define GAME_CONFIGURATION_POSITION 2
#define TILE_DEFINITION_POSITION 3
#define MAP_DIMENSION_POSITION 4
#define MAP_TILES_POSITION 5

/* ************************************** *
 * *********** AUX STRUCTURES *********** *
 * ************************************** */

/**
 * Dimensions of a map.
 */
struct AuxMapDimension {
	int nrows, ncols;
};

/**
 * Map: list of tiles.
 */
struct AuxMap {
	std::vector<Tile*> tileList;
	AuxMapDimension dimension;
};

/* *************************************************** *
 * *********** PRINTING OF PARSED ELEMENTS *********** *
 * *************************************************** */

void printHeader(std::string title) {
	unsigned int titleLength = title.size() + 2;
	std::string output;
	output.append("*****");
	for (unsigned j = 0; j < titleLength; j++) {
		output.append("*");
	}
	output.append("*****");
	output.append("\n");

	output.append("***** ");
	output.append(title);
	output.append(" *****");
	output.append("\n");

	output.append("*****");
	for (unsigned j = 0; j < titleLength; j++) {
		output.append("*");
	}
	output.append("*****");
	output.append("\n");

	std::cout << output;
}

/**
 * Prints a tile to check for parsing integrity.
 */
void printTile(Tile* tile, std::ofstream& outputFile) {
	outputFile << "  - position: [" << tile->getPosition()->getX() << ", "
			<< tile->getPosition()->getY() << ", "
			<< tile->getPosition()->getZ() << "]" << std::endl;
	outputFile << "    texture: " << tile->getTextureIdentifier() << std::endl;
}

/**
 * Prints an personaje to check if it was parsed correctly.
 */
void printPersonaje(Personaje* parsedPersonaje, std::ofstream& outputFile) {
	outputFile << "    player:\n";
	outputFile << "      name: ";
	outputFile << parsedPersonaje->getName() << std::endl;
	outputFile << "      position: [";
	outputFile << parsedPersonaje->getPosition()->getX() << ", ";
	outputFile << parsedPersonaje->getPosition()->getY() << ", ";
	outputFile << parsedPersonaje->getPosition()->getZ() << "]\n";
	outputFile << "      speed:\n";
	outputFile << "        magnitude: ";
	outputFile << parsedPersonaje->getSpeed()->getMagnitude() << std::endl;
	outputFile << "        direction: [";
	outputFile << parsedPersonaje->getSpeed()->getDirection().GetX() << ", ";
	outputFile << parsedPersonaje->getSpeed()->getDirection().GetY() << "]";
	outputFile << std::endl;
	outputFile << "      powers:\n";
	for (unsigned i = 0; i < parsedPersonaje->getPowers().size(); i++) {
		outputFile << "        - name: "
				<< parsedPersonaje->getPowers()[i]->getName() << std::endl;
		outputFile << "          damage: "
				<< parsedPersonaje->getPowers()[i]->getDamage() << std::endl;
		outputFile << "          range: "
				<< parsedPersonaje->getPowers()[i]->getRange() << std::endl;
	}

}

/**
 * Prints an personaje to check if it was parsed correctly.
 */
void printEntity(Entity* parsedPersonaje, std::ofstream& outputFile) {
	outputFile << "    entity:\n";
	outputFile << "      name: ";
	outputFile << parsedPersonaje->getName() << std::endl;
	outputFile << "      position: [";
	outputFile << parsedPersonaje->getCurrentPos()->GetX() << ", ";
	outputFile << parsedPersonaje->getCurrentPos()->GetY() << ", ";
	outputFile << "0" << "]\n";
}

/**
 * Prints a list of player views.
 */
void printPlayerViews(std::vector<PersonajeVista*> entityViews,
		std::ofstream& outputFile) {

	outputFile << "- playerViews:" << std::endl;
	for (unsigned int j = 0; j < entityViews.size(); j++) {
		PersonajeVista* parsedEntityView = entityViews[j];
		outputFile << "  - imageSrc: " << parsedEntityView->getImagePath()
				<< std::endl;
		outputFile << "    anchorPixel: " << "["
				<< parsedEntityView->getAnchorPixel()->GetX() << ", "
				<< parsedEntityView->getAnchorPixel()->GetY() << "]"
				<< std::endl;
		printPersonaje((Personaje*) parsedEntityView->getEntity(), outputFile);
	}

}

/**
 * Prints a list of player views.
 */
void printEntityViews(std::vector<EntityView*> entityViews,
		std::ofstream& outputFile) {

	outputFile << "- entityViews:" << std::endl;
	for (unsigned int j = 0; j < entityViews.size(); j++) {
		EntityView* parsedEntityView = entityViews[j];
		outputFile << "  - imageSrc: " << parsedEntityView->getImagePath()
				<< std::endl;
		outputFile << "    anchorPixel: " << "["
				<< parsedEntityView->getAnchorPixel()->GetX() << ", "
				<< parsedEntityView->getAnchorPixel()->GetY() << "]"
				<< std::endl;
		printEntity(parsedEntityView->getEntity(), outputFile);
	}

}

/**
 * Prints an personaje to check if it was parsed correctly.
 */
void printTextureHolder(TextureHolder* parsedTileDefinition,
		std::ofstream& outputFile) {

	std::vector<std::string> textureIds;

	textureIds.push_back("soil");
	textureIds.push_back("neutral");
	textureIds.push_back("water");
	textureIds.push_back("tree");

	outputFile << "- tiles:" << std::endl;

	for (unsigned int i = 0; i < textureIds.size(); i++) {
		outputFile << "  - identifier: ";
		outputFile << textureIds[i] << std::endl;
		outputFile << "    imageSrc: ";
		outputFile << parsedTileDefinition->getTextureSrc(textureIds[i]);
		outputFile << std::endl;
	}

}

/**
 * Prints the dimensions of a map.
 */
void printMapDimensions(AuxMap &mapConfiguration, std::ofstream& outputFile) {

	outputFile << "- mapDimensions: ";
	outputFile << "[" << mapConfiguration.dimension.nrows;
	outputFile << ", " << mapConfiguration.dimension.ncols << "]" << std::endl;

}

/**
 * Prints the configured tiles of a map.
 */
void printConfiguredTiles(AuxMap &mapConfiguration, std::ofstream& outputFile) {

	outputFile << "- tileLocations:" << std::endl;

	for (unsigned int i = 0; i < mapConfiguration.tileList.size(); i++) {
		printTile(mapConfiguration.tileList[i], outputFile);
	}
}

/**
 * Prints the map configuration.
 */
void printMapConfiguration(AuxMap &mapConfiguration,
		std::ofstream& outputFile) {

	printMapDimensions(mapConfiguration, outputFile);
	printConfiguredTiles(mapConfiguration, outputFile);

}

/**
 * Prints the animated configuration.
 */
void printGameConfiguration(GameConfiguration* aConfig,
		std::ofstream& outputFile) {

	outputFile << "- gameConfiguration:" << std::endl;
	outputFile << "    fps: " << aConfig->getFps() << std::endl;
	outputFile << "    delay: " << aConfig->getDelay() << std::endl;
	outputFile << "    gameMusic: " << aConfig->getGameMusicSrc() << std::endl;
	outputFile << "    defaultScreenHeight: "
			<< aConfig->getDefaultScreenHeight() << std::endl;
	outputFile << "    defaultScreenWidth: " << aConfig->getDefaultScreenWidth()
			<< std::endl;
	outputFile << "    defaultBPP: " << aConfig->getDefaultBPP() << std::endl;
	outputFile << "    menuBackImage: " << aConfig->getMenuBackImageSrc()
			<< std::endl;
	outputFile << "    menuBackMusic: " << aConfig->getMenuBackMusicSrc()
			<< std::endl;

}

/* ************************************** *
 * *********** ENTITY PARSING *********** *
 * ************************************** */

/**
 * Sobrecarga de operador >> para llenar los datos de un puntero
 * a Position.
 */
void operator >>(const YAML::Node& yamlNode, Position* vector) {
	int auxX, auxY, auxZ;

	yamlNode[0] >> auxX;
	yamlNode[1] >> auxY;
	yamlNode[2] >> auxZ;

	vector->setX(auxX);
	vector->setY(auxY);
	vector->setZ(auxZ);
}

/**
 * Sobrecarga de operador >> para llenar los datos de un puntero
 * a Position.
 */
void operator >>(const YAML::Node& yamlNode, Vector2* vector) {
	int auxX, auxY;

	yamlNode[0] >> auxX;
	yamlNode[1] >> auxY;

	vector->SetValues(auxX, auxY);
}

/**
 * Sobrecarga de operador >> para llenar los datos de un puntero
 * a Power.
 */
void operator >>(const YAML::Node& yamlNode, Power* power) {
	std::string auxName;
	int auxDamage, auxRange;

	yamlNode["name"] >> auxName;
	yamlNode["damage"] >> auxDamage;
	yamlNode["range"] >> auxRange;

	power->setName(auxName);
	power->setDamage(auxDamage);
	power->setRange(auxRange);
}

/**
 * Sobrecarga de operador >> para llenar los datos de un puntero
 * a Speed.
 */
void operator >>(const YAML::Node& yamlNode, Speed* speed) {
	int auxMagnitude;
	yamlNode["magnitude"] >> auxMagnitude;
	speed->setMagnitude(auxMagnitude);

	Vector2* auxPosition = new Vector2(0, 0);
	yamlNode["direction"] >> auxPosition;
	speed->setDirection(Vector2(auxPosition->GetX(), auxPosition->GetY()));
	delete auxPosition;
}

/**
 * Sobrecarga de operador >> para llenar los datos de un puntero
 * a Personaje.
 */
void operator >>(const YAML::Node& yamlNode, Personaje* personaje) {

	Position* auxPosition = new Position(0, 0, 0);
	Speed* auxSpeed = new Speed(0, Vector2(0, 0));
	std::string auxName;
	std::vector<Power*> auxPowers;

	yamlNode["name"] >> auxName;
	yamlNode["position"] >> auxPosition;
	yamlNode["speed"] >> auxSpeed;

	const YAML::Node& powers = yamlNode["powers"];
	for (unsigned i = 0; i < powers.size(); i++) {
		Power* power = new Power("", 0, 0);
		powers[i] >> power;
		auxPowers.push_back(power);
	}

	personaje->setName(auxName);
	personaje->setPosition(auxPosition);
	personaje->setSpeed(auxSpeed);
	personaje->setPowers(auxPowers);
}

/**
 * Sobrecarga de operador >> para llenar los datos de un puntero
 * a Personaje.
 */
void operator >>(const YAML::Node& yamlNode, Entity* entity) {

	Position* auxPosition = new Position(0, 0, 0);
	std::string auxName;

	yamlNode["name"] >> auxName;
	yamlNode["position"] >> auxPosition;

	entity->setName(auxName);
	entity->setPos(auxPosition->getX(), auxPosition->getY());
}

/**
 * Sobrecarga de operador >> para llenar los campos de una PersonajeVista.
 */
void operator >>(const YAML::Node& yamlNode, PersonajeVista* playerView) {

	std::vector<Power*> auxPowers;
	Personaje* personaje = new Personaje("", NULL, NULL, auxPowers);
	std::string auxImageSrc;
	Vector2* auxAnchorPixel = new Vector2(0, 0);

	yamlNode["imageSrc"] >> auxImageSrc;
	yamlNode["anchorPixel"] >> auxAnchorPixel;
	yamlNode["player"] >> personaje;

	playerView->setImagePath(auxImageSrc);
	playerView->setAnchorPixel(auxAnchorPixel);
	playerView->setEntity(personaje);

}

/**
 * Sobrecarga de operador >> para llenar los campos de una EntityView.
 */
void operator >>(const YAML::Node& yamlNode, EntityView* entityView) {

	Entity* auxEntity = new Entity();
	Vector2* auxAnchorPixel = new Vector2(0, 0);
	std::vector<Power*> auxPowers;
	std::string auxImageSrc;

	yamlNode["imageSrc"] >> auxImageSrc;
	yamlNode["anchorPixel"] >> auxAnchorPixel;
	yamlNode["entity"] >> auxEntity;

	entityView->setImagePath(auxImageSrc);
	entityView->setAnchorPixel(auxAnchorPixel);
	entityView->setEntity(auxEntity);

}

/**
 * Sobrecarga de operador >> para llenar los datos de una lista de entidades.
 */
void operator >>(const YAML::Node& yamlNode,
		std::vector<PersonajeVista*>& entityList) {
	const YAML::Node& playerViews = yamlNode["playerViews"];
	for (unsigned i = 0; i < playerViews.size(); i++) {
		PersonajeVista* entityView = new PersonajeVista();
		playerViews[i] >> entityView;
		entityList.push_back(entityView);
	}
}
/**
 * Sobrecarga de operador >> para llenar los datos de una lista de entidades.
 */
void operator >>(const YAML::Node& yamlNode,
		std::vector<EntityView*>& entityList) {
	const YAML::Node& entityViews = yamlNode["entityViews"];
	for (unsigned i = 0; i < entityViews.size(); i++) {
		EntityView* entityView = new EntityView();
		entityViews[i] >> entityView;
		entityList.push_back(entityView);
	}
}

/* ******************************************************* *
 * *********** ANIMATION CONFIGURATION PARSING *********** *
 * ******************************************************* */

/**
 * Sobrecarga de operador >> para parsear un AnimationConfiguration.
 */
void operator >>(const YAML::Node& yamlNode,
		GameConfiguration* animationConfig) {
	const YAML::Node& configuration = yamlNode["gameConfiguration"];
	unsigned int auxFps, auxDelay, auxHeight, auxWidth, auxBPP;
	std::string auxGameMusicSrc, auxMenuImage, auxMenuMusic;

	configuration["fps"] >> auxFps;
	configuration["delay"] >> auxDelay;
	configuration["gameMusic"] >> auxGameMusicSrc;
	configuration["defaultScreenHeight"] >> auxHeight;
	configuration["defaultScreenWidth"] >> auxWidth;
	configuration["defaultBPP"] >> auxBPP;
	configuration["menuBackImage"] >> auxMenuImage;
	configuration["menuBackMusic"] >> auxMenuMusic;

	animationConfig->setDelay(auxDelay);
	animationConfig->setFps(auxFps);
	animationConfig->setGameMusicSrc(auxGameMusicSrc);
	animationConfig->setDefaultScreenHeight(auxHeight);
	animationConfig->setDefaultScreenWidth(auxWidth);
	animationConfig->setDefaultBPP(auxBPP);
	animationConfig->setMenuBackImageSrc(auxMenuImage);
	animationConfig->setMenuBackMusicSrc(auxMenuMusic);
}

/* *********************************************** *
 * *********** TILE DEFINITION PARSING *********** *
 * *********************************************** */

/**
 * Extraction operator for tile definition.
 */
void operator >>(const YAML::Node& yamlNode, TileDefinition* tileDefinition) {

	std::string auxId, auxSrc;

	yamlNode["identifier"] >> auxId;
	yamlNode["imageSrc"] >> auxSrc;

	tileDefinition->setTileId(auxId);
	tileDefinition->setTileImageSrc(auxSrc);
}

/**
 * Extraction of all configured entities.
 */
void operator >>(const YAML::Node& yamlNode, TextureHolder* textureHolder) {
	const YAML::Node& yamlTileDefinitions = yamlNode["tiles"];
	for (unsigned i = 0; i < yamlTileDefinitions.size(); i++) {
		TileDefinition* tileDef = new TileDefinition("", "");
		yamlTileDefinitions[i] >> tileDef;
		textureHolder->addTexture(tileDef);
	}
}

/* *********************************** *
 * *********** MAP PARSING *********** *
 * *********************************** */

/**
 * Extraction operator for map tile.
 */
void operator >>(const YAML::Node& yamlNode, Tile* tile) {
	std::string auxId;
	Position* auxPosition = new Position(0, 0, 0);

	yamlNode["position"] >> auxPosition;
	yamlNode["texture"] >> auxId;

	tile->setPosition(auxPosition);
	tile->setTextureIdentifier(auxId);
}

/**
 * Extraction operator of a map.
 */
void operator >>(const YAML::Node& yamlNode, AuxMap& destMap) {
	const YAML::Node& tileList = yamlNode["tileLocations"];
	for (unsigned i = 0; i < tileList.size(); i++) {
		Tile* tile = new Tile(NULL, "");
		tileList[i] >> tile;
		destMap.tileList.push_back(tile);
	}
}

/**
 * Extraction operator of a map.
 */
void operator >>(AuxMap& originConfig, MapData* destMap) {
	for (unsigned int i = 0; i < originConfig.tileList.size(); i++) {
		Tile* auxTile = originConfig.tileList[i];
		int auxCol = auxTile->getPosition()->getX();
		int auxRow = auxTile->getPosition()->getY();
		destMap->SetTileType(auxTile->getTextureIdentifier(), auxRow, auxCol);
	}
}

/**
 * Extraction operator of a map.
 */
void operator >>(const YAML::Node& yamlNode, AuxMapDimension& dimension) {
	const YAML::Node& yamlDimensions = yamlNode["mapDimensions"];

	yamlDimensions[0] >> dimension.nrows;
	yamlDimensions[1] >> dimension.ncols;
}

/* ************************************************ *
 * *********** ENTITY VIEW MAP CREATION *********** *
 * ************************************************ */

void loadEntityViewMap(EntityViewMap* entityViewMap,
		std::vector<PersonajeVista*> entityViewVector) {

	for (unsigned int j = 0; j < entityViewVector.size(); j++) {
		PersonajeVista* parsedEntityView = entityViewVector[j];
		Personaje* personaje = (Personaje*) parsedEntityView->getEntity();

		/**
		 * TODO: hacer conversion entre Posicion y Coordinates.
		 */
		Coordinates coordinates = Coordinates(
				personaje->GetCurrentPos()->GetX(),
				personaje->GetCurrentPos()->GetY());

		entityViewMap->positionEntityView(parsedEntityView, coordinates);
	}

}

void loadEntityViewMap(EntityViewMap* entityViewMap,
		std::vector<EntityView*> entityViewVector) {

	for (unsigned int j = 0; j < entityViewVector.size(); j++) {
		EntityView* parsedEntityView = entityViewVector[j];
		Entity* personaje = parsedEntityView->getEntity();

		/**
		 * TODO: hacer conversion entre Posicion y Coordinates.
		 */
		Coordinates coordinates = Coordinates(
				personaje->getCurrentPos()->GetX(),
				personaje->getCurrentPos()->GetY());

		entityViewMap->positionEntityView(parsedEntityView, coordinates);
	}

}

/* ********************************************* *
 * *********** CONFIGURATION LOADING *********** *
 * ********************************************* */

/**
 * Loads the configuration, prints its output and returns
 * a persistent configuration object.
 */
PersistentConfiguration ConfigurationReader::loadConfiguration(
		std::string configurationFile, std::string outputFilename) {

	/**
	 * TODO: hacer free de los structs auxiliares luego
	 * de parsear.
	 */
	std::ifstream inputFile(configurationFile.c_str(), std::ifstream::in);
	std::ofstream outputFile(outputFilename.c_str());

	// Error Check
	if (!inputFile) {
		cout << "No se encontro el archivo de configuracion" << std::endl;
		exit(1);
	}

	// Parser initialization.
	YAML::Parser parser(inputFile);
	YAML::Node yamlNode;
	parser.GetNextDocument(yamlNode);

	// Parsing PersonajeVista.
	std::vector<PersonajeVista*> playerViewVector;
	yamlNode[PLAYERVIEWS_POSITION] >> playerViewVector;

	// Parsing EntityViews.
	std::vector<EntityView*> entityViewVector;
	yamlNode[ENTITYVIEWS_POSITION] >> entityViewVector;

	// Parsing animation configuration.
	GameConfiguration* animationConfig = new GameConfiguration();
	yamlNode[GAME_CONFIGURATION_POSITION] >> animationConfig;

	// Parsing tile definition.
	TextureHolder* textureHolder = new TextureHolder();
	yamlNode[TILE_DEFINITION_POSITION] >> textureHolder;

	// Parsing map dimensions.
	AuxMap mapConfiguration;
	AuxMapDimension mapDimension;
	yamlNode[MAP_DIMENSION_POSITION] >> mapDimension;
	mapConfiguration.dimension = mapDimension;
	MapData* mapData = new MapData(mapConfiguration.dimension.nrows,
			mapConfiguration.dimension.ncols);

	// Parsing map tile locations.
	yamlNode[MAP_TILES_POSITION] >> mapConfiguration;
	mapConfiguration >> mapData;

	// Create entityViewMap:
	EntityViewMap* entityViewMap = new EntityViewMap(mapData->GetNRows(),
			mapData->GetNCols());
	loadEntityViewMap(entityViewMap, playerViewVector);
	loadEntityViewMap(entityViewMap, playerViewVector);

	// Packing parser results.
	PersistentConfiguration configuration = PersistentConfiguration();
	configuration.setEntityViewMap(entityViewMap);
	configuration.setTextureHolder(textureHolder);
	configuration.setMapData(mapData);
	configuration.setAnimationConfiguration(animationConfig);

	// Print parsed elements.
	printPlayerViews(playerViewVector, outputFile);
	printEntityViews(entityViewVector, outputFile);
	printGameConfiguration(animationConfig, outputFile);
	printTextureHolder(textureHolder, outputFile);
	printMapConfiguration(mapConfiguration, outputFile);
	printHeader("END OF PARSER");

	return configuration;
}

ConfigurationReader::ConfigurationReader() {
}

ConfigurationReader::ConfigurationReader(const ConfigurationReader& orig) {
}

ConfigurationReader::~ConfigurationReader() {
}
