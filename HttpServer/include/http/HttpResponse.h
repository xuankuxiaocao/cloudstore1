#pragma once

#include <muduo/net/TcpServer.h>

namespace http
{

class HttpResponse 
{
public:
    enum HttpStatusCode
    {
        kUnknown,                       //未知状态码
        k200Ok = 200,                   //200ok 一切正常
        k204NoContent = 204,            //204NO CONTENT 成功处理请求，但响应体为空
        k301MovedPermanently = 301,     //301MOVED PERMANENTLY 永久重定向
        k400BadRequest = 400,           //400Bad Request 错误请求，表示请求报文有误
        k401Unauthorized = 401,         //401Unauthorized 未授权，无权限访问
        k403Forbidden = 403,            //403FORBIDDEN 服务器理解请求禁止访问
        k404NotFound = 404,             //URL 资源不存在
        k409Conflict = 409,             //409Conflict 请求冲突，比如重复创建资源
        k500InternalServerError = 500,  //500Internal Server Error 服务器内部处理请求报文出现错误
    };

    HttpResponse(bool close = true)
        : statusCode_(kUnknown)
        , closeConnection_(close)
    {}

    void setVersion(std::string version)
    { httpVersion_ = version; }
    void setStatusCode(HttpStatusCode code)
    { statusCode_ = code; }

    HttpStatusCode getStatusCode() const
    { return statusCode_; }

    void setStatusMessage(const std::string message)
    { statusMessage_ = message; }

    void setCloseConnection(bool on)
    { closeConnection_ = on; }

    bool closeConnection() const
    { return closeConnection_; }
    
    void setContentType(const std::string& contentType)
    { addHeader("Content-Type", contentType); }

    void setContentLength(uint64_t length)
    { addHeader("Content-Length", std::to_string(length)); }

    void addHeader(const std::string& key, const std::string& value)
    { headers_[key] = value; }
    
    void setBody(const std::string& body)
    { 
        body_ = body;
        // body_ += "\0";
    }

    void setStatusLine(const std::string& version,
                         HttpStatusCode statusCode,
                         const std::string& statusMessage);

    void setErrorHeader(){}

    void appendToBuffer(muduo::net::Buffer* outputBuf) const;
private:
    std::string                        httpVersion_; 
    HttpStatusCode                     statusCode_;
    std::string                        statusMessage_;
    bool                               closeConnection_;
    std::map<std::string, std::string> headers_;
    std::string                        body_;
    bool                               isFile_;
};

} // namespace http