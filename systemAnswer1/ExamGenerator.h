#pragma once
#include "Question.h"
#include <vector>
#include <string>

class ExamGenerator {
public:
    // 根据知识点、难度范围、数量随机选题
    std::vector<Question> generateExam(const std::vector<Question>& allQuestions,
        const std::string& tagFilter,
        int minDiff, int maxDiff,
        int count);

    // 导出题目到文件
    void exportExam(const std::vector<Question>& exam, const std::string& filename);
};