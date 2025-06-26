#pragma once

#include <map>
#include <string>
#include <unordered_map>

#include <muduo/base/Timestamp.h>

namespace http
{

    class HttpRequest
    {
    public:
        enum Method
        {
            kInvalid, // 无效方法
            kGet,     // 获取资源
            kPost,    // 创建/修改资源 --提交表格
            kHead,    // 只获取响应头部(无响应体)
            kPut,     // 替换/创建资源
            kDelete,  // 删除资源
            kOptions  // 获取目标支持的资源通信选项 --对端支持的方法等
        };

        HttpRequest()
            : method_(kInvalid), version_("Unknown")
        {
        }
        /*
            <Method> <Request-URI> <HTTP-Version>  // 请求行
            <Header>: <Value>                      // 请求头（多行）
            ...
                                                  // 空行（CRLF）
            <Body>                                 // 请求体（可选）
        */
        // 设置收到响应体的时间？
        void setReceiveTime(muduo::Timestamp t);
        muduo::Timestamp receiveTime() const { return receiveTime_; }
        // 设置请求行中的method字段
        bool setMethod(const char *start, const char *end);
        Method method() const { return method_; }
        // 设置请求行的url字段
        void setPath(const char *start, const char *end);
        std::string path() const { return path_; }
        // 设置请求行中http_version字段
        void setVersion(std::string v)
        {
            version_ = v;
        }
        std::string getVersion() const
        {
            return version_;
        }

        void setPathParameters(const std::string &key, const std::string &value);
        std::string getPathParameters(const std::string &key) const;

        void setQueryParameters(const char *start, const char *end);
        std::string getQueryParameters(const std::string &key) const;

        void addHeader(const char *start, const char *colon, const char *end);
        std::string getHeader(const std::string &field) const;

        const std::map<std::string, std::string> &headers() const
        {
            return headers_;
        }

        //设置请求体
        //第一种设置
        void setBody(const std::string &body) { content_ = body; }
        //第二种设置
        void setBody(const char *start, const char *end)
        {
            if (end >= start)
            {
                //把start的前end-start个字符赋给content_
                content_.assign(start, end - start);
            }
        }
        std::string getBody() const
        {
            return content_;
        }
        //设置请求体长度
        void setContentLength(uint64_t length)
        {
            contentLength_ = length;
        }
        uint64_t contentLength() const
        {
            return contentLength_;
        }

        void swap(HttpRequest &that);

    private:
        Method method_;                                                // 请求方法
        std::string version_;                                          // http版本
        std::string path_;                                             // 请求路径
        std::unordered_map<std::string, std::string> pathParameters_;  // 路径参数
        std::unordered_map<std::string, std::string> queryParameters_; // 查询参数
        muduo::Timestamp receiveTime_;                                 // 接收时间
        std::map<std::string, std::string> headers_;                   // 请求头
        std::string content_;                                          // 请求体
        uint64_t contentLength_{0};                                    // 请求体长度
    };

} // namespace http