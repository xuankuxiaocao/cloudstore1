#include "../include/UserAuth.h"
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <openssl/sha.h>
#include <sstream>

UserAuth::UserAuth(const std::string& host, const std::string& user, const std::string& password, const std::string& database) {
    http::MysqlUtil::init(host, user, password, database);
}

std::string UserAuth::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool UserAuth::userExists(const std::string& username) {
    try {
        auto conn = http::db::DbConnectionPool::getInstance().getConnection();
        std::string sql = "SELECT * FROM users WHERE username = ?";
        auto stmt = conn->getConnection()->prepareStatement(sql);
        stmt->setString(1, username);
        auto res = stmt->executeQuery();
        return res->next();
    } catch (...) {
        return false;
    }
}

bool UserAuth::registerUser(const std::string& username, const std::string& password) {
    if (userExists(username)) {
        return false;
    }

    try {
        auto conn = http::db::DbConnectionPool::getInstance().getConnection();
        std::string sql = "INSERT INTO users (username, password) VALUES (?, ?)";
        auto stmt = conn->getConnection()->prepareStatement(sql);
        stmt->setString(1, username);
        stmt->setString(2, hashPassword(password));
        stmt->executeUpdate();
        return true;
    } catch (...) {
        return false;
    }
}

bool UserAuth::loginUser(const std::string& username, const std::string& password) {
    try {
        auto conn = http::db::DbConnectionPool::getInstance().getConnection();
        std::string sql = "SELECT password FROM users WHERE username = ?";
        auto stmt = conn->getConnection()->prepareStatement(sql);
        stmt->setString(1, username);
        auto res = stmt->executeQuery();
        if (res->next()) {
            std::string storedHash = res->getString(1);
            std::string inputHash = hashPassword(password);
            return storedHash == inputHash;
        }
        return false;
    } catch (...) {
        return false;
    }
}