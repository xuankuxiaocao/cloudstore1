#include "../../include/middleware/MiddlewareChain.h"
#include <muduo/base/Logging.h>

namespace http
{
    namespace middleware
    {

        //增加中间件
        void MiddlewareChain::addMiddleware(std::shared_ptr<Middleware> middleware)
        {
            middlewares_.push_back(middleware);
        }

        //在发http请求前，先经过一系列中间件处理
        void MiddlewareChain::processBefore(HttpRequest &request)
        {
            for (auto &middleware : middlewares_)
            {
                middleware->before(request);
            }
        }

        //发出响应后，先经过一系列中间件处理
        void MiddlewareChain::processAfter(HttpResponse &response)
        {
            try
            {
                // 反向处理响应，以保持中间件的正确执行顺序
                for (auto it = middlewares_.rbegin(); it != middlewares_.rend(); ++it)
                {
                    if (*it)
                    { // 添加空指针检查
                        (*it)->after(response);
                    }
                }
            }
            catch (const std::exception &e)
            {
                LOG_ERROR << "Error in middleware after processing: " << e.what();
            }
        }

    } // namespace middleware
} // namespace http
