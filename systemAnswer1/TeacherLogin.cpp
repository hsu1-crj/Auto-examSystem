#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include "TeacherLogin.h"  // 假设存在Teacher结构体定义的头文件


// 添加全局变量来存储当前登录的教师信息
std::string g_currentTeacherId = "";
std::string g_currentTeacherPassword = "";

// 教师账户文件
const std::string TEACHER_ACCOUNTS_FILE = "teacher_accounts.txt";

// 加载所有教师账户
std::vector<TeacherAccount> loadTeacherAccounts() {
    std::vector<TeacherAccount> accounts;
    std::ifstream file(TEACHER_ACCOUNTS_FILE);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find(',');
            if (pos != std::string::npos) {
                TeacherAccount account;
                account.id = line.substr(0, pos);
                account.password = line.substr(pos + 1);
                accounts.push_back(account);
            }
        }
        file.close();
    }
    return accounts;
}

// 检查教师是否已注册
bool checkTeacherExists(const std::string& teacherId) {
    auto accounts = loadTeacherAccounts();
    return std::find_if(accounts.begin(), accounts.end(),
        [&](const TeacherAccount& acc) { return acc.id == teacherId; }) != accounts.end();
}

// 验证密码是否匹配
bool verifyPasswordTeacher(const std::string& teacherId, const std::string& password) {
    auto accounts = loadTeacherAccounts();
    auto it = std::find_if(accounts.begin(), accounts.end(),
        [&](const TeacherAccount& acc) { return acc.id == teacherId; });

    if (it != accounts.end()) {
        return it->password == password;
    }
    return false;
}

// 保存新教师账户
bool saveTeacherAccount(const std::string& teacherId, const std::string& password) {
    std::ofstream file(TEACHER_ACCOUNTS_FILE, std::ios::app);
    if (file.is_open()) {
        file << teacherId << "," << password << std::endl;
        file.close();
        return true;
    }
    return false;
}

// 教师注册功能
bool registerTeacher() {
    std::string id, password, confirmPassword;

    std::cout << "======= 教师注册 =======\n";
    std::cout << "请输入教师工号: ";
    std::getline(std::cin, id);

    // 检查教师工号是否已存在
    if (checkTeacherExists(id)) {
        std::cout << "该教师工号已注册，请直接登录或使用其他工号。\n";
        return false;
    }

    // 输入并确认密码
    bool passwordsMatch = false;
    while (!passwordsMatch) {
        std::cout << "请输入密码 (至少4位): ";
        std::getline(std::cin, password);

        if (password.length() < 4) {
            std::cout << "密码长度不足4位，请重新输入。\n";
            continue;
        }

        std::cout << "请再次输入密码确认: ";
        std::getline(std::cin, confirmPassword);

        if (password != confirmPassword) {
            std::cout << "两次输入的密码不匹配，请重新输入。\n";
        }
        else {
            passwordsMatch = true;
        }
    }

    // 保存新账户
    if (saveTeacherAccount(id, password)) {
        std::cout << "注册成功！现在您可以使用您的教师工号登录了。\n";
        return true;
    }
    else {
        std::cout << "注册失败，无法保存账户信息。\n";
        return false;
    }
}

// 教师登录功能
bool loginTeacher() {
    std::string id, password;
    bool loginSuccess = false;
    int attempts = 0;
    const int MAX_ATTEMPTS = 3;

    while (!loginSuccess && attempts < MAX_ATTEMPTS) {
        std::cout << "======= 教师登录 =======\n";
        std::cout << "请输入教师工号 (如若尚未注册账号 请输入 'r' 注册新账号): ";
        std::getline(std::cin, id);

        // 检查是否要注册新账号
        if (id == "r") {
            if (registerTeacher()) {
                // 注册成功后重新开始登录过程
                attempts = 0;
                continue;
            }
            else {
                // 注册失败，给用户一次重新选择的机会
                std::cout << "是否继续尝试登录? (y/n): ";
                std::string choice;
                std::getline(std::cin, choice);
                if (choice != "y" && choice != "Y") {
                    return false;
                }
                continue;
            }
        }

        // 检查账号是否存在
        if (!checkTeacherExists(id)) {
            std::cout << "该教师工号尚未注册。请先注册账号。\n";
            std::cout << "是否立即注册? (y/n): ";
            std::string choice;
            std::getline(std::cin, choice);
            if (choice == "y" || choice == "Y") {
                if (registerTeacher()) {
                    // 注册成功后重新开始登录过程
                    attempts = 0;
                    continue;
                }
            }
            attempts++;
            continue;
        }

        // 输入密码
        std::cout << "请输入密码: ";
        std::getline(std::cin, password);

        // 验证密码
        if (verifyPasswordTeacher(id, password)) {
            loginSuccess = true;
            g_currentTeacherId = id;
            g_currentTeacherPassword = password;
            std::cout << "登录成功！欢迎 教师工号:" << id << "\n\n";
        }
        else {
            attempts++;
            std::cout << "密码错误，请重新输入。还剩" << (MAX_ATTEMPTS - attempts) << "次机会。\n";
        }
    }

    if (!loginSuccess) {
        std::cout << "登录失败次数过多，程序退出\n";
    }

    return loginSuccess;
}