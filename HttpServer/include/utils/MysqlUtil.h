 #pragma once
 #include "db/DbConnectionPool.h"
 
#include <string>

namespace http
{

class MysqlUtil
{
public:
    // 初始化数据库连接池
    static void init(const std::string& host, const std::string& user,
                    const std::string& password, const std::string& database,
                    size_t poolSize = 10)
    {
        http::db::DbConnectionPool::getInstance().init(
            host, user, password, database, poolSize);
    }

    template<typename... Args>
    //执行查询
    sql::ResultSet* executeQuery(const std::string& sql, Args&&... args)
    {
        auto conn = http::db::DbConnectionPool::getInstance().getConnection();
        return conn->executeQuery(sql, std::forward<Args>(args)...);
    }

    template<typename... Args>
    //执行更新
    int executeUpdate(const std::string& sql, Args&&... args)
    {
        auto conn = http::db::DbConnectionPool::getInstance().getConnection();
        return conn->executeUpdate(sql, std::forward<Args>(args)...);
    }
};

} // namespace http
