#pragma once
#include <string>

// ����ȫ�ֱ��� (ʹ�� extern �ؼ���)
extern std::string g_currentStudentId;
extern std::string g_currentStudentPassword;
// ѧ���˻��ṹ
struct StudentAccount {
    std::string id;
    std::string password;
};

// ��������
bool loginStudent();
bool registerStudent();
bool checkStudentExists(const std::string& studentId);
bool verifyPassword(const std::string& studentId, const std::string& password);
std::vector<StudentAccount> loadStudentAccounts();
bool saveStudentAccount(const std::string& studentId, const std::string& password);