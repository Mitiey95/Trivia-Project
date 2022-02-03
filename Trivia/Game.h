#pragma once

#include "Question.h"
#include "User.h"
#include "DataBase.h"
#include "Helper.h"

#include <WinSock2.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

class User;
class DataBase;
class Game {
public:
	Game(const std::vector<User*>& players, int questionsNo, DataBase& db);
	~Game();
	void sendFirstQuestion();
	void handleFinishGame();
	bool handleNextTurn();
	bool handleAnswerFromUser(User* user, int answerNo, int time);
	bool leaveGame(User* currUser);
	void sendQuestionsToAllUsers();

private:
	std::vector<Question*> _questions;
	std::vector<User*> _players;
	int _questions_no;
	int currQuestionIndex;
	DataBase& _db;
	std::map<std::string, int> _results;
	int _currentTurnAnswers;

};
