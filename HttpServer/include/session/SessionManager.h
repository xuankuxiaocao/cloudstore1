#pragma once

#include "SessionStorage.h"
#include "../http/HttpRequest.h"
#include "../http/HttpResponse.h"
#include <memory>
#include <random>

namespace http
{
    namespace session
    {

        class SessionManager
        {
        public:
            explicit SessionManager(std::unique_ptr<SessionStorage> storage);

            // 从请求中获取或创建会话
            std::shared_ptr<Session> getSession(const HttpRequest &req, HttpResponse *resp);

            // 销毁会话
            void destroySession(const std::string &sessionId);

            // 清理过期会话
            void cleanExpiredSessions();

            // 更新会话
            void updateSession(std::shared_ptr<Session> session)
            {
                storage_->save(session);
            }

        private:
            // 生成唯一的会话标识符，确保会话的唯一性和安全性
            std::string generateSessionId();
            // 从cookie中获取sessionId
            std::string getSessionIdFromCookie(const HttpRequest &req);
            // 设置cookie
            void setSessionCookie(const std::string &sessionId, HttpResponse *resp);

        private:
            // 会话存储
            std::unique_ptr<SessionStorage> storage_;
            std::mt19937 rng_; // 用于生成随机会话id
        };

    } // namespace session
} // namespace http