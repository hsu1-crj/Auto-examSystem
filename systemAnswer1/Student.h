#pragma once
#include <string>
#include <vector>
#include <map>

class Student {
private:
    std::string id;
    std::string password;

public:
    Student(const std::string& sId, const std::string& sPw);

    // �ύ�𰸲�����÷�
    int submitAnswer(const std::vector<std::string>& userAnswer,
        const std::vector<std::string>& correctAnswer);

    // ��¼�ɼ�
    void recordScore(int score, const std::string& filename) const;

    // ����ѧϰ����
    void generateReport(const std::string& filename, float correctRate,
        const std::string& weakPoints,
        const std::vector<std::string>& recommendIds);

    // ��ȡ����֪ʶ��
    std::map<std::string, int> getWeakPoints() const;

    // ����������ȥ���ַ���ǰ��հ��ַ�
    static std::string trimString(const std::string& str);
};

// ȫ�ֱ��������ڴ洢��ǰ��¼��ѧ����Ϣ
extern std::string g_currentStudentId;
extern std::string g_currentStudentPassword;
