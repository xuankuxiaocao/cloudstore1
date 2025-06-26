#include "../../include/http/HttpContext.h"

using namespace muduo;
using namespace muduo::net;

namespace http
{

    /*
        <Method> <Request-URI> <HTTP-Version>  // 请求行
        <Method> <Request-URI> <HTTP-Version>  // 请求行
        <Header>: <Value>                      // 请求头（多行）
        ...
                                              // 空行（CRLF）
        <Body>                                 // 请求体（可选）
    */
    // 将报文解析出来将关键信息封装到HttpRequest对象里面去
    bool HttpContext::parseRequest(Buffer *buf, Timestamp receiveTime)
    {
        bool ok = true; // 解析每行请求格式是否正确
        bool hasMore = true;
        while (hasMore)
        {
            // 填充请求行
            if (state_ == kExpectRequestLine)
            {
                const char *crlf = buf->findCRLF(); // 注意这个返回值边界可能有错
                /*
                  const char* findCRLF() const
                    {
                        // FIXME: replace with memmem()?
                        //如果找到目标，返回指向第一个匹配位置的迭代器；否则返回 last1（即 beginWrite()）。
                        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
                        return crlf == beginWrite() ? NULL : crlf;
                    }
                */
                if (crlf)
                {
                    // 只有Method URI HTTP_Version三者都成功填充，才会返回true
                    ok = processRequestLine(buf->peek(), crlf);
                    if (ok)
                    {
                        request_.setReceiveTime(receiveTime);
                        // 让buf的读指针移到/r/n的下一位，也就是请求头的第一位
                        buf->retrieveUntil(crlf + 2);
                        // 接下来就可以填充请求头了
                        state_ = kExpectHeaders;
                    }
                    else
                    {
                        hasMore = false;
                    }
                }
                else
                {
                    hasMore = false;
                }
            }
            // 填充请求头
            else if (state_ == kExpectHeaders)
            {
                const char *crlf = buf->findCRLF();
                if (crlf)
                {
                    const char *colon = std::find(buf->peek(), crlf, ':');
                    if (colon < crlf)
                    {
                        request_.addHeader(buf->peek(), colon, crlf);
                    }
                    else if (buf->peek() == crlf)
                    {
                        // 空行，结束Header
                        // 根据请求方法和Content-Length判断是否需要继续读取body
                        if (request_.method() == HttpRequest::kPost ||
                            request_.method() == HttpRequest::kPut)
                        {
                            std::string contentLength = request_.getHeader("Content-Length");
                            if (!contentLength.empty())
                            {
                                request_.setContentLength(std::stoi(contentLength));
                                if (request_.contentLength() > 0)
                                {
                                    state_ = kExpectBody;
                                }
                                else
                                {
                                    state_ = kGotAll;
                                    hasMore = false;
                                }
                            }
                            else
                            {
                                // POST/PUT 请求没有 Content-Length，是HTTP语法错误
                                ok = false;
                                hasMore = false;
                            }
                        }
                        else
                        {
                            // GET/HEAD/DELETE 等方法直接完成（没有请求体）
                            state_ = kGotAll;
                            hasMore = false;
                        }
                    }
                    else
                    {
                        ok = false; // Header行格式错误
                        hasMore = false;
                    }
                    buf->retrieveUntil(crlf + 2); // 开始读指针指向下一行数据
                }
                else
                {
                    hasMore = false;
                }
            }

            else if (state_ == kExpectBody)
            {
                // 检查缓冲区中是否有足够的数据
                if (buf->readableBytes() < request_.contentLength())
                {
                    hasMore = false; // 数据不完整，等待更多数据
                    return true;
                }

                // 只读取 Content-Length 指定的长度
                std::string body(buf->peek(), buf->peek() + request_.contentLength());
                request_.setBody(body);

                // 准确移动读指针
                buf->retrieve(request_.contentLength());

                state_ = kGotAll;
                hasMore = false;
            }
        }

        return ok; // ok为false代表报文语法解析错误
    }

    // 解析请求行
    bool HttpContext::processRequestLine(const char *begin, const char *end)
    {
        bool succeed = false;
        const char *start = begin;
        /*
            <Method> <Request-URI> <HTTP-Version>  // 请求行
            <Method> <Request-URI> <HTTP-Version>  // 请求行
            <Header>: <Value>                      // 请求头（多行）
            ...
                                                  // 空行（CRLF）
            <Body>                                 // 请求体（可选）
        */
        // 找Method
        const char *space = std::find(start, end, ' ');
        if (space != end && request_.setMethod(start, space))
        {
            start = space + 1;
            // 找URI
            space = std::find(start, end, ' ');
            if (space != end)
            {
                const char *argumentStart = std::find(start, space, '?');
                if (argumentStart != space) // 请求带参数
                {
                    request_.setPath(start, argumentStart); // 注意这些返回值边界
                    request_.setQueryParameters(argumentStart + 1, space);
                }
                else // 请求不带参数
                {
                    request_.setPath(start, space);
                }

                start = space + 1;
                succeed = ((end - start == 8) && std::equal(start, end - 1, "HTTP/1."));
                if (succeed)
                {
                    if (*(end - 1) == '1')
                    {
                        request_.setVersion("HTTP/1.1");
                    }
                    else if (*(end - 1) == '0')
                    {
                        request_.setVersion("HTTP/1.0");
                    }
                    else
                    {
                        succeed = false;
                    }
                }
            }
        }
        return succeed;
    }

} // namespace http