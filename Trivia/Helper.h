#pragma once

#include <vector>
#include <string>
#include <WinSock2.h>


enum MessageType : int
{
	SIGN_IN_ASK = 200,
	SIGN_OUT_ASK = 201,
	SIGN_IN_ANS = 102,
	SIGN_UP_ASK = 203,
	SIGN_UP_ANS = 104,
	ROOM_LIST_ASK = 205,
	ROOM_LIST_ANS = 106,
	USERS_ROOM_ASK = 207,
	USERS_ROOM_ANS = 108,
	JOIN_EXIST_ROOM_ASK = 209,
	JOIN_EXIST_ROOM_ANS = 110,
	OUT_ROOM_ASK = 211,
	OUT_ROOM_ANS = 112,
	CREATE_ROOM_ASK = 213,
	CREATE_ROOM_ANS = 114,
	CLOSE_ROOM_ASK = 215,
	CLOSE_ROOM_ANS = 116,
	START_GAME_ASK = 217,
	SEND_QUESTIONS_ANS = 118,
	SEND_ANSWER_ASK = 219,
	SEND_COMMENT_ANS = 120,
	END_GAME_ANS = 121,
	OUT_GAME_ASK = 222,
	OUT_GAME_ANS = 122,
	BEST_SCORE_ASK = 223,
	BEST_SCORE_ANS = 124,
	PROFILE_ASK = 225,
	PROFILE_ANS = 126,
	EXIT_ASK = 299,

	PRINT_SERVER = 300

};


class Helper
{
public:


	static int getMessageTypeCode(SOCKET sc);
	static int getIntPartFromSocket(SOCKET sc, int bytesNum);
	static std::string getStringPartFromSocket(SOCKET sc, int bytesNum);
	static void sendData(SOCKET sc, std::string message);
	static void send_update_message_to_client(SOCKET sc, const std::string& file_content, const std::string& second_username, const std::string& all_users);
	static std::string getPaddedNumber(int num, int digits);

private:
	static char* getPartFromSocket(SOCKET sc, int bytesNum);
	static char* getPartFromSocket(SOCKET sc, int bytesNum, int flags);

};


#ifdef _DEBUG // vs add this define in debug mode
#include <stdio.h>
// Q: why do we need traces ?
// A: traces are a nice and easy way to detect bugs without even debugging
// or to understand what happened in case we miss the bug in the first time
#define TRACE(msg, ...) printf(msg "\n", __VA_ARGS__);
// for convenient reasons we did the traces in stdout
// at general we would do this in the error stream like that
// #define TRACE(msg, ...) fprintf(stderr, msg "\n", __VA_ARGS__);

#else // we want nothing to be printed in release version
#define TRACE(msg, ...) printf(msg "\n", __VA_ARGS__);
#define TRACE(msg, ...) // do nothing
#endif