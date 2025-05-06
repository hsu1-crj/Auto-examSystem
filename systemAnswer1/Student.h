#pragma once
#include <string>
#include <vector>
#include "Question.h"

class Student {
private:
    std::string id;
    std::string password;

public:
    Student() : id(""), password("") {}
    Student(const std::string& sId, const std::string& sPw);

    // ���Ⲣ����÷�
    int submitAnswer(const std::vector<std::string>& userAnswer,
        const std::vector<std::string>& correctAnswer);

    // ��¼�ɼ��� scores.csv
    void recordScore(int score, const std::string& filename) const;

    // ����ѧϰ���棨ʾ���������ɽ����ʷ�ɼ����з�����
    void generateReport(const std::string& filename, float correctRate,
        const std::string& weakPoints,
        const std::vector<std::string>& recommendIds);
};