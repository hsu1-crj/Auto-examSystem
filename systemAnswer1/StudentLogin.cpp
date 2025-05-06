#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include "Student.h"
#include "Question.h"
#include "StudentLogin.h"

// ���ȫ�ֱ������洢��ǰ��¼��ѧ����Ϣ
std::string g_currentStudentId = "";
std::string g_currentStudentPassword = "";

// ѧ���˻��ļ�
const std::string STUDENT_ACCOUNTS_FILE = "student_accounts.txt";

// ��������ѧ���˻�
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

// ���ѧ���Ƿ���ע��
bool checkStudentExists(const std::string& studentId) {
    auto accounts = loadStudentAccounts();
    return std::find_if(accounts.begin(), accounts.end(),
        [&](const StudentAccount& acc) { return acc.id == studentId; }) != accounts.end();
}

// ��֤�����Ƿ�ƥ��
bool verifyPassword(const std::string& studentId, const std::string& password) {
    auto accounts = loadStudentAccounts();
    auto it = std::find_if(accounts.begin(), accounts.end(),
        [&](const StudentAccount& acc) { return acc.id == studentId; });

    if (it != accounts.end()) {
        return it->password == password;
    }
    return false;
}

// ������ѧ���˻�
bool saveStudentAccount(const std::string& studentId, const std::string& password) {
    std::ofstream file(STUDENT_ACCOUNTS_FILE, std::ios::app);
    if (file.is_open()) {
        file << studentId << "," << password << std::endl;
        file.close();
        return true;
    }
    return false;
}

// ѧ��ע�Ṧ��
bool registerStudent() {
    std::string id, password, confirmPassword;

    std::cout << "======= ѧ��ע�� =======\n";
    std::cout << "������ѧ��: ";
    std::getline(std::cin, id);

    // ���ѧ���Ƿ��Ѵ���
    if (checkStudentExists(id)) {
        std::cout << "��ѧ����ע�ᣬ��ֱ�ӵ�¼��ʹ������ѧ�š�\n";
        return false;
    }

    // ���벢ȷ������
    bool passwordsMatch = false;
    while (!passwordsMatch) {
        std::cout << "���������� (����4λ): ";
        std::getline(std::cin, password);

        if (password.length() < 4) {
            std::cout << "���볤�Ȳ���4λ�����������롣\n";
            continue;
        }

        std::cout << "���ٴ���������ȷ��: ";
        std::getline(std::cin, confirmPassword);

        if (password != confirmPassword) {
            std::cout << "������������벻ƥ�䣬���������롣\n";
        }
        else {
            passwordsMatch = true;
        }
    }

    // �������˻�
    if (saveStudentAccount(id, password)) {
        std::cout << "ע��ɹ�������������ʹ������ѧ�ŵ�¼�ˡ�\n";
        return true;
    }
    else {
        std::cout << "ע��ʧ�ܣ��޷������˻���Ϣ��\n";
        return false;
    }
}

// ѧ����¼����
bool loginStudent() {
    std::string id, password;
    bool loginSuccess = false;
    int attempts = 0;
    const int MAX_ATTEMPTS = 3;

    while (!loginSuccess && attempts < MAX_ATTEMPTS) {
        std::cout << "======= ѧ����¼ =======\n";
        std::cout << "������ѧ�� (������δע���˺� ������ 'r' ע�����˺�): ";
        std::getline(std::cin, id);

        // ����Ƿ�Ҫע�����˺�
        if (id == "r") {
            if (registerStudent()) {
                // ע��ɹ������¿�ʼ��¼����
                attempts = 0;
                continue;
            }
            else {
                // ע��ʧ�ܣ����û�һ������ѡ��Ļ���
                std::cout << "�Ƿ�������Ե�¼? (y/n): ";
                std::string choice;
                std::getline(std::cin, choice);
                if (choice != "y" && choice != "Y") {
                    return false;
                }
                continue;
            }
        }

        // ����˺��Ƿ����
        if (!checkStudentExists(id)) {
            std::cout << "��ѧ����δע�ᡣ����ע���˺š�\n";
            std::cout << "�Ƿ�����ע��? (y/n): ";
            std::string choice;
            std::getline(std::cin, choice);
            if (choice == "y" || choice == "Y") {
                if (registerStudent()) {
                    // ע��ɹ������¿�ʼ��¼����
                    attempts = 0;
                    continue;
                }
            }
            attempts++;
            continue;
        }

        // ��������
        std::cout << "����������: ";
        std::getline(std::cin, password);

        // ��֤����
        if (verifyPassword(id, password)) {
            loginSuccess = true;
            g_currentStudentId = id;
            g_currentStudentPassword = password;
            std::cout << "��¼�ɹ�����ӭ ѧ��:" << id << "\n\n";
        }
        else {
            attempts++;
            std::cout << "����������������롣��ʣ" << (MAX_ATTEMPTS - attempts) << "�λ��ᡣ\n";
        }
    }

    if (!loginSuccess) {
        std::cout << "��¼ʧ�ܴ������࣬�����˳�\n";
    }

    return loginSuccess;
}