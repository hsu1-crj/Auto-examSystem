#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>

class ReportGenerator {
public:
    static bool isManualGeneration; // 标记是否是手动生成报告

    static void generateWeeklyReport(const std::string& studentId);

private:
    static std::map<std::string, int> countErrorsByTag(const std::string& studentId);
    static std::vector<std::string> getRecommendedExercises(
        const std::map<std::string, int>& errorsByTag,
        const std::string& studentId = "",
        int count = 3);
    static float calculateCorrectRate(const std::string& studentId);
    static bool isTimeToGenerateReport(); // 检查是否是周一

    // 辅助函数
    static void loadQuestionDatabase(std::map<std::string, std::vector<std::string>>& tagToQuestions);
    static std::vector<std::string> selectRandomQuestions(const std::vector<std::string>& questions, int count);
    static std::vector<std::string> getRandomQuestions(
        const std::map<std::string, std::vector<std::string>>& tagToQuestions,
        int count,
        const std::set<std::string>& doneQuestions = {});
    static std::string trimString(const std::string& str);
    static std::set<std::string> getStudentDoneQuestions(const std::string& studentId);
};