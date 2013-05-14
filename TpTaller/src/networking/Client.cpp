/*
 * Client.cpp
 *
 *  Created on: 25/04/2013
 *      Author: tomas
 */

#include <networking/Client.h>
#include <networking/ComunicationUtils.h>
#include <model/map/TextureDefinition.h>
#include <model/map/TextureHolder.h>
#include <model/Logs/Logs.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <libgen.h>

#include <iostream>
#include <sstream>
#include <fstream>

#define MAPFILE "./mapita.yaml"
#define READING_SIZE 4096
#define ALIVE_SIGNAL "ALIVE"
#define OK 0
#define ERROR -1

using namespace std;

/* ***************************************************************** */
/* *************  FUNCIONES EJECUTADAS EN LOS THREADS ************** */
/* ***************************************************************** */

void* transmit(void* _client){

	Client* client = (Client*) _client;

	//client->downloadFile();
	bool playing = true;

	client->registerPlayer();

	int ok = client->getServerAproval();
	if (ok != 0){
		client->recvNewName();
	}

	client->addLocalPlayer();

	while (playing){

		playing = client->exchangeAliveSignals();
		if (!playing) break;

		client->checkNewPlayers();
		client->sendEvents();
		map<string,PlayerUpdate*> updates = client->recvPlayersUpdates();
		if (!updates.empty()) client->updatePlayers(updates);

	}


	return NULL;
}


/* ***************************************************************** */
/* *************************  CLASE CLIENT ************************* */
/* ***************************************************************** */

Client::Client(string host, int port, Game* game) {
	// TODO Auto-generated constructor stub
	struct sockaddr_in hints;
	struct hostent *server;

	//Creo el nuevo socket
	clientID = socket(AF_INET, SOCK_STREAM, 0);
	if (clientID < 0){
		Logs::logErrorMessage("Cliente: El cliente no se ha podido inicializar");
		exit(1);
	}


	// Obtengo el host del servidor
	server = gethostbyname(host.c_str());
	if (server == NULL) {
		Logs::logErrorMessage("Cliente: No se ha podido obtener el host del servidor");
		exit(1);
	}

	// Seteo las cosas necesarias para conectarme al servidor
	memset(&hints, 0, sizeof hints);
	hints.sin_family = AF_INET;
	if(inet_pton(AF_INET, host.c_str(), &hints.sin_addr)<=0)
	{
		Logs::logErrorMessage("Cliente: Error al obtener la direccion IP del servidor");
		exit(1);
	}
	hints.sin_port = htons(port);

	// Conecto al servidor utilizando el socket creado.
	if (connect(clientID,(struct sockaddr *) &hints,sizeof(hints)) < 0){
		Logs::logErrorMessage("Cliente: Ha ocurrido un error conectandose al servidor");
	}

	// Devuelvo el socket ID.
	this->game = game;

}


/* ******************** CLIENT SET PLAYER INFO ********************* */

void Client::initPlayerInfo(PlayerView* view){
	this->info = new PlayerInfo();
	info->setName(view->getPersonaje()->getName());
	info->setWalkingImageSrc(view->getTextureHolder()->getTextureSrc(view->getName() + string(WALKING_MODIFIER) ));
	info->setRunningImageSrc(view->getTextureHolder()->getTextureSrc(view->getName() + string(RUNNING_MODIFIER) ));
	info->setIdleImageSrc(view->getTextureHolder()->getTextureSrc(view->getName() + string(IDLE_MODIFIER) ));
	info->setAttackImageSrc(view->getTextureHolder()->getTextureSrc(view->getName() + string(ATTACK_MODIFIER) ));
	info->setIdleBlockingImageSrc(view->getTextureHolder()->getTextureSrc(view->getName() + string(IDLE_BLOCKING_MODIFIER) ));
	info->setAnchorPixel(view->getAnchorPixel());
	info->setDelay(view->getDelay());
	info->setFPS(view->getFps());
	info->setImageDimentions(view->getImageWidth(), view->getImageHeight());
	info->setPlayer(view->getPersonaje());
	Coordinates* c = new Coordinates(view->getPersonaje()->getCoordinates()->getRow(), view->getPersonaje()->getCoordinates()->getCol());
	info->setInitCoordinates(c);

	this->view = view;
	this->player = view->getPersonaje();
}


/* **************************** CLIENT RUN ************************** */

void Client::run(){

	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	if (pthread_create(&thread, &attr, transmit,(void*)this ) != 0) {
		Logs::logErrorMessage("Cliente: Error al inicializar transmit thread");
		exit(0);
	}

}


/* *************  FUNCIONES DE RECEPCION DE ARCHIVOS *************** */

