#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include "DbConnection.h"

namespace http 
{
namespace db 
{

class DbConnectionPool 
{
public:
    // 单例模式
    static DbConnectionPool& getInstance() 
    {
        static DbConnectionPool instance;
        return instance;
    }

    // 初始化连接池
    void init(const std::string& host,
             const std::string& user,
             const std::string& password,
             const std::string& database,
             size_t poolSize = 10);

    // 获取连接
    std::shared_ptr<DbConnection> getConnection();

private:
    // 构造函数
    DbConnectionPool();
    // 析构函数
    ~DbConnectionPool();

    // 禁止拷贝
    DbConnectionPool(const DbConnectionPool&) = delete;
    DbConnectionPool& operator=(const DbConnectionPool&) = delete;

    std::shared_ptr<DbConnection> createConnection();

    void checkConnections(); // 添加连接检查方法

private:
    std::string                               host_;
    std::string                               user_;
    std::string                               password_;
    std::string                               database_;
    std::queue<std::shared_ptr<DbConnection>> connections_;
    std::mutex                                mutex_;
    std::condition_variable                   cv_;
    bool                                      initialized_ = false;
    //后台独立监控数据库连接池的状态。该线程会持续执行checkConnections函数，自动检查连接池中所有连接的可用性
    //（例如：检测空闲超时、验证连接有效性、回收异常连接等），确保连接池始终处于健康状态。
    std::thread                               checkThread_; // 添加检查线程
};

} // namespace db
} // namespace http
