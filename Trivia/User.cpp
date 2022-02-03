#include "User.h"

User::User(std::string username, SOCKET sock)
{
	_username = username;
	_sock = sock;
	_currGame = nullptr;
	_currRoom = nullptr;

	_rightAns = 0;
	_wrongAns = 0;
	_timeAns = 0;
}

void User::send(std::string message)
{
	Helper::sendData(_sock, message);
}

std::string User::getUsername()
{
	return _username;
}

SOCKET User::getSocket()
{
	return _sock;
}

Room* User::getRoom()
{
	return _currRoom;
}

Game* User::getGame()
{
	return _currGame;
}

void User::setGame(Game* gm)
{
	_currGame = gm;
}

void User::clearRoom()
{
	_currRoom = nullptr;
}

bool User::createRoom(int roomId, std::string roomName, int maxUsers, int questionsNo, int questionTime)
{
	std::string message;
	message += std::to_string(CREATE_ROOM_ANS);
	
	if (_currRoom != nullptr) {
		for (int i = 0; i < _currRoom->getUsers().size(); i++) {
			if (_currRoom->getUsers()[i] == this) {
				message += std::to_string(1);		//FAIL
				this->send(message);

				return false;
			}
		}
	}
	
	_currRoom = new Room(roomId, this, roomName, maxUsers, questionsNo, questionTime);
	message += std::to_string(0);		//SUCCESS
	message += Helper::getPaddedNumber(roomId, 4);		// ID OF NEW ROOM
	this->send(message);

	return true;
}

bool User::joinRoom(Room* newRoom)
{
	if (_currRoom == newRoom) {
		return false;
	}

	_currRoom = newRoom;
	return newRoom->Room::joinRoom(this);
}

void User::leaveRoom()
{
	if (_currRoom != nullptr) {
		_currRoom->leaveRoom(this);
		_currRoom = nullptr;
	}
}

int User::closeRoom()
{
	if (_currRoom != nullptr) {
		int id = _currRoom->closeRoom(this);

		delete _currRoom;
		_currRoom = nullptr;

		return id;
	}

	return -1;
}

bool User::leaveGame()
{
	if (_currGame != nullptr) {
		_rightAns = 0;
		_wrongAns = 0;
		_timeAns = 0;

		_currGame->leaveGame(this);
		_currGame = nullptr;
		return true;
	}
	return false;
}

int User::getRightAns()
{
	return _rightAns;
}

int User::getWrongAns()
{
	return _wrongAns;
}

int User::getTimeAns()
{
	return _timeAns;
}

void User::setRightAns(int rightAns)
{
	_rightAns = rightAns;
}

void User::setWrongAns(int wrongAns)
{
	_wrongAns = wrongAns;
}

void User::setTimeAns(int timeAns)
{
	_timeAns = timeAns;
}
