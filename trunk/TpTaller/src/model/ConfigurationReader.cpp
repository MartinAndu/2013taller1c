/* 
 * File:   ConfigurationReader.cpp
 * Author: gonchub (el niño parser)
 * 
 * Created on March 21, 2013, 9:25 PM
 */

#include <yaml-cpp/yaml.h>
#include <model/entityProperties/Speed.h>
#include <model/entityProperties/Position.h>
#include <model/entityProperties/Power.h>
#include <model/ConfigurationReader.h>
#include <model/entities/Entity.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <exception>
#include <cstdlib>
#include <stdlib.h>

using namespace std;
/**
 * YAML Configuration file position.
 */
#define CONFIGURATION_ENTITIES_DEFINITION 0
#define CONFIGURATION_TILES_DEFINITION 1
#define CONFIGURATION_MAP_DEFINITION 2

/**
 * Structure to represent a three dimensional vector.
 */
struct ThreeDimensionalVector {
	int x, y, z;
};

/**
 * Extraction operator for two dimensional vectors.
 */

/* ************************************** *
 * *********** ENTITY PARSING *********** *
 * ************************************** */

/**
 * Structure to represent the power of an entity.
 */
struct AuxPower {
	std::string name;
	int damage;
	int range;
};

/**
 * Structure to represent the power of an entity.
 */
struct AuxSpeed {
	int magnitude;
	ThreeDimensionalVector direction;
};

/**
 * Structure to represent an entity.
 */
struct AuxEntity {
	std::string name;
	ThreeDimensionalVector position;
	AuxSpeed speed;
	std::vector<AuxPower> powers;
};


void operator >>(const YAML::Node& yamlNode, ThreeDimensionalVector& vector) {
	yamlNode[0] >> vector.x;
	yamlNode[1] >> vector.y;
	yamlNode[2] >> vector.z;
}


/**
 * Structure to represent an entity.
 */
struct AuxEntityList {
	std::vector<AuxEntity> entities;
};

/**
 * Extraction operator for power.
 */
void operator >>(const YAML::Node& yamlNode, AuxPower& power) {
	yamlNode["name"] >> power.name;
	yamlNode["damage"] >> power.damage;
	yamlNode["range"] >> power.range;
}

/**
 * Extraction operator for power.
 */
void operator >>(const YAML::Node& yamlNode, AuxSpeed& speed) {
	yamlNode["magnitude"] >> speed.magnitude;
	yamlNode["direction"] >> speed.direction;
}

/**
 * Extraction of an entity.
 */
void operator >>(const YAML::Node& yamlNode, AuxEntity& entity) {
	yamlNode["name"] >> entity.name;
	yamlNode["position"] >> entity.position;
	yamlNode["speed"] >> entity.speed;
	const YAML::Node& powers = yamlNode["powers"];
	for (unsigned i = 0; i < powers.size(); i++) {
		AuxPower power;
		powers[i] >> power;
		entity.powers.push_back(power);
	}
}

/**
 * Extraction of all configured entities.
 */
void operator >>(const YAML::Node& yamlNode, AuxEntityList& entityList) {
	const YAML::Node& entities = yamlNode["entities"];
	for (unsigned i = 0; i < entities.size(); i++) {
		AuxEntity entity;
		entities[i] >> entity;
		entityList.entities.push_back(entity);
	}
}

/**
 * Prints an entity to check if it was parsed correctly.
 */
void printEntity(Entity* parsedEntity) {

	std::cout << "Name: ";
	std::cout << parsedEntity->getName() << "\n";
	std::cout << "Position: (";
	std::cout << parsedEntity->getPosition()->getX() << ", ";
	std::cout << parsedEntity->getPosition()->getY() << ", ";
	std::cout << parsedEntity->getPosition()->getZ() << ")\n";
	std::cout << "Speed:\n";
	std::cout << "      " << "Magnitude: ";
	std::cout << parsedEntity->getSpeed()->getMagnitude() << "\n";
	std::cout << "      " << "Direction: (";
	std::cout << parsedEntity->getSpeed()->getDirection()->getX() << ", ";
	std::cout << parsedEntity->getSpeed()->getDirection()->getY() << ", ";
	std::cout << parsedEntity->getSpeed()->getDirection()->getZ() << ")\n";
	std::cout << "Powers:\n";
	for (unsigned i = 0; i < parsedEntity->getPowers().size(); i++) {
		std::cout << "       " << "- Name: "
				<< parsedEntity->getPowers()[i]->getName() << "\n";
		std::cout << "       " << "  Damage: "
				<< parsedEntity->getPowers()[i]->getDamage() << "\n";
		std::cout << "       " << "  Range: "
				<< parsedEntity->getPowers()[i]->getRange() << "\n";
	}

}

/**
 * Converts an auxiliary struct entity into a model-based entity.
 */
Entity* parseEntity(AuxEntity entity) {
	Position* parsedPosition = new Position(entity.position.x,
			entity.position.y, entity.position.z);

	Speed* parsedSpeed = new Speed(entity.speed.magnitude,
			new Position(entity.speed.direction.x, entity.speed.direction.y,
					entity.speed.direction.z));

	std::vector<Power*> parsedPowers;
	for (unsigned i = 0; i < entity.powers.size(); i++) {
		Power* parsedPower = new Power(entity.powers[i].name,
				entity.powers[i].damage, entity.powers[i].range);
		parsedPowers.push_back(parsedPower);
	}

	Entity* parsedEntity = new Entity(entity.name, parsedPosition, parsedSpeed,
			parsedPowers);

	return parsedEntity;
}

