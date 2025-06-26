#pragma once
#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../CloudStoreServer.h"

class EntryHandler : public http::router::RouterHandler 
{
public:
    explicit EntryHandler(CloudStoreServer* server) : server_(server) {}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;

private:
    CloudStoreServer* server_;
};