#include "Student.h"
#include <fstream>
#include <ctime>
#include <sstream>
#include <iostream>
#include <algorithm>

Student::Student(const std::string& sId, const std::string& sPw)
    : id(sId), password(sPw) {
}

int Student::submitAnswer(const std::vector<std::string>& userAnswer,
    const std::vector<std::string>& correctAnswer) {
    int score = 0;
    int totalQuestions = userAnswer.size();
    int correctCount = 0;

    // 检查每个答案
    for (size_t i = 0; i < userAnswer.size() && i < correctAnswer.size(); i++) {
        // 去除前后空白字符
        std::string userAns = trimString(userAnswer[i]);
        std::string correctAns = trimString(correctAnswer[i]);

        if (userAns == correctAns) {
            correctCount++;
        }
    }

    // 计算得分
    if (totalQuestions > 0) {
        score = (correctCount * 100) / totalQuestions;
    }

    return score;
}

// 将recordScore修改为const函数
void Student::recordScore(int score, const std::string& filename) const {
    std::ofstream ofs;
    bool fileExists = std::ifstream(filename).good();

    // 如果文件不存在，添加表头行
    if (!fileExists) {
        ofs.open(filename);
        if (ofs.is_open()) {
            // 添加列标题，使 Excel 能更好地识别内容
            ofs << "学生ID,分数,测试时间\n";
            ofs.close();
        }
    }

    // 以追加模式打开文件
    ofs.open(filename, std::ios::app);
    if (!ofs.is_open()) return;

    // 获取当前时间
    time_t now = time(nullptr);
    struct tm t;
    localtime_s(&t, &now);
    char timeBuf[64];

    // 格式化时间，使用引号包裹时间字符串
    strftime(timeBuf, sizeof(timeBuf), "\"%Y-%m-%d %H:%M\"", &t);

    // 写入记录（时间字符串带引号）
    ofs << id << "," << score << "," << timeBuf << "\n";
    ofs.close();
}

// 假设需要写入一个文本文件，包含正确率、薄弱知识点、推荐练习ID
void Student::generateReport(const std::string& filename, float correctRate,
    const std::string& weakPoints,
    const std::vector<std::string>& recommendIds)
{
    std::ofstream ofs(filename);
    if (!ofs.is_open()) return;
    ofs << "正确率：" << correctRate * 100 << "%\n"
        << "薄弱知识点：" << weakPoints << "\n"
        << "推荐练习：";
    for (const auto& id : recommendIds) {
        ofs << id << " ";
    }
    ofs << "\n";
    ofs.close();
}

// 从答题错误记录中获取学生的薄弱知识点
std::map<std::string, int> Student::getWeakPoints() const {
    std::map<std::string, int> weakPoints;
    std::ifstream logFile("answer_log.txt");

    if (!logFile.is_open()) {
        return weakPoints;
    }

    std::string line;
    while (std::getline(logFile, line)) {
        std::istringstream ss(line);
        std::vector<std::string> fields;
        std::string field;

        while (std::getline(ss, field, ',')) {
            fields.push_back(field);
        }

        // 确保记录格式正确且是当前学生的记录
        if (fields.size() >= 4 && fields[0] == id) {
            std::string knowledgePoint = fields[2];
            bool correct = (fields[3] == "1");

            if (!correct) {
                // 累计知识点错误数
                weakPoints[knowledgePoint]++;
            }
        }
    }

    return weakPoints;
}

// 辅助函数：去除字符串前后空白字符
std::string Student::trimString(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}
