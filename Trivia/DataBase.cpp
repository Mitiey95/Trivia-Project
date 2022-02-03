#include "DataBase.h"

DataBase::DataBase()
{
	std::string dbFileName = "TriviaDB.sqlite";
	int doesFileExist = _access(dbFileName.c_str(), 0);
	int res = sqlite3_open(dbFileName.c_str(), &_db);
	if (doesFileExist == -1) {
		const char* sqlStatement = "";
		dbProcess(sqlStatement);

		sqlStatement = "CREATE TABLE USERS (USERNAME TEXT NOT NULL, PASSWORD TEXT NOT NULL, EMAIL TEXT NOT NULL); ";
		dbProcess(sqlStatement);


		sqlStatement = "CREATE TABLE QUESTIONS (QUESTION TEXT NOT NULL, CORRECT_ANS TEXT NOT NULL, ANS2 TEXT NOT NULL, ANS3 TEXT NOT NULL, ANS4 TEXT NOT NULL); ";
		dbProcess(sqlStatement);

		sqlStatement = "INSERT INTO QUESTIONS (QUESTION, CORRECT_ANS, ANS2, ANS3, ANS4) VALUES('2 + 2 = ', '4', '5', '3', '6');";
		dbProcess(sqlStatement);

		sqlStatement = "INSERT INTO QUESTIONS (QUESTION, CORRECT_ANS, ANS2, ANS3, ANS4) VALUES('1 + 3 = ', '4', '5', '3', '6');";
		dbProcess(sqlStatement);

		sqlStatement = "INSERT INTO QUESTIONS (QUESTION, CORRECT_ANS, ANS2, ANS3, ANS4) VALUES('12 - 8 = ', '4', '5', '3', '6');";
		dbProcess(sqlStatement);

		sqlStatement = "INSERT INTO QUESTIONS (QUESTION, CORRECT_ANS, ANS2, ANS3, ANS4) VALUES('9 - 5 = ', '4', '5', '3', '6');";
		dbProcess(sqlStatement);

		sqlStatement = "INSERT INTO QUESTIONS (QUESTION, CORRECT_ANS, ANS2, ANS3, ANS4) VALUES('20 - 16 = ', '4', '5', '3', '6');";
		dbProcess(sqlStatement);

		sqlStatement = "CREATE TABLE RESULTS (USERNAME TEXT NOT NULL, NUMBER_GAMES INTEGER NOT NULL, NUMBER_RIGHT_ANS INTEGER NOT NULL, NUMBER_WRONG_ANS INTEGER NOT NULL, AVG_TIME INTEGER NOT NULL); ";
		dbProcess(sqlStatement);
	}
}

DataBase::~DataBase()
{
	sqlite3_close(_db);
	_db = nullptr;
}

bool DataBase::isUserExists(std::string username)
{
	std::list<std::string> usernames;
	std::stringstream sqlStatement;
	sqlStatement << "SELECT USERNAME FROM USERS WHERE USERNAME = '"
		<< username
		<< "';";

	char* errMessage = nullptr;
	sqlite3_exec(_db, sqlStatement.str().c_str(), callbackUsers, &usernames, &errMessage);
	if (usernames.empty()) {
		return false;
	}
	return true;
}

bool DataBase::addNewUser(std::string username, std::string password, std::string email)
{
	std::stringstream sqlStatement;
	sqlStatement << "INSERT INTO USERS (USERNAME, PASSWORD , EMAIL) VALUES('"
		<< username
		<< "', '"
		<< password
		<< "', '"
		<< email
		<< "');";

	dbProcess(sqlStatement.str().c_str());
	return true;
}

bool DataBase::isUserAndPassMatch(std::string username, std::string password)
{
	std::list<std::string> passwords;
	std::stringstream sqlStatement;
	sqlStatement << "SELECT PASSWORD FROM USERS WHERE USERNAME = '"
		<< username
		<< "' AND PASSWORD = '"
		<< password
		<< "';";

	char* errMessage = nullptr;
	sqlite3_exec(_db, sqlStatement.str().c_str(), callbackUsers, &passwords, &errMessage);
	
	if (passwords.empty()) {
		return false;
	}
	return true;
}

std::vector<Question*> DataBase::initQuestions(int questionsNo)
{
	std::vector<Question*> questions;
	std::stringstream sqlStatement;
	sqlStatement << "SELECT * FROM QUESTIONS LIMIT "
		<< questionsNo
		<< ";";

	char* errMessage = nullptr;
	sqlite3_exec(_db, sqlStatement.str().c_str(), callbackQuestions, &questions, &errMessage);

	return questions;
}

std::vector<std::string> DataBase::getBestScores()
{
	std::vector<std::string> bestResults;
	std::stringstream sqlStatement;
	sqlStatement << "SELECT USERNAME, NUMBER_RIGHT_ANS AS 'SCORE' FROM RESULTS ORDER BY SCORE DESC LIMIT 3;";

	char* errMessage = nullptr;
	sqlite3_exec(_db, sqlStatement.str().c_str(), callbackBestScores, &bestResults, &errMessage);

	return bestResults;

}

std::vector<std::string> DataBase::getPersonalStatus(std::string username)
{
	std::vector<std::string> personalStatus;
	std::stringstream sqlStatement;
	sqlStatement << "SELECT * FROM RESULTS WHERE USERNAME = '"
		<< username
		<< "';";

	char* errMessage = nullptr;
	sqlite3_exec(_db, sqlStatement.str().c_str(), callbackPersonalStatus, &personalStatus, &errMessage);

	if (personalStatus.size() == 0) {
		personalStatus.push_back("0");
		personalStatus.push_back("0");
		personalStatus.push_back("0");
		personalStatus.push_back("0");
		personalStatus.push_back("0");
	}

	return personalStatus;
}

