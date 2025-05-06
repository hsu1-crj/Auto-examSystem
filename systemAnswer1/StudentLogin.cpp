#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include "Student.h"
#include "Question.h"
#include "StudentLogin.h"

// 添加全局变量来存储当前登录的学生信息
std::string g_currentStudentId = "";
std::string g_currentStudentPassword = "";

// 学生账户文件
const std::string STUDENT_ACCOUNTS_FILE = "student_accounts.txt";

// 加载所有学生账户
std::vector<StudentAccount> loadStudentAccounts() {
    std::vector<StudentAccount> accounts;
    std::ifstream file(STUDENT_ACCOUNTS_FILE);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find(',');
            if (pos != std::string::npos) {
                StudentAccount account;
                account.id = line.substr(0, pos);
                account.password = line.substr(pos + 1);
                accounts.push_back(account);
            }
        }
        file.close();
    }
    return accounts;
}

// 检查学生是否已注册
bool checkStudentExists(const std::string& studentId) {
    auto accounts = loadStudentAccounts();
    return std::find_if(accounts.begin(), accounts.end(),
        [&](const StudentAccount& acc) { return acc.id == studentId; }) != accounts.end();
}

// 验证密码是否匹配
bool verifyPassword(const std::string& studentId, const std::string& password) {
    auto accounts = loadStudentAccounts();
    auto it = std::find_if(accounts.begin(), accounts.end(),
        [&](const StudentAccount& acc) { return acc.id == studentId; });

    if (it != accounts.end()) {
        return it->password == password;
    }
    return false;
}

// 保存新学生账户
bool saveStudentAccount(const std::string& studentId, const std::string& password) {
    std::ofstream file(STUDENT_ACCOUNTS_FILE, std::ios::app);
    if (file.is_open()) {
        file << studentId << "," << password << std::endl;
        file.close();
        return true;
    }
    return false;
}

// 学生注册功能
bool registerStudent() {
    std::string id, password, confirmPassword;

    std::cout << "======= 学生注册 =======\n";
    std::cout << "请输入学号: ";
    std::getline(std::cin, id);

    // 检查学号是否已存在
    if (checkStudentExists(id)) {
        std::cout << "该学号已注册，请直接登录或使用其他学号。\n";
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
    if (saveStudentAccount(id, password)) {
        std::cout << "注册成功！现在您可以使用您的学号登录了。\n";
        return true;
    }
    else {
        std::cout << "注册失败，无法保存账户信息。\n";
        return false;
    }
}

// 学生登录功能
bool loginStudent() {
    std::string id, password;
    bool loginSuccess = false;
    int attempts = 0;
    const int MAX_ATTEMPTS = 3;

    while (!loginSuccess && attempts < MAX_ATTEMPTS) {
        std::cout << "======= 学生登录 =======\n";
        std::cout << "请输入学号 (如若尚未注册账号 请输入 'r' 注册新账号): ";
        std::getline(std::cin, id);

        // 检查是否要注册新账号
        if (id == "r") {
            if (registerStudent()) {
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
        if (!checkStudentExists(id)) {
            std::cout << "该学号尚未注册。请先注册账号。\n";
            std::cout << "是否立即注册? (y/n): ";
            std::string choice;
            std::getline(std::cin, choice);
            if (choice == "y" || choice == "Y") {
                if (registerStudent()) {
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
        if (verifyPassword(id, password)) {
            loginSuccess = true;
            g_currentStudentId = id;
            g_currentStudentPassword = password;
            std::cout << "登录成功！欢迎 学号:" << id << "\n\n";
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