/*
 * file: main.cpp
 *
 *  Created on: 22/03/2013
 *  Project: TpTaller
 *
 */

#include <cstdlib>
#include <string.h>
#include <stdio.h>
#include <model/ConfigurationReader.h>
#include <Game.h>
#include <Menu.h>

#define CONFIGURATION_FILE "./configuration/entities.yaml"

using namespace std;

/*
 *
 */
int main(int argc, char** argv) {
	// Lectura del archivo de configuracion
	ConfigurationReader* cfgReader = new ConfigurationReader();
//	cfgReader->loadConfiguration(CONFIGURATION_FILE);

	Menu* menu = new Menu();
	menu->init();
	Game* game = new Game(cfgReader);

	delete cfgReader;

	MenuEvent event;

	while(event != EXIT_EVENT){

		event = menu->run();
		switch (event){
		case NOTHING_EVENT:
			break;
		case EXIT_EVENT:
			break;
		case NEWGAME_EVENT:
			//Aca inicio el juego
			game->run();
			break;
		case CONFIG_EVENT:
			//Aca inicio el menu de configuraciones
			cout << "Se inicio el menu de configuraciones" << endl;
			break;

		}


	}

	delete game;
	delete menu;



	return 0;
}


