#include "Student.h"
#include <fstream>
#include <ctime>
#include <sstream>

Student::Student(const std::string& sId, const std::string& sPw)
    : id(sId), password(sPw) {
}

int Student::submitAnswer(const std::vector<std::string>& userAnswer,
    const std::vector<std::string>& correctAnswer) {
    int score = 0;

    // Check if the arrays have the same size
    if (userAnswer.size() == correctAnswer.size()) {
        bool allMatch = true;

        // Compare each answer at the same position
        for (size_t i = 0; i < userAnswer.size(); i++) {
            if (userAnswer[i] != correctAnswer[i]) {
                allMatch = false;
                break;
            }
        }

        if (allMatch) score = 100; // Full score for all correct answers
    }

    return score;
}
// 将recordScore修改为const方法
void Student::recordScore(int score, const std::string& filename) const {
    std::ofstream ofs;
    bool fileExists = std::ifstream(filename).good();

    // 如果文件不存在，则添加标题行
    if (!fileExists) {
        ofs.open(filename);
        if (ofs.is_open()) {
            // 添加列标题，使 Excel 能更好地识别列
            ofs << "学生ID,分数,答题时间\n";
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

    // 写入记录，时间字符串带引号
    ofs << id << "," << score << "," << timeBuf << "\n";
    ofs.close();
}

// 简单示例：写入一个文本文件，包含正确率、薄弱知识点、推荐练习ID
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