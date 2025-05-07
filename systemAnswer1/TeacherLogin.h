#pragma once
#include <string>
#include <vector>

// ����ȫ�ֱ��� (ʹ�� extern �ؼ���)
extern std::string g_currentTeacherId;
extern std::string g_currentTeacherPassword;

// ��ʦ�˻��ṹ
struct TeacherAccount {
    std::string id;
    std::string password;
};

// ��������
bool loginTeacher();
bool registerTeacher();
bool checkTeacherExists(const std::string& teacherId);
bool verifyPasswordTeacher(const std::string& teacherId, const std::string& password);
std::vector<TeacherAccount> loadTeacherAccounts();
bool saveTeacherAccount(const std::string& teacherId, const std::string& password);