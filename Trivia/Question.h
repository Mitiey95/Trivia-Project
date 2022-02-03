#pragma once

#include <iostream>
#include <string>
#include <ctime> 

#define COUNT_RANDOM 10

class Question {
public:
	Question(int id, std::string question, std::string correctAnswer, std::string answer2, std::string answer3, std::string answer4);
	std::string getQuestion();
	std::string* getAnswers();
	int getCorrectAnswerIndex();
	int getId();

private:
	std::string _question;
	std::string _answers[4];
	int _correctAnswerIndex;
	int _id;

};
