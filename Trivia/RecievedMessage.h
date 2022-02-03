#pragma once

#include "User.h"
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <vector>

class ReceivedMessage {
public:
	ReceivedMessage(SOCKET socket, int msgCode);
	ReceivedMessage(SOCKET socket, int msgCode, std::vector<std::string> values);
	SOCKET getSock();
	User* getUser();
	void setUser(User* user);
	int getMessageCode();
	std::vector<std::string>& getValues();

private:
	SOCKET _sock;
	User* _user;
	int _messageCode;
	std::vector<std::string> _values;

};