/* *********************************************** *
 * *********** TILE DEFINITION PARSING *********** *
 * *********************************************** */

/**
 * Definition of a tile texture.
 */
struct AuxTileDefinition {
	std::string identifier;
	std::string imageSrc;
};

/**
 * List of all tile texture definitions.
 */
struct AuxTileDefinitionList {
	std::vector<AuxTileDefinition> tileDefinitionList;
};

/**
 * Extraction operator for tile definition.
 */
void operator >>(const YAML::Node& yamlNode,
		AuxTileDefinition& tileDefinition) {
	yamlNode["identifier"] >> tileDefinition.identifier;
	yamlNode["imageSrc"] >> tileDefinition.imageSrc;
}

/**
 * Extraction of all configured entities.
 */
void operator >>(const YAML::Node& yamlNode,
		AuxTileDefinitionList& tileDefinitionList) {
	const YAML::Node& tileDefinitions = yamlNode["tiles"];
	for (unsigned i = 0; i < tileDefinitions.size(); i++) {
		AuxTileDefinition tileDef;
		tileDefinitions[i] >> tileDef;
		tileDefinitionList.tileDefinitionList.push_back(tileDef);
	}
}

/**
 * Prints an entity to check if it was parsed correctly.
 */
void printTileDefinition(AuxTileDefinition& parsedTileDefinition) {

	std::cout << "Identifier: ";
	std::cout << parsedTileDefinition.identifier << "\n";
	std::cout << "Image Source: ";
	std::cout << parsedTileDefinition.imageSrc << "\n";

}

/**
 * Converts an auxiliary struct entity into a model-based entity.
 */
AuxTileDefinition& parseTileDefinition(AuxTileDefinition& tileDefinition) {
	return tileDefinition;
}

/* *********************************** *
 * *********** MAP PARSING *********** *
 * *********************************** */

struct AuxTile {
	ThreeDimensionalVector position;
	std::string textureIdentifier;
};

struct AuxMap {
	std::vector<AuxTile> tileList;
};

/**
 * Extraction operator for map tile.
 */
void operator >>(const YAML::Node& yamlNode, AuxTile& tile) {
	yamlNode["position"] >> tile.position;
	yamlNode["texture"] >> tile.textureIdentifier;
}

/**
 * Extraction operator of a map.
 */
void operator >>(const YAML::Node& yamlNode, AuxMap& entity) {
	const YAML::Node& tileList = yamlNode["map"];
	for (unsigned i = 0; i < tileList.size(); i++) {
		AuxTile tile;
		tileList[i] >> tile;
		entity.tileList.push_back(tile);
	}
}

/**
 * Parsing of a tile.
 */
AuxTile parseMapTile(AuxTile& tile) {
	return tile;
}

/**
 * Prints a tile to check for parsing integrity.
 */
void printTile(AuxTile& tile) {
	std::cout << "Position: ";
	std::cout << "(" << tile.position.x << ", " << tile.position.y << ", "
			<< tile.position.z << ")\n";
	std::cout << "Texture: ";
	std::cout << tile.textureIdentifier << "\n";
}

/**
 * Loads the configuration and prints its output.
 */

void ConfigurationReader::loadConfiguration(std::string configurationFile) {
	std::ifstream inputFile(configurationFile.c_str(), std::ifstream::in);

	// Error Check
	if (!inputFile) {
		cout << "No se encontro el archivo de conf\n";
		exit(1);
	}

	// Parser initialization.
	YAML::Parser parser(inputFile);
	YAML::Node yamlNode;
	parser.GetNextDocument(yamlNode);

	// Parsing entities.
	AuxEntityList entities;
	yamlNode[CONFIGURATION_ENTITIES_DEFINITION] >> entities;
	for (unsigned j = 0; j < entities.entities.size(); j++) {
		Entity* parsedEntity = parseEntity(entities.entities[j]);
		printEntity(parsedEntity);
	}

	// Parsing tile definition.
	AuxTileDefinitionList tileDefinitionList;
	yamlNode[CONFIGURATION_TILES_DEFINITION] >> tileDefinitionList;
	for (unsigned j = 0; j < tileDefinitionList.tileDefinitionList.size();
			j++) {
		AuxTileDefinition parsedTileDefinition = parseTileDefinition(
				tileDefinitionList.tileDefinitionList[j]);
		printTileDefinition(parsedTileDefinition);
	}

	// Parsing map tile locations.
	AuxMap mapConfiguration;
	yamlNode[CONFIGURATION_MAP_DEFINITION] >> mapConfiguration;
	for (unsigned j = 0; j < mapConfiguration.tileList.size(); j++) {
		AuxTile parsedTile = parseMapTile(mapConfiguration.tileList[j]);
		printTile(parsedTile);
	}

}

ConfigurationReader::ConfigurationReader() {
}

ConfigurationReader::ConfigurationReader(const ConfigurationReader& orig) {
}

ConfigurationReader::~ConfigurationReader() {
}

