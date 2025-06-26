#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <chrono>

namespace http
{

    namespace session
    {

        class SessionManager;
        // 表示一个用户会话，它保存会话数据并管理到期时间.
        class Session : public std::enable_shared_from_this<Session>
        {
        public:
            Session(const std::string &sessionId, SessionManager *sessionManager, int maxAge = 3600); // 默认1小时过期 60s x 60

            const std::string &getId() const
            {
                return sessionId_;
            }

            // 检查会话是否过期
            bool isExpired() const;
            void refresh(); // 刷新过期时间

            // 设置该会话所属的会话管理器
            void setManager(SessionManager *sessionManager)
            {
                sessionManager_ = sessionManager;
            }

            // 获取该会话所属的会话管理器
            SessionManager *getManager() const
            {
                return sessionManager_;
            }

            // 数据存取
            void setValue(const std::string &key, const std::string &value);
            // 获取数据
            std::string getValue(const std::string &key) const;
            // 移出数据
            void remove(const std::string &key);
            // 清空数据
            void clear();

        private:
            //会话id
            std::string sessionId_;
            //k-v存储数据
            std::unordered_map<std::string, std::string> data_;
            //存储一个精确的时间点，表示该会话（Session）的过期时刻。
            std::chrono::system_clock::time_point expiryTime_;
            int maxAge_; // 过期时间（秒）
            //当前会话所属会话管理器
            SessionManager *sessionManager_;
        };

    } // namespace session
} // namespace http