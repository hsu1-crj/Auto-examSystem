#ifndef EXAM_MODIFIER_H
#define EXAM_MODIFIER_H

#include <string>
#include <vector>
#include "Question.h"  // 假设已有Question类定义

class ExamModifier {
public:
    // 构造函数
    ExamModifier(const std::string& questionBank = "questions.txt");

    // 替换试卷中的题目
    bool replaceQuestionInExam();

private:
    std::string questionBankFile;
    std::vector<Question> allQuestions;

    // 从文件中读取试卷
    std::vector<Question> readExamFromFile(const std::string& examFilename);

    // 尝试从二进制文件中读取题目
    std::vector<Question> readExamFromBinaryFile(const std::string& examFilename);

    // 将修改后的试卷写入文件
    bool writeExamToFile(const std::string& examFilename, const std::vector<Question>& examQuestions);

    // 显示试卷中的题目
    void displayExamQuestions(const std::vector<Question>& examQuestions);
    std::string cleanChineseText(const std::string& text);
};


#endif // EXAM_MODIFIER_H