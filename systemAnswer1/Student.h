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

    // 提交答案并计算得分
    int submitAnswer(const std::vector<std::string>& userAnswer,
        const std::vector<std::string>& correctAnswer);

    // 记录成绩
    void recordScore(int score, const std::string& filename) const;

    // 生成学习报告
    void generateReport(const std::string& filename, float correctRate,
        const std::string& weakPoints,
        const std::vector<std::string>& recommendIds);

    // 获取薄弱知识点
    std::map<std::string, int> getWeakPoints() const;

    // 辅助函数：去除字符串前后空白字符
    static std::string trimString(const std::string& str);
};

// 全局变量，用于存储当前登录的学生信息
extern std::string g_currentStudentId;
extern std::string g_currentStudentPassword;
