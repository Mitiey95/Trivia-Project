#pragma once

#include "Game.h"
#include "Room.h"
#include "Helper.h"
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <algorithm>

class Room;
class Game;
class User {
public:
	User(std::string username, SOCKET sock);
	void send(std::string message);
	std::string getUsername();
	SOCKET getSocket();
	Room* getRoom();
	Game* getGame();
	void setGame(Game* gm);
	void clearRoom();
	bool createRoom(int roomId, std::string roomName, int maxUsers, int questionsNo, int questionTime);
	bool joinRoom(Room* newRoom);
	void leaveRoom();
	int closeRoom();
	bool leaveGame();

	int getRightAns();
	int getWrongAns();
	int getTimeAns();

	void setRightAns(int rightAns);
	void setWrongAns(int wrongAns);
	void setTimeAns(int timeAns);

private:
	std::string _username;
	Room* _currRoom;
	Game* _currGame;
	SOCKET _sock;
	
	int _rightAns;
	int _wrongAns;
	int _timeAns;

};
