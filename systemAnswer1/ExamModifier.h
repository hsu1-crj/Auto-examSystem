#ifndef EXAM_MODIFIER_H
#define EXAM_MODIFIER_H

#include <string>
#include <vector>
#include "Question.h"  // ��������Question�ඨ��

class ExamModifier {
public:
    // ���캯��
    ExamModifier(const std::string& questionBank = "questions.txt");

    // �滻�Ծ��е���Ŀ
    bool replaceQuestionInExam();

private:
    std::string questionBankFile;
    std::vector<Question> allQuestions;

    // ���ļ��ж�ȡ�Ծ�
    std::vector<Question> readExamFromFile(const std::string& examFilename);

    // ���ԴӶ������ļ��ж�ȡ��Ŀ
    std::vector<Question> readExamFromBinaryFile(const std::string& examFilename);

    // ���޸ĺ���Ծ�д���ļ�
    bool writeExamToFile(const std::string& examFilename, const std::vector<Question>& examQuestions);

    // ��ʾ�Ծ��е���Ŀ
    void displayExamQuestions(const std::vector<Question>& examQuestions);
    std::string cleanChineseText(const std::string& text);
};


#endif // EXAM_MODIFIER_H