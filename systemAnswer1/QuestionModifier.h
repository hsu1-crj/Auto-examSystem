#pragma once
#ifndef QUESTION_MODIFIER_H
#define QUESTION_MODIFIER_H

#include <string>
#include <vector>
#include "Question.h" // Assuming this header exists with Question class definition

class QuestionModifier {
public:
    // Constructor
    QuestionModifier(const std::string& filename = "questions.txt");

    // Main method to modify a question
    bool modifyQuestion();

    // Helper methods for modification
    bool modifyQuestionContent(Question& question);
    bool modifyQuestionOptions(Question& question);
    bool modifyQuestionAnswer(Question& question);
    bool modifyQuestionTag(Question& question);
    bool modifyQuestionDifficulty(Question& question);

    // Save all questions back to file
    bool saveAllQuestions();
    void listAllQuestionIds();

private:
    std::string filename;
    std::vector<Question> questions;

    // Find a question by ID
    int findQuestionIndex(const std::string& id);

    // Display a question
    void displayQuestion(const Question& question);
};

#endif // QUESTION_MODIFIER_H