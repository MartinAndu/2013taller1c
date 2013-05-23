/*
 * Server.h
 *
 *  Created on: 26/04/2013
 *      Author: tomas
 */

#ifndef SERVER_H_
#define SERVER_H_
#include <networking/Chat/ChatServer.h>
#include <networking/PlayerEvent.h>
#include <networking/PlayerInfo.h>
#include <MultiplayerGame.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <exception>
#include <cstdlib>

#include <stdlib.h>
#include <pthread.h>

namespace std {

class Server {
public:
	Server(int port);

	void run(MultiplayerGame* game);

	void sendMap(string mapfile,int sockID);
	void sendFiles(std::vector<std::string> wBase, std::vector<std::string> woBase, int sockID);

	std::vector<std::string> listFilesInDirectory(std::string directory);
	std::vector<std::string> listFilesInDirectoryWithBase(std::string directory);

	PlayerInfo* recieveNewPlayer(int clientSocket);
	void sendAproval(int clientSocket, int result);
	int isNameAbilivable(string playerName);
	string getAbilivableName(string playerName);
	void sendNewName(int clientSocket, string newName);
	int addPlayerToGame(int clientSocket, PlayerInfo* info);

	bool exchangeAliveSignals(int clientSocket);
	void sendNewPlayers(int clientSocket, map<int,string> *sended);


	vector<PlayerEvent*> recvEvents(int clientSocket);
	void recvChatMessages(int clientSocket);

	void getPlayersUpdates();
	void sendPlayersUpdates(int clientSocket, string playerName);

	void disconectPlayer(int clientSocket, string playerName);
	Chat* recvChat(int clientSocket);
	void deliverMessages(int clientSocket);
	MultiplayerGame* getGame();

	ChatServer* getChat();
	map<string,int> getPlayerConnected();
	void setMessages(vector<ChatMessage*>);

	bool isActive();
	void setActive();
	void setInactive();

	virtual ~Server();

private:

	int reconectPlayer(int clientSocket, string playerName, PlayerInfo* info);

	int serverID;
	ChatServer* chat;
	MultiplayerGame* game;
	map<int,PlayerInfo*> gamePlayers;
	map<string,int> conectedPlayers;
	map<string,int> disconectedPlayers;
	map<string,vector<PlayerUpdate*> > updates;
	vector<ChatMessage*> messages;
	bool active;
	map<int,pthread_t> connections;

};

typedef struct aux{
	Server* server;
	int clientID;
}ThreadParameter;

} /* namespace std */
#endif /* SERVER_H_ */
