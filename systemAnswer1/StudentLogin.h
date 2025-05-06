#pragma once
#include <string>

// 声明全局变量 (使用 extern 关键字)
extern std::string g_currentStudentId;
extern std::string g_currentStudentPassword;
// 学生账户结构
struct StudentAccount {
    std::string id;
    std::string password;
};

// 声明函数
bool loginStudent();
bool registerStudent();
bool checkStudentExists(const std::string& studentId);
bool verifyPassword(const std::string& studentId, const std::string& password);
std::vector<StudentAccount> loadStudentAccounts();
bool saveStudentAccount(const std::string& studentId, const std::string& password);