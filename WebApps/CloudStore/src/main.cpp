#include "../include/UserAuth.h"
#include <iostream>

int main() {
    // 请根据实际情况修改数据库连接信息
    UserAuth auth("tcp://127.0.0.1:3306", "root", "root", "cloudstore");

    // 示例注册
    if (auth.registerUser("testuser", "testpass")) {
        std::cout << "注册成功！" << std::endl;
    } else {
        std::cout << "注册失败！" << std::endl;
    }

    // 示例登录
    if (auth.loginUser("testuser", "testpass")) {
        std::cout << "登录成功！" << std::endl;
    } else {
        std::cout << "登录失败！" << std::endl;
    }

    return 0;
}