#pragma once
#include "Question.h"
#include <vector>
#include <string>

class ExamGenerator {
public:
    // ����֪ʶ�㡢�Ѷȷ�Χ���������ѡ��
    std::vector<Question> generateExam(const std::vector<Question>& allQuestions,
        const std::string& tagFilter,
        int minDiff, int maxDiff,
        int count);

    // ������Ŀ���ļ�
    void exportExam(const std::vector<Question>& exam, const std::string& filename);
};