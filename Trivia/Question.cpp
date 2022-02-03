#include "Question.h"

Question::Question(int id, std::string question, std::string correctAnswer, std::string answer2, std::string answer3, std::string answer4)
{
	srand((unsigned)time(0));
	int random1, random2;

	_question = question;
	_id = id;
	_correctAnswerIndex = 0;

	_answers[0] = correctAnswer;
	_answers[1] = answer2;
	_answers[2] = answer3;
	_answers[3] = answer4;

	for (int i = 0; i < COUNT_RANDOM; i++) {
		random1 = rand() % 4;
		random2 = rand() % 4;

		std::string answerTemp = _answers[random2];
		_answers[random2] = _answers[random1];
		_answers[random1] = answerTemp;

		if (_answers[random1] == correctAnswer) {
			_correctAnswerIndex = random1;
		}
		else if (_answers[random2] == correctAnswer) {
			_correctAnswerIndex = random2;
		}
	}
}

std::string Question::getQuestion()
{
	return _question;
}

std::string* Question::getAnswers()
{
	return _answers;
}

int Question::getCorrectAnswerIndex()
{
	return _correctAnswerIndex;
}

int Question::getId()
{
	return _id;
}
