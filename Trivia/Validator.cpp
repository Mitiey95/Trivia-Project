#include "Validator.h"

bool Validator::isPasswordValid(std::string password)
{
    bool hasDigit = false;
    bool hasBigLetter = false;
    bool hasSmallLetter = false;

    if (password.length() < 4) {
        return false;
    }
    
    for (int i = 0; i < password.length(); i++) {
        if (password[i] == ' ') {
            return false;
        }
        if (isdigit(password[i])) {
            hasDigit = true;
        }
        if (password[i] >= 'a' && password[i] <= 'z') {
            hasSmallLetter = true;
        }
        if (password[i] >= 'A' && password[i] <= 'Z') {
            hasBigLetter = true;
        }
    }

    if (hasDigit && hasSmallLetter && hasBigLetter) {
        return true;
    }

    return false;
}

bool Validator::isUsernameValid(std::string username)
{
    if (username.length() == 0) {
        return false;
    }

    if (!isalpha(username[0])) {
        return false;
    }

    for (int i = 0; i < username.length(); i++) {
        if (username[i] == ' ') {
            return false;
        }
    }

    return true;
}
