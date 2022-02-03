#pragma once

#include "RecievedMessage.h"
#include "User.h"
#include "Room.h"
#include "DataBase.h"
#include "Helper.h"
#include "Validator.h"

#include <deque>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <WinSock2.h>
#include <Windows.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>

#define MAX_LEN 1024

class TriviaServer
{
public:
	TriviaServer();
	~TriviaServer();
	void server();


private:
	//	Methods
	void bindAndListen();
	void accept();
	void clientHandler(SOCKET client_socket);
	
	void safeDeleteUser(ReceivedMessage* receivedMessage);

	User* handlelSignin(ReceivedMessage* receivedMessage);
	bool handlelSignup(ReceivedMessage* receivedMessage);
	void handlelSignout(ReceivedMessage* receivedMessage);
	
	void handleLeaveGame(ReceivedMessage* receivedMessage);
	void handleStartGame(ReceivedMessage* receivedMessage);
	void handlePlayerAnswer(ReceivedMessage* receivedMessage);
	
	bool handleCreateRoom(ReceivedMessage* receivedMessage);
	bool handleCloseRoom(ReceivedMessage* receivedMessage);
	bool handleJoinRoom(ReceivedMessage* receivedMessage);
	bool handleLeaveRoom(ReceivedMessage* receivedMessage);
	void handleGetUsersInRoom(ReceivedMessage* receivedMessage);
	void handleGetRooms(ReceivedMessage* receivedMessage);
	
	void handleGetBestScores(ReceivedMessage* receivedMessage);
	void handleGetPersonalStatus(ReceivedMessage* receivedMessage);
	
	void handleRecievedMessages();
	void addRecievedMessage(ReceivedMessage* msg);
	ReceivedMessage* buildRecieveMessage(SOCKET client_socket, int msgCode);

	User* getUserByName(std::string name);
	User* getUserBySocket(SOCKET socket);
	Room* getRoomById(int id);


	// Fildes
	SOCKET _socket;
	std::map<SOCKET, User*> _connectedUsers;
	DataBase _db;
	std::map<int, Room*> _roomsList;
	std::mutex _mtxReceivedMessages;
	std::queue<ReceivedMessage*> _queRcvMessage;

	//
	std::condition_variable _msgQueueCondition;
	//

	static int _roomIdSequence;
};

