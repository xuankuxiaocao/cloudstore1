#pragma once
#include <string>
#include "../../../../HttpServer/include/utils/MysqlUtil.h"

class UserAuth {
public:
    UserAuth(const std::string& host, const std::string& user, const std::string& password, const std::string& database);
    bool registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);
private:
    bool userExists(const std::string& username);
    std::string hashPassword(const std::string& password);
};