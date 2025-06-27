#include "../include/handlers/RegisterHandler.h"

void RegisterHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp) {
    // 解析body(json格式)
    json parsed = json::parse(req.getBody());
    std::string username = parsed["user_name"];
    std::string nickname = parsed["nick_name"];
    std::string phone = parsed["phone"];
    std::string email = parsed["email"];
    std::string password = parsed["password"];

    // 判断用户是否已经存在，如果存在则注册失败
    int userId = insertUser(username, nickname, phone, email, password);
    if (userId != -1) {
        // 插入成功
        // 封装成功响应
        json successResp;   
        successResp["status"] = "success";
        successResp["message"] = "Register successful";
        successResp["userId"] = userId;
        std::string successBody = successResp.dump(4);

        resp->setStatusLine(req.getVersion(), http::HttpResponse::k200Ok, "OK");
        resp->setCloseConnection(false);
        resp->setContentType("application/json");
        resp->setContentLength(successBody.size());
        resp->setBody(successBody);
    }
    else
    {
        // 插入失败
        json failureResp;
        failureResp["status"] = "error";
        failureResp["message"] = "username already exists";
        std::string failureBody = failureResp.dump(4);

        resp->setStatusLine(req.getVersion(), http::HttpResponse::k409Conflict, "Conflict");
        resp->setCloseConnection(false);
        resp->setContentType("application/json");
        resp->setContentLength(failureBody.size());
        resp->setBody(failureBody);
    }
}

int RegisterHandler::insertUser(const std::string &username, const std::string &nickname, const std::string &phone, const std::string &email, const std::string &password)
{
    // 判断用户是否存在，如果存在则返回-1，否则返回用户id
    if (!isUserExist(username))
    {
        // 用户不存在，插入用户
        // 使用预处理语句避免 SQL 注入，并插入所有字段
        std::string sql = "INSERT INTO user (user_name, nick_name, phone, email, password) VALUES (?, ?, ?, ?, ?)";
        mysqlUtil_.executeUpdate(sql, username, nickname, phone, email, password);

        std::string sql2 = "SELECT id FROM user WHERE user_name = ?";
        sql::ResultSet* res = mysqlUtil_.executeQuery(sql2, username);
        if (res->next()) {
            int userId = res->getInt("id");
            delete res; // 释放资源
            return userId;
        }
        delete res; // 释放资源
    }
    return -1;
}

bool RegisterHandler::isUserExist(const std::string &username)
{
    std::string sql = "SELECT id FROM user WHERE user_name = ?";
    sql::ResultSet* res = mysqlUtil_.executeQuery(sql, username);
    bool exists = res->next();
    delete res; // 释放资源
    return exists;
}