#include "TriviaServer.h"

#include <exception>
#include <iostream>
#include <string>
#include <numeric>

static const unsigned short PORT = 8826;
static const unsigned int IFACE = 0;

TriviaServer::TriviaServer()
{
	DataBase _db();
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET)
	{
		throw std::exception(__FUNCTION__ " - socket");
	}
}

TriviaServer::~TriviaServer()
{
	TRACE(__FUNCTION__ " closing accepting socket");
	try
	{
		closesocket(_socket);
	}
	catch (...) {}
}

void TriviaServer::server()
{
	bindAndListen();

	std::thread thread(&TriviaServer::handleRecievedMessages, this);
	thread.detach();

	while (true)
	{
		TRACE("accepting client...");
		accept();
	}
}

void TriviaServer::bindAndListen()
{
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(PORT);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = IFACE;

	if (::bind(_socket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		throw std::exception(__FUNCTION__ " - bind");
	}
	TRACE("binded");

	if (::listen(_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		throw std::exception(__FUNCTION__ " - listen");
	}
	TRACE("listening...");
}

void TriviaServer::accept()
{
	SOCKET client_socket = ::accept(_socket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
	{
		throw std::exception(__FUNCTION__);
	}

	TRACE("Client accepted !");

	std::thread thread(&TriviaServer::clientHandler, this, client_socket);
	thread.detach();
}

void TriviaServer::clientHandler(SOCKET client_socket)
{
	ReceivedMessage* currRcvMsg = nullptr;
	try
	{
		int msgCode = Helper::getMessageTypeCode(client_socket);

		while (msgCode != EXIT_ASK)
		{
			currRcvMsg = buildRecieveMessage(client_socket, msgCode);
			addRecievedMessage(currRcvMsg);

			msgCode = Helper::getMessageTypeCode(client_socket);
		}

		currRcvMsg = buildRecieveMessage(client_socket, EXIT_ASK);
		addRecievedMessage(currRcvMsg);
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception was catch in function clientHandler. socket=" << client_socket << ", what=" << e.what() << std::endl;
		currRcvMsg = buildRecieveMessage(client_socket, SIGN_OUT_ASK);
		addRecievedMessage(currRcvMsg);
	}
	closesocket(client_socket);
}

void TriviaServer::safeDeleteUser(ReceivedMessage* receivedMessage)
{
	try
	{
		handlelSignout(receivedMessage);
		closesocket(receivedMessage->getSock());
	}
	catch (...) {}
}

User* TriviaServer::handlelSignin(ReceivedMessage* receivedMessage)
{
	int flag = 0;		//SUCCESS

	User* user = new User(receivedMessage->getValues()[0], receivedMessage->getSock());

	if (!_db.isUserAndPassMatch(receivedMessage->getValues()[0], receivedMessage->getValues()[1])) {
		flag = 1;		//WRONG_DETAILS
		Helper::sendData(receivedMessage->getSock(), std::to_string(SIGN_IN_ANS) + std::to_string(flag));
	}
	else if (getUserByName(receivedMessage->getValues()[0]) != nullptr)
	{
		flag = 2;		//ALREADY_CONNECTED
		Helper::sendData(receivedMessage->getSock(), std::to_string(SIGN_IN_ANS) + std::to_string(flag));
	}
	else {
		_connectedUsers.insert(std::pair<SOCKET, User*>(receivedMessage->getSock(), user));
		Helper::sendData(receivedMessage->getSock(), std::to_string(SIGN_IN_ANS) + std::to_string(flag));
	}

	return user;
}

bool TriviaServer::handlelSignup(ReceivedMessage* receivedMessage)
{
	int flag = 0;		//SUCCESS
	User* user = new User(receivedMessage->getValues()[0], receivedMessage->getSock());

	if (!Validator::isPasswordValid(receivedMessage->getValues()[1])) {
		flag = 1;		//PASS_ILLEGAL
		Helper::sendData(receivedMessage->getSock(), std::to_string(SIGN_UP_ANS) + std::to_string(flag));
		return false;
	}
	else if (!Validator::isUsernameValid(receivedMessage->getValues()[0])) {
		flag = 3;		//USERNAME_ILLEGAL
		Helper::sendData(receivedMessage->getSock(), std::to_string(SIGN_UP_ANS) + std::to_string(flag));
		return false;
	}
	else if (_db.isUserExists(receivedMessage->getValues()[0])) {
		flag = 2;		//USER_IS_ALREADY_EXISTS
		Helper::sendData(receivedMessage->getSock(), std::to_string(SIGN_UP_ANS) + std::to_string(flag));
		return false;
	}
	else {
		_db.addNewUser(receivedMessage->getValues()[0], receivedMessage->getValues()[1], receivedMessage->getValues()[2]);
		Helper::sendData(receivedMessage->getSock(), std::to_string(SIGN_UP_ANS) + std::to_string(flag));
	}

	return true;
}

void TriviaServer::handlelSignout(ReceivedMessage* receivedMessage)
{
	auto it = _connectedUsers.find(receivedMessage->getSock());
	if (it != _connectedUsers.end()) {
		_connectedUsers.erase(it);
	}
}

void TriviaServer::handleLeaveGame(ReceivedMessage* receivedMessage)
{
	User* user = receivedMessage->getUser();
	if (user != nullptr) {
		user->leaveGame();
		user->send(std::to_string(OUT_GAME_ANS));
	}
}

void TriviaServer::handleStartGame(ReceivedMessage* receivedMessage)
{
	User* user = receivedMessage->getUser();
	if (user != nullptr) {
		Game* game = new Game(user->getRoom()->getUsers(), user->getRoom()->getQuestionsNo(), _db);
		
		auto it = _roomsList.find(user->getRoom()->getId());
		_roomsList.erase(it);

		int users_size = user->getRoom()->getUsers().size();
		std::vector<User*> _users = user->getRoom()->getUsers();

		for (int i = 0; i < users_size; i++) {
			_users[i]->clearRoom();
		}

		game->sendFirstQuestion();
	}
	
}

void TriviaServer::handlePlayerAnswer(ReceivedMessage* receivedMessage)
{
	User* user = getUserBySocket(receivedMessage->getSock());

	if (receivedMessage->getUser()->getGame() != nullptr) {
		receivedMessage->getUser()->getGame()->handleAnswerFromUser(user, atoi(receivedMessage->getValues()[0].c_str()) - 1, atoi(receivedMessage->getValues()[1].c_str()));
	}
}

bool TriviaServer::handleCreateRoom(ReceivedMessage* receivedMessage)
{
	User* admin = receivedMessage->getUser();
	if (admin != nullptr) {
		int roomId = _roomsList.size();

		bool flag = admin->createRoom(roomId,
			receivedMessage->getValues()[0],
			atoi(receivedMessage->getValues()[1].c_str()),
			atoi(receivedMessage->getValues()[2].c_str()),
			atoi(receivedMessage->getValues()[3].c_str())
			);

		if (flag) {
			_roomsList.insert(std::pair<int, Room*>(roomId, admin->getRoom()));
			return true;
		}
	}
	return false;
}

bool TriviaServer::handleCloseRoom(ReceivedMessage* receivedMessage)
{
	User* admin = receivedMessage->getUser();
	if (admin != nullptr) {
		int roomId = admin->closeRoom();

		if (roomId != -1) {
			auto it = _roomsList.find(roomId);
			_roomsList.erase(it, _roomsList.end());
			return true;
		}
	}
	return false;
}

bool TriviaServer::handleJoinRoom(ReceivedMessage* receivedMessage)
{
	bool flag = false;
	User* user = receivedMessage->getUser();

	if (user != nullptr) {
		Room* room = getRoomById(atoi(receivedMessage->getValues()[0].c_str()));

		if (room != nullptr) {
			flag = user->joinRoom(room);
		}
		
		if (flag) {
			return true;
		}
		else {
			std::string message;
			message += std::to_string(JOIN_EXIST_ROOM_ANS);
			message += std::to_string(2);		// ROOM_NOT_EXIST
			user->send(message);
		}
	}
	return false;
}

bool TriviaServer::handleLeaveRoom(ReceivedMessage* receivedMessage)
{
	User* user = receivedMessage->getUser();
	if (user != nullptr) {
		Room* room = user->getRoom();
		
		if (room != nullptr) {
			user->leaveRoom();
			return true;
		}
	}
	return false;
}

void TriviaServer::handleGetUsersInRoom(ReceivedMessage* receivedMessage)
{
	User* user = receivedMessage->getUser();

	if (user != nullptr) {
		Room* room = getRoomById(atoi(receivedMessage->getValues()[0].c_str()));

		if (room != nullptr) {
			std::string msg = room->getUsersListMessage();
			user->send(msg);
		}
		else {
			std::string message;
			message += std::to_string(USERS_ROOM_ANS);
			message += std::to_string(0);		// ROOM_NOT_EXIST
			user->send(message);
		}
	}
	
}

void TriviaServer::handleGetRooms(ReceivedMessage* receivedMessage)
{
	User* user = receivedMessage->getUser();

	if (user != nullptr) {
		std::string message;
		message += std::to_string(ROOM_LIST_ANS);
		message += Helper::getPaddedNumber(_roomsList.size(), 4);

		for (auto it = _roomsList.begin(); it != _roomsList.end(); ++it) {
			message += Helper::getPaddedNumber(it->first, 4);
			message += Helper::getPaddedNumber(it->second->getName().length(), 2);
			message += it->second->getName();
		}
		
		user->send(message);
	}
}

void TriviaServer::handleGetBestScores(ReceivedMessage* receivedMessage)
{
	std::vector<std::string> results = _db.getBestScores();
	std::string message;
	message += std::to_string(BEST_SCORE_ANS);

	if (results.size() == 0) {
		message += "01-000000";
		message += "01-000000";
		message += "01-000000";
	}
	else if (results.size() == 2) {
		message += Helper::getPaddedNumber(results[0].size(), 2);
		message += results[0];
		message += Helper::getPaddedNumber(atoi(results[1].c_str()), 6);

		message += "01-000000";
		message += "01-000000";
	}
	else if (results.size() == 4) {
		message += Helper::getPaddedNumber(results[0].size(), 2);
		message += results[0];
		message += Helper::getPaddedNumber(atoi(results[1].c_str()), 6);

		message += Helper::getPaddedNumber(results[2].size(), 2);
		message += results[2];
		message += Helper::getPaddedNumber(atoi(results[3].c_str()), 6);

		message += "01-000000";
	}
	else {
		for (int i = 0; i < 6; i += 2) {
			message += Helper::getPaddedNumber(results[i].size(), 2);
			message += results[i];
			message += Helper::getPaddedNumber(atoi(results[i + 1].c_str()), 6);
		}
	}

	receivedMessage->getUser()->send(message);
}

void TriviaServer::handleGetPersonalStatus(ReceivedMessage* receivedMessage)
{
	std::vector<std::string> personalStatus = _db.getPersonalStatus(receivedMessage->getUser()->getUsername());
	std::string message;
	message += std::to_string(PROFILE_ANS);

	message += Helper::getPaddedNumber(atoi(personalStatus[1].c_str()), 4);
	message += Helper::getPaddedNumber(atoi(personalStatus[2].c_str()), 6);
	message += Helper::getPaddedNumber(atoi(personalStatus[3].c_str()), 6);
	message += Helper::getPaddedNumber(atoi(personalStatus[4].c_str()), 4);

	receivedMessage->getUser()->send(message);
}

void TriviaServer::handleRecievedMessages()
{
	int msgCode = 0;
	SOCKET clientSock = 0;

	while (true)
	{
		try
		{
			std::unique_lock<std::mutex> lck(_mtxReceivedMessages);

			if (_queRcvMessage.empty())
			{
				_msgQueueCondition.wait(lck);
			}
			
			ReceivedMessage* currMessage = _queRcvMessage.front();
			_queRcvMessage.pop();
			lck.unlock();

			clientSock = currMessage->getSock();
			currMessage->setUser(getUserBySocket(currMessage->getSock()));
			msgCode = currMessage->getMessageCode();
			
			if (msgCode ==  EXIT_ASK) {
				safeDeleteUser(currMessage);
			}
			else if (msgCode == SIGN_IN_ASK) {
				currMessage->setUser(handlelSignin(currMessage));
			}
			else if (msgCode == SIGN_UP_ASK) {
				handlelSignup(currMessage);
			}
			else if (msgCode == SIGN_OUT_ASK){
				handlelSignout(currMessage);
			}
			else if (msgCode == CREATE_ROOM_ASK) {
				handleCreateRoom(currMessage);
			}
			else if (msgCode == CLOSE_ROOM_ASK) {
				handleCloseRoom(currMessage);
			}
			else if (msgCode == JOIN_EXIST_ROOM_ASK) {
				handleJoinRoom(currMessage);
			}
			else if (msgCode == OUT_ROOM_ASK) {
				handleLeaveRoom(currMessage);
			}
			else if (msgCode == USERS_ROOM_ASK) {
				handleGetUsersInRoom(currMessage);
			}
			else if (msgCode == ROOM_LIST_ASK) {
				handleGetRooms(currMessage);
			}
			else if (msgCode == START_GAME_ASK) {
				handleStartGame(currMessage);
			}
			else if (msgCode == OUT_GAME_ASK) {
				handleLeaveGame(currMessage);
			}
			else if (msgCode == SEND_ANSWER_ASK) {
				handlePlayerAnswer(currMessage);
			}
			else if (msgCode == BEST_SCORE_ASK) {
				handleGetBestScores(currMessage);
			}
			else if (msgCode == PROFILE_ASK) {
				handleGetPersonalStatus(currMessage);
			}
			else if (msgCode == PRINT_SERVER)
			{
				std::cout << "Hello World !!!" << std::endl;
			}
			else {
				safeDeleteUser(currMessage);
			}

			delete currMessage;
		}
		catch (...)
		{
			std::cout << "Error ..." << std::endl;
		}
	}
}

void TriviaServer::addRecievedMessage(ReceivedMessage* msg)
{
	std::lock_guard<std::mutex> lockGuard(_mtxReceivedMessages);
	_queRcvMessage.push(msg);
	_msgQueueCondition.notify_all();
}

ReceivedMessage* TriviaServer::buildRecieveMessage(SOCKET client_socket, int msgCode)
{
	std::vector<std::string> values;
	ReceivedMessage* msg = nullptr;

	if (msgCode == SIGN_IN_ASK) {

		int usernameSize = Helper::getIntPartFromSocket(client_socket, 2);
		std::string username = Helper::getStringPartFromSocket(client_socket, usernameSize);
		values.push_back(username);

		int passwordSize = Helper::getIntPartFromSocket(client_socket, 2);
		std::string password = Helper::getStringPartFromSocket(client_socket, passwordSize);
		values.push_back(password);

		msg = new ReceivedMessage(client_socket, msgCode, values);
	}
	else if (msgCode == SIGN_UP_ASK){
		int usernameSize = Helper::getIntPartFromSocket(client_socket, 2);
		std::string username = Helper::getStringPartFromSocket(client_socket, usernameSize);
		values.push_back(username);

		int passwordSize = Helper::getIntPartFromSocket(client_socket, 2);
		std::string password = Helper::getStringPartFromSocket(client_socket, passwordSize);
		values.push_back(password);

		int emailSize = Helper::getIntPartFromSocket(client_socket, 2);
		std::string email = Helper::getStringPartFromSocket(client_socket, emailSize);
		values.push_back(email);

		msg = new ReceivedMessage(client_socket, msgCode, values);
	}
	else if (msgCode == SIGN_OUT_ASK){
		msg = new ReceivedMessage(client_socket, msgCode, values);
	}
	else if (msgCode == CREATE_ROOM_ASK) {
		int nameSize = Helper::getIntPartFromSocket(client_socket, 2);
		std::string name = Helper::getStringPartFromSocket(client_socket, nameSize);
		values.push_back(name);

		std::string playersNumber = Helper::getStringPartFromSocket(client_socket, 1);
		values.push_back(playersNumber);

		std::string questionNumber = Helper::getStringPartFromSocket(client_socket, 2);
		values.push_back(questionNumber);

		std::string questionTimeInSec = Helper::getStringPartFromSocket(client_socket, 2);
		values.push_back(questionTimeInSec);

		msg = new ReceivedMessage(client_socket, msgCode, values);
	}
	else if (msgCode == CLOSE_ROOM_ASK) {
		msg = new ReceivedMessage(client_socket, msgCode);
	}
	else if (msgCode == JOIN_EXIST_ROOM_ASK) {
		std::string roomId = Helper::getStringPartFromSocket(client_socket, 4);
		values.push_back(roomId);

		msg = new ReceivedMessage(client_socket, msgCode, values);
	}
	else if (msgCode == OUT_ROOM_ASK) {
		msg = new ReceivedMessage(client_socket, msgCode);
	}
	else if (msgCode == USERS_ROOM_ASK) {
		std::string roomId = Helper::getStringPartFromSocket(client_socket, 4);
		values.push_back(roomId);

		msg = new ReceivedMessage(client_socket, msgCode, values);
	}
	else if (msgCode == ROOM_LIST_ASK) {
		msg = new ReceivedMessage(client_socket, msgCode);
	}
	else if (msgCode == START_GAME_ASK) {
		msg = new ReceivedMessage(client_socket, msgCode);
	}
	else if (msgCode == OUT_GAME_ASK) {
		msg = new ReceivedMessage(client_socket, msgCode);
	}
	else if (msgCode == SEND_ANSWER_ASK) {
		std::string answerNumber = Helper::getStringPartFromSocket(client_socket, 1);
		if (answerNumber == "") {
			answerNumber = "5";
		}
		values.push_back(answerNumber);

		std::string timeInSeconds = Helper::getStringPartFromSocket(client_socket, 2);
		values.push_back(timeInSeconds);

		msg = new ReceivedMessage(client_socket, msgCode, values);
	}
	else if (msgCode == BEST_SCORE_ASK) {
		msg = new ReceivedMessage(client_socket, msgCode);
	}
	else if(msgCode == PROFILE_ASK)
	{
		msg = new ReceivedMessage(client_socket, msgCode);
	}
	else {
		msg = new ReceivedMessage(client_socket, msgCode);
	}

	return msg;
}

User* TriviaServer::getUserByName(std::string name)
{
	for (auto it = _connectedUsers.begin(); it != _connectedUsers.end(); ++it)
	{
		if (it->second->getUsername() == name) {
			return it->second;
		}
	}
	return nullptr;
}

User* TriviaServer::getUserBySocket(SOCKET socket)
{
	auto it = _connectedUsers.find(socket);
	if (it != _connectedUsers.end()) {
		return it->second;
	}
	return nullptr;
}

Room* TriviaServer::getRoomById(int id)
{
	auto it = _roomsList.find(id);
	if (it != _roomsList.end()) {
		return it->second;
	}
	return nullptr;
}
