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

    // 答题并计算得分
    int submitAnswer(const std::vector<std::string>& userAnswer,
        const std::vector<std::string>& correctAnswer);

    // 记录成绩到 scores.csv
    void recordScore(int score, const std::string& filename) const;

    // 生成学习报告（示例函数，可结合历史成绩进行分析）
    void generateReport(const std::string& filename, float correctRate,
        const std::string& weakPoints,
        const std::vector<std::string>& recommendIds);
};