void DataBase::insertNewResult(std::string username, int rightAnswers, int wrongAnswers, int answerTime)
{
	std::stringstream sqlStatement;
	sqlStatement << "INSERT INTO RESULTS (USERNAME, NUMBER_GAMES, NUMBER_RIGHT_ANS, NUMBER_WRONG_ANS, AVG_TIME) VALUES('"
		<< username
		<< "', '"
		<< 1
		<< "', '"
		<< rightAnswers
		<< "', '"
		<< wrongAnswers
		<< "', '"
		<< answerTime
		<< "');";

	dbProcess(sqlStatement.str().c_str());
}

bool DataBase::isResultExists(std::string username)
{
	std::list<std::string> usernames;
	std::stringstream sqlStatement;
	sqlStatement << "SELECT USERNAME FROM RESULTS WHERE USERNAME = '"
		<< username
		<< "';";

	char* errMessage = nullptr;
	sqlite3_exec(_db, sqlStatement.str().c_str(), callbackUsers, &usernames, &errMessage);
	if (usernames.empty()) {
		return false;
	}
	return true;
}

void DataBase::updateResults(std::string username, int rightAnswers, int wrongAnswers, int answerTime)
{
	std::stringstream sqlStatement;
	sqlStatement << "UPDATE RESULTS SET NUMBER_GAMES = ((SELECT NUMBER_GAMES FROM RESULTS WHERE USERNAME = '"
		<< username
		<< "') + 1) , NUMBER_RIGHT_ANS = ((SELECT NUMBER_RIGHT_ANS FROM RESULTS WHERE USERNAME = '"
		<< username
		<< "') + "
		<< rightAnswers
		<< ") , NUMBER_WRONG_ANS = ((SELECT NUMBER_WRONG_ANS FROM RESULTS WHERE USERNAME = '"
		<< username
		<< "') + "
		<< wrongAnswers
		<< ") , AVG_TIME = ((SELECT AVG_TIME FROM RESULTS WHERE USERNAME = '"
		<< username
		<< "') * (SELECT NUMBER_GAMES FROM RESULTS WHERE USERNAME = '"
		<< username
		<< "') + "
		<< answerTime
		<< ") / ((SELECT NUMBER_GAMES FROM RESULTS WHERE USERNAME = '"
		<< username
		<< "') + 1) WHERE USERNAME = '"
		<< username
		<< "';";

	dbProcess(sqlStatement.str().c_str());
}

int DataBase::callbackUsers(void* data, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++) {
		if (std::string(azColName[i]) == "USERNAME") {
			((std::list<std::string>*)data)->push_back(argv[i]);
		}
		else if (std::string(azColName[i]) == "PASSWORD") {
			((std::list<std::string>*)data)->push_back(argv[i]);
		}
	}
	return 0;
}

int DataBase::callbackQuestions(void* data, int argc, char** argv, char** azColName)
{
	std::string id;
	std::string question;
	std::string correctAnswer;
	std::string answer2;
	std::string answer3;
	std::string answer4;

	for (int i = 0; i < argc; i++) {
		if (std::string(azColName[i]) == "QUESTION_ID") {
			id = argv[i];
		}
		else if (std::string(azColName[i]) == "QUESTION") {
			question = argv[i];
		}
		else if (std::string(azColName[i]) == "CORRECT_ANS") {
			correctAnswer = argv[i];
		}
		else if (std::string(azColName[i]) == "ANS2") {
			answer2 = argv[i];
		}
		else if (std::string(azColName[i]) == "ANS3") {
			answer3 = argv[i];
		}
		else if (std::string(azColName[i]) == "ANS4") {
			answer4 = argv[i];
		}
	}
	Question* quesstionObject = new Question(atoi(id.c_str()), question, correctAnswer, answer2, answer3, answer4);
	((std::vector<Question*>*)data)->push_back(quesstionObject);
	return 0;
}

int DataBase::callbackBestScores(void* data, int argc, char** argv, char** azColName)
{
	std::string username;
	std::string score;

	for (int i = 0; i < argc; i++) {
		if (std::string(azColName[i]) == "USERNAME" || std::string(azColName[i]) == "SCORE") {
			((std::vector<std::string>*)data)->push_back(argv[i]);
		}
	}

	return 0;

}

int DataBase::callbackPersonalStatus(void* data, int argc, char** argv, char** azColName)
{

	for (int i = 0; i < argc; i++) {
		if (std::string(azColName[i]) == "USERNAME") {
			((std::vector<std::string>*)data)->push_back(argv[i]);
		}
		else if (std::string(azColName[i]) == "NUMBER_GAMES") {
			((std::vector<std::string>*)data)->push_back(argv[i]);
		}
		else if (std::string(azColName[i]) == "NUMBER_RIGHT_ANS") {
			((std::vector<std::string>*)data)->push_back(argv[i]);
		}
		else if (std::string(azColName[i]) == "NUMBER_WRONG_ANS") {
			((std::vector<std::string>*)data)->push_back(argv[i]);
		}
		else if (std::string(azColName[i]) == "AVG_TIME") {
			((std::vector<std::string>*)data)->push_back(argv[i]);
		}
	}

	return 0;
}

void DataBase::dbProcess(const char* sqlStatement)
{
	char** errMessage = nullptr;
	int res = sqlite3_exec(_db, sqlStatement, nullptr, nullptr, errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Operation Failed: \n" << sqlStatement << std::endl;
	}
}
