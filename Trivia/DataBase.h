#pragma once

#include "Question.h"
#include "User.h"

#include "sqlite3.h"
#include <io.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <sstream>

class DataBase {
public:
	
	DataBase();
	~DataBase();
	bool isUserExists(std::string username);
	bool addNewUser(std::string username, std::string password, std::string email);
	bool isUserAndPassMatch(std::string username, std::string password);
	std::vector<Question*> initQuestions(int questionsNo);

	std::vector<std::string> getBestScores();
	std::vector<std::string> getPersonalStatus(std::string username);

	void insertNewResult(std::string username, int rightAnswers, int wrongAnswers, int answerTime);
	bool isResultExists(std::string username);
	void updateResults(std::string username, int rightAnswers, int wrongAnswers, int answerTime);

private:
	sqlite3* _db;

	static int callbackUsers(void* data, int argc, char** argv, char** azColName);
	static int callbackGames(void* data, int argc, char** argv, char** azColName);
	static int callbackQuestions(void* data, int argc, char** argv, char** azColName);
	static int callbackBestScores(void* data, int argc, char** argv, char** azColName);
	static int callbackPersonalStatus(void* data, int argc, char** argv, char** azColName);

	static int callbackCount(void* data, int argc, char** argv, char** azColName);

	void dbProcess(const char* sqlStatement);
	
};