void Client::downloadMap(){

	char* line = (char*) malloc(READING_SIZE*sizeof(char));
	// NULL CONTROL TODO

	line[0] = 'x';

	printf("Downloading map...\n");

	std::ofstream f;
	f.open(MAPFILE);

	//NULL CONTROL TODO

	while (line[0] != EOF){

		recv(clientID,line,READING_SIZE,0);
		if (line[0] != EOF) f << line;

	}

	f.close();

}

void Client::downloadFile() {

	// Read Picture Size
	int size;
	read(clientID, &size, sizeof(int));

	// Read filename size
	int filenameSize;
	read(clientID, &filenameSize, sizeof(int));

	// Read picture file name
	char fileName[filenameSize];
	read(clientID, fileName, filenameSize * sizeof(char));

	char* fileBaseDir = strdup(fileName);
	char* fileBaseName = strdup(fileName);

	// Read Picture Byte Array
	char p_array[size];
	read(clientID, p_array, size);

	// Convert it Back into Picture
	FILE *image;

	string dirName = string(dirname(fileBaseDir));
	string makeDir = string("mkdir -p ");
	system(string(makeDir + dirName).c_str());

	string outputFile(fileName);

	image = fopen(outputFile.c_str(), "w");
	fwrite(p_array, 1, sizeof(p_array), image);
	fclose(image);

}


/* *********************  ENVIO DEL NUEVO JUGADOR ****************** */

void Client::registerPlayer(){
	ComunicationUtils::sendPlayerInfo(clientID,info);
}

int Client::getServerAproval(){
	return ComunicationUtils::recvNumber(clientID);
}

void Client::recvNewName(){
	string newName = ComunicationUtils::recvString(clientID);
	this->player->setName(newName);
	this->view->setShowableName(newName);
	this->info->setName(newName);
	this->game->getChat()->assignPlayer(newName);
}

void Client::addLocalPlayer(){
	this->players.insert(pair<string,Player*>(player->getName(), player));
}


/* *********************** CLIENT MAIN LOOP ************************ */

bool Client::exchangeAliveSignals(){

	ComunicationUtils::sendString(clientID, ALIVE_SIGNAL);
	string signal = ComunicationUtils::recvString(clientID);
	if (signal.compare(ALIVE_SIGNAL) == 0) return true;


	Logs::logErrorMessage("Client: Ha fallado la conexion con el servidor");
	cout << "Client: Ha fallado la conexion con el servidor" << endl;
	return false;

}

void Client::checkNewPlayers(){

	// 1ro recibo la cantidad de players nuevos que hay
	int n = ComunicationUtils::recvNumber(clientID);
	// No hay nuevos jugadores
	if (n <= 0) return;

	for (int i = 0; i < n ; i++){

		// SI NO HA SIDO ENVIADO, LO ENVIO
		PlayerInfo* info = ComunicationUtils::recvPlayerInfo(clientID);
		string playerName = info->getPlayer()->getName();

		cout << "CANT: " << players.count(playerName) << " PLAYER " << playerName << endl;

		for (map<string, Player*>::iterator it = players.begin() ; it != players.end() ; ++it){
			cout << "REGISTRO: " << it->first << endl;
		}

		if (players.count(playerName) != 0) return;

		players.insert( pair<string,Player*>(info->getPlayer()->getName(), info->getPlayer()) );
		cout << info->getPlayer()->getName() << " has conected..." << endl;

		// Creo la playerView y la registro en el game.
		PlayerView* view = info->createPlayerView();
		Player* player = info->getPlayer();
		game->addNewPlayer(player,view, info->getInitCoordinates());


	}

}

void Client::sendEvents(){
	list<PlayerEvent*> events = game->getEvents();

	// 1ro envio la cantidad de events que voy a mandar
	ComunicationUtils::sendNumber(clientID,events.size());

	for (list<PlayerEvent*>::iterator it = events.begin() ; it != events.end() ; ++it ){
		ComunicationUtils::sendPlayerEvent(clientID,*it);
	}

	game->cleanEvents();

}

map<string,PlayerUpdate*> Client::recvPlayersUpdates(){

	map<string,PlayerUpdate*> updates;

	int nUpdates = ComunicationUtils::recvNumber(clientID);
	if (nUpdates <= 0) return updates;

	for (int i = 0 ; i < nUpdates ; i++){

		PlayerUpdate* update = ComunicationUtils::recvPlayerUpdate(clientID);
		string name = update->getName();
		updates[name] = update;

	}

	return updates;

}

void Client::updatePlayers(map<string,PlayerUpdate*> updates){

	for (map<string,PlayerUpdate*>::iterator it = updates.begin() ; it != updates.end() ; ++it){
		if (players.count(it->first) != 0){
			players[it->first]->update(it->second);
			players[it->first]->update();
		}
		delete it->second;
	}

}


/* ************************ CLIENT GETTERS ************************* */

Game* Client::getGame(){
	return game;
}


/* ********************** CLIENT DESTRUCTOR ************************ */

Client::~Client() {
	close(clientID);
}
