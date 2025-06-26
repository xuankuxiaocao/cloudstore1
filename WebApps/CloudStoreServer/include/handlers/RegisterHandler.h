#pragma once
#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../../../HttpServer/include/utils/MysqlUtil.h"
#include "../CloudStoreServer.h"

class RegisterHandler : public http::router::RouterHandler 
{
public:
    explicit RegisterHandler(CloudStoreServer* server) : server_(server) {}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:
    int insertUser(const std::string& username, const std::string& password);
    bool isUserExist(const std::string& username);
private:
    CloudStoreServer* server_;
    http::MysqlUtil     mysqlUtil_;
};