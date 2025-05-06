#include "ExamGenerator.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <random>
#include "Question.h"

// trim utility to remove leading/trailing whitespace
static std::string trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace((unsigned char)*start)) {
        start++;
    }
    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace((unsigned char)*end));
    return std::string(start, end + 1);
}

std::vector<Question> ExamGenerator::generateExam(const std::vector<Question>& allQuestions,
    const std::string& tagFilter,
    int minDiff, int maxDiff,
    int count)
{
    std::string trimmedFilter = trim(tagFilter);
    std::vector<Question> filtered;

    // Debug: show what we loaded
    std::cout << "[Debug] All loaded questions:\n";
    for (auto& q : allQuestions) {
        std::cout << "ID=" << q.getId()
            << ", Tag=" << q.getTag()
            << ", Diff=" << q.getDifficulty() << "\n";
    }

    // Filter questions
    for (auto& q : allQuestions) {
        std::string ttag = trim(q.getTag());
        if (ttag == trimmedFilter &&
            q.getDifficulty() >= minDiff &&
            q.getDifficulty() <= maxDiff)
        {
            filtered.push_back(q);
        }
    }

    // Shuffle
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(filtered.begin(), filtered.end(), g);

    // Truncate
    if (count > (int)filtered.size()) {
        count = (int)filtered.size();
    }

    // Debug info
    std::cout << "[Debug] Filtered questions count: " << filtered.size() << "\n";

    std::vector<Question> result(filtered.begin(), filtered.begin() + count);
    return result;
}

void ExamGenerator::exportExam(const std::vector<Question>& exam, const std::string& filename) {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return;
    }

    for (auto& q : exam) {
        ofs << "[" << q.getId() << "]\n"
            << "题干：" << q.getContent() << "\n"
			<< "选项：";
        for (const auto& option : q.getOptions()) {
            ofs << option << " ";
        }
           ofs << "\n知识点：" << q.getTag() << "\n"
            << "难度：" << q.getDifficulty() << "\n\n";
    }
    ofs.close();
}