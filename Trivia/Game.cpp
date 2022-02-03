#include "Game.h"

Game::Game(const std::vector<User*>& players, int questionsNo, DataBase& db)
	: _db(db)
{
	_questions = _db.initQuestions(questionsNo);
	_questions_no = questionsNo;
	_players = players;
	_currentTurnAnswers = 0;

	currQuestionIndex = 0;

	for (int i = 0; i < _players.size(); i++) {
		_players[i]->setGame(this);
		_results.insert(std::pair<std::string, int>(_players[i]->getUsername(), 0));
	}
}

Game::~Game()
{
	for (auto itP : _players)
	{
		delete itP;
	}
	_players.clear();
	_questions.clear();
	_results.clear();
}

void Game::sendFirstQuestion()
{
	sendQuestionsToAllUsers();
}

void Game::handleFinishGame()
{
	std::string message;
	message += std::to_string(END_GAME_ANS);

	message += std::to_string(_players.size());

	for (auto it = _results.begin(); it != _results.end(); ++it) {
		message += Helper::getPaddedNumber(it->first.size(), 2);
		message += it->first;

		message += Helper::getPaddedNumber(it->second, 2);
	}

	for (int i = 0; i < _players.size(); i++) {
		if (_db.isResultExists(_players[i]->getUsername())) {
			_db.updateResults(_players[i]->getUsername(), _players[i]->getRightAns(), _players[i]->getWrongAns(), _players[i]->getTimeAns() / (_players[i]->getRightAns() + _players[i]->getWrongAns()));
		}
		else {
			_db.insertNewResult(_players[i]->getUsername(), _players[i]->getRightAns(), _players[i]->getWrongAns(), _players[i]->getTimeAns() / (_players[i]->getRightAns() + _players[i]->getWrongAns()));
		}

		_players[i]->setRightAns(0);
		_players[i]->setWrongAns(0);
		_players[i]->setTimeAns(0);

		_players[i]->send(message);
		_players[i]->setGame(nullptr);
		_players[i]->clearRoom();
	}
}

bool Game::handleNextTurn()
{
	currQuestionIndex++;
	_currentTurnAnswers = 0;

	if (_players.size() == 0 || _questions_no == currQuestionIndex) {
		handleFinishGame();
		return false;
	}

	sendQuestionsToAllUsers();

	return true;
}

bool Game::handleAnswerFromUser(User* user, int answerNo, int time)
{
	bool isCorrect = false;
	
	if (answerNo == _questions[currQuestionIndex]->getCorrectAnswerIndex()) {
		auto it = _results.find(user->getUsername());
		it->second++;

		isCorrect = true;
		user->setRightAns(user->getRightAns() + 1);
	}
	else {
		user->setWrongAns(user->getWrongAns() + 1);
	}
	user->setTimeAns(user->getTimeAns() + time);

	std::string message;
	message += std::to_string(SEND_COMMENT_ANS);
	if (isCorrect) {
		message += std::to_string(1);		//CORRECT
	}
	else {
		message += std::to_string(0);		//INCORRECT
	}

	user->send(message);

	_currentTurnAnswers++;

	if (_currentTurnAnswers == _players.size()) {
		handleNextTurn();
	}

	return true;
}

bool Game::leaveGame(User* currUser)
{
	for (int i = 0; i < _players.size(); i++) {
		if (_players[i]->getUsername() == currUser->getUsername()) {
			_players.erase(_players.begin() + i);
		}
	}
	return true;
}

void Game::sendQuestionsToAllUsers()
{
	std::string message;
	message += std::to_string(SEND_QUESTIONS_ANS);

	message += Helper::getPaddedNumber(_questions[currQuestionIndex]->getQuestion().size(), 3);
	message += _questions[currQuestionIndex]->getQuestion();

	message += Helper::getPaddedNumber(_questions[currQuestionIndex]->getAnswers()[0].size(), 3);
	message += _questions[currQuestionIndex]->getAnswers()[0];

	message += Helper::getPaddedNumber(_questions[currQuestionIndex]->getAnswers()[1].size(), 3);
	message += _questions[currQuestionIndex]->getAnswers()[1];

	message += Helper::getPaddedNumber(_questions[currQuestionIndex]->getAnswers()[2].size(), 3);
	message += _questions[currQuestionIndex]->getAnswers()[2];

	message += Helper::getPaddedNumber(_questions[currQuestionIndex]->getAnswers()[3].size(), 3);
	message += _questions[currQuestionIndex]->getAnswers()[3];

	for (int i = 0; i < _players.size(); i++) {
		_players[i]->send(message);
	}

}
