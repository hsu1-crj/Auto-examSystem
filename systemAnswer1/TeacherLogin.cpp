#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include "TeacherLogin.h"  // �������Teacher�ṹ�嶨���ͷ�ļ�


// ���ȫ�ֱ������洢��ǰ��¼�Ľ�ʦ��Ϣ
std::string g_currentTeacherId = "";
std::string g_currentTeacherPassword = "";

// ��ʦ�˻��ļ�
const std::string TEACHER_ACCOUNTS_FILE = "teacher_accounts.txt";

// �������н�ʦ�˻�
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

// ����ʦ�Ƿ���ע��
bool checkTeacherExists(const std::string& teacherId) {
    auto accounts = loadTeacherAccounts();
    return std::find_if(accounts.begin(), accounts.end(),
        [&](const TeacherAccount& acc) { return acc.id == teacherId; }) != accounts.end();
}

// ��֤�����Ƿ�ƥ��
bool verifyPasswordTeacher(const std::string& teacherId, const std::string& password) {
    auto accounts = loadTeacherAccounts();
    auto it = std::find_if(accounts.begin(), accounts.end(),
        [&](const TeacherAccount& acc) { return acc.id == teacherId; });

    if (it != accounts.end()) {
        return it->password == password;
    }
    return false;
}

// �����½�ʦ�˻�
bool saveTeacherAccount(const std::string& teacherId, const std::string& password) {
    std::ofstream file(TEACHER_ACCOUNTS_FILE, std::ios::app);
    if (file.is_open()) {
        file << teacherId << "," << password << std::endl;
        file.close();
        return true;
    }
    return false;
}

// ��ʦע�Ṧ��
bool registerTeacher() {
    std::string id, password, confirmPassword;

    std::cout << "======= ��ʦע�� =======\n";
    std::cout << "�������ʦ����: ";
    std::getline(std::cin, id);

    // ����ʦ�����Ƿ��Ѵ���
    if (checkTeacherExists(id)) {
        std::cout << "�ý�ʦ������ע�ᣬ��ֱ�ӵ�¼��ʹ���������š�\n";
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
    if (saveTeacherAccount(id, password)) {
        std::cout << "ע��ɹ�������������ʹ�����Ľ�ʦ���ŵ�¼�ˡ�\n";
        return true;
    }
    else {
        std::cout << "ע��ʧ�ܣ��޷������˻���Ϣ��\n";
        return false;
    }
}

// ��ʦ��¼����
bool loginTeacher() {
    std::string id, password;
    bool loginSuccess = false;
    int attempts = 0;
    const int MAX_ATTEMPTS = 3;

    while (!loginSuccess && attempts < MAX_ATTEMPTS) {
        std::cout << "======= ��ʦ��¼ =======\n";
        std::cout << "�������ʦ���� (������δע���˺� ������ 'r' ע�����˺�): ";
        std::getline(std::cin, id);

        // ����Ƿ�Ҫע�����˺�
        if (id == "r") {
            if (registerTeacher()) {
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
        if (!checkTeacherExists(id)) {
            std::cout << "�ý�ʦ������δע�ᡣ����ע���˺š�\n";
            std::cout << "�Ƿ�����ע��? (y/n): ";
            std::string choice;
            std::getline(std::cin, choice);
            if (choice == "y" || choice == "Y") {
                if (registerTeacher()) {
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
        if (verifyPasswordTeacher(id, password)) {
            loginSuccess = true;
            g_currentTeacherId = id;
            g_currentTeacherPassword = password;
            std::cout << "��¼�ɹ�����ӭ ��ʦ����:" << id << "\n\n";
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