#pragma once

#include <iostream>
#include <string>

class Validator {
public:
	static bool isPasswordValid(std::string password);
	static bool isUsernameValid(std::string username);

};