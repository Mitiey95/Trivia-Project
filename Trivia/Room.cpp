#include "Room.h"

Room::Room(int id, User* admin, std::string name, int maxUsers, int questionsNo, int questionTime)
{
	_id = id;
	_admin = admin;
	_name = name;
	_maxUsers = maxUsers;
	_questionsNo = questionsNo;
	_questionTime = questionTime;
	_users.push_back(admin);
}

bool Room::joinRoom(User* user)
{
	std::string message;
	message += std::to_string(JOIN_EXIST_ROOM_ANS);

	if (_users.size() == _maxUsers) {
		message += std::to_string(1);		// ROOM IS FULL
		user->send(message);
		return false;
	}

	_users.push_back(user);

	message += std::to_string(0);		// SUCCESS
	message += Helper::getPaddedNumber(_questionsNo, 2);
	message += Helper::getPaddedNumber(_questionTime, 2);

	user->send(message);

	sendMessage(getUsersListMessage());

	return true;
}

void Room::leaveRoom(User* user)
{
	for (int i = 0; i < _users.size(); i++) {
		if (_users[i]->getUsername() == user->getUsername()) {
			_users.erase(_users.begin() + i);
		}
	}

	std::string message;
	message += std::to_string(OUT_ROOM_ANS);
	message += std::to_string(0);		// SUCCESS
	user->send(message);

	sendMessage(getUsersListMessage());
}

int Room::closeRoom(User* user)
{
	std::string message;
	message += std::to_string(CLOSE_ROOM_ANS);

	for (int i = 0; i < _users.size(); i++) {
		if (_admin->getSocket() != _users[i]->getSocket()) {
			_users[i]->clearRoom();
		}

		_users[i]->send(message);
	}

	return _id;
}

User* Room::getAdmin()
{
	return _admin;
}

std::vector<User*> Room::getUsers()
{
	return _users;
}

std::string Room::getUsersListMessage()
{
	std::string message;
	message += std::to_string(USERS_ROOM_ANS);
	message += std::to_string(_users.size());

	for (int i = 0; i < _users.size(); i++) {
		message += Helper::getPaddedNumber(_users[i]->getUsername().size(), 2);
		message += _users[i]->getUsername();
	}

	return message;
}

int Room::getQuestionsNo()
{
	return _questionsNo;
}

int Room::getId()
{
	return _id;
}

std::string Room::getName()
{
	return _name;
}

std::string Room::getUsersAsString(std::vector<User*> usersList, User* excludeUser)
{
	std::string usersString;

	if (excludeUser == nullptr) {
		for (int i = 0; i < usersList.size(); i++) {
			usersString += usersList[i]->getUsername();
		}
	}
	else {
		for (int i = 0; i < usersList.size(); i++) {
			if (usersList[i]->getSocket() != excludeUser->getSocket())
			{
				usersString += usersList[i]->getUsername();
			}
		}
	}

	return usersString;
}

void Room::sendMessage(std::string message)
{
	sendMessage(nullptr, message);
}

void Room::sendMessage(User* excludeUser, std::string message)
{
	if (excludeUser == nullptr) {
		for (int i = 0; i < _users.size(); i++) {
			 _users[i]->send(message);
		}
	}
	else {
		for (int i = 0; i < _users.size(); i++) {
			if (_users[i]->getSocket() != excludeUser->getSocket())
			{
				_users[i]->send(message);
			}
		}
	}
}


