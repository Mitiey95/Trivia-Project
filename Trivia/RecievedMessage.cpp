#include "RecievedMessage.h"

ReceivedMessage::ReceivedMessage(SOCKET socket, int msgCode)
{
	_sock = socket;
	_messageCode = msgCode;
}

ReceivedMessage::ReceivedMessage(SOCKET socket, int msgCode, std::vector<std::string> values)
{
	_sock = socket;
	_messageCode = msgCode;
	_values = values;
}

SOCKET ReceivedMessage::getSock()
{
	return _sock;
}

User* ReceivedMessage::getUser()
{
	return _user;
}

void ReceivedMessage::setUser(User* user)
{
	_user = user;
}

int ReceivedMessage::getMessageCode()
{
	return _messageCode;
}

std::vector<std::string>& ReceivedMessage::getValues()
{
	return _values;
}
