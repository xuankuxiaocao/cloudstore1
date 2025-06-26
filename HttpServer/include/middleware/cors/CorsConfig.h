#pragma once

#include <string>
#include <vector>

namespace http 
{
namespace middleware 
{

struct CorsConfig 
{
    //表示允许哪些“来源”（域名/网站）访问你的服务器资源。 ---白名单
    std::vector<std::string> allowedOrigins;
    //允许的Method方法
    std::vector<std::string> allowedMethods;
    //允许的请求头，存键值对中的值
    std::vector<std::string> allowedHeaders;
    //是否允许携带cookie
    bool allowCredentials = false;
    //预检请求缓存时间，减少服务器重复发送options预检请求
    int maxAge = 3600;
    
    static CorsConfig defaultConfig() 
    {
        CorsConfig config;
        config.allowedOrigins = {"*"};
        config.allowedMethods = {"GET", "POST", "PUT", "DELETE", "OPTIONS"};
        config.allowedHeaders = {"Content-Type", "Authorization"};
        return config;
    }
};

} // namespace middleware
} // namespace http