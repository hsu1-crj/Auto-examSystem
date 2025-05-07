#pragma once
#include <string>
#include <vector>

// 声明全局变量 (使用 extern 关键字)
extern std::string g_currentTeacherId;
extern std::string g_currentTeacherPassword;

// 教师账户结构
struct TeacherAccount {
    std::string id;
    std::string password;
};

// 声明函数
bool loginTeacher();
bool registerTeacher();
bool checkTeacherExists(const std::string& teacherId);
bool verifyPasswordTeacher(const std::string& teacherId, const std::string& password);
std::vector<TeacherAccount> loadTeacherAccounts();
bool saveTeacherAccount(const std::string& teacherId, const std::string& password);