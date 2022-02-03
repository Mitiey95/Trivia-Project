#pragma once

#include "User.h"
#include <iostream>
#include <string>
#include <vector>

class User;
class Room {
public:
	
	Room(int id, User* admin, std::string name, int maxUsers, int questionsNo, int questionTime);
	bool joinRoom(User* user);
	void leaveRoom(User* user);
	int closeRoom(User* user);
	User* getAdmin();
	std::vector<User*> getUsers();
	std::string getUsersListMessage();
	int getQuestionsNo();
	int getId();
	std::string getName();

private:
	std::vector<User*> _users;
	User* _admin;
	int _maxUsers;
	int _questionTime;
	int _questionsNo;
	std::string _name;
	int _id;

	std::string getUsersAsString(std::vector<User*> usersList, User* excludeUser);
	void sendMessage(std::string message);
	void sendMessage(User* excludeUser, std::string message);
	
};
