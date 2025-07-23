#include "QuestionModifier.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

QuestionModifier::QuestionModifier(const std::string& fname) : filename(fname) {
    // Load all questions from file
    questions = Question::loadFromFile(filename);
}
// 添加到QuestionModifier类中的调试函数
void QuestionModifier::listAllQuestionIds() {
    std::cout << "文件中的题目ID列表:" << std::endl;
    for (const auto& q : questions) {
        std::cout << "ID: [" << q.getId() << "]" << std::endl;
    }
}
bool QuestionModifier::modifyQuestion() {
    listAllQuestionIds();
    std::string id;
    std::cout << "请输入要修改的题目ID: ";
    std::getline(std::cin, id);


    int index = findQuestionIndex(id);
    if (index == -1) {
        std::cout << "未找到ID为 " << id << " 的题目!" << std::endl;
        return false;
    }

    // Display the current question
    displayQuestion(questions[index]);

    // Ask which part to modify
    std::cout << "\n请选择要修改的部分：" << std::endl;
    std::cout << "1. 题干" << std::endl;
    std::cout << "2. 选项" << std::endl;
    std::cout << "3. 答案" << std::endl;
    std::cout << "4. 知识点" << std::endl;
    std::cout << "5. 难度" << std::endl;
    std::cout << "0. 返回" << std::endl;

    std::string choice;
    std::cout << "请输入选择: ";
    std::getline(std::cin, choice);

    bool modified = false;

    switch (std::stoi(choice)) {
    case 1:
        modified = modifyQuestionContent(questions[index]);
        break;
    case 2:
        modified = modifyQuestionOptions(questions[index]);
        break;
    case 3:
        modified = modifyQuestionAnswer(questions[index]);
        break;
    case 4:
        modified = modifyQuestionTag(questions[index]);
        break;
    case 5:
        modified = modifyQuestionDifficulty(questions[index]);
        break;
    case 0:
        return false;
    default:
        std::cout << "无效选择!" << std::endl;
        return false;
    }

    if (modified) {
        if (saveAllQuestions()) {
            std::cout << "题目修改成功并保存到文件!" << std::endl;
            return true;
        }
        else {
            std::cout << "保存文件失败!" << std::endl;
            return false;
        }
    }

    return false;
}

bool QuestionModifier::modifyQuestionContent(Question& question) {
    std::string newContent;
    std::cout << "请输入新的题干: ";
    std::getline(std::cin, newContent);

    if (newContent.empty()) {
        std::cout << "题干不能为空!" << std::endl;
        return false;
    }

    question.setContent(newContent);
    return true;
}

bool QuestionModifier::modifyQuestionOptions(Question& question) {
    std::cout << "当前选项: ";
    std::vector<std::string> options = question.getOptions();
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << options[i];
        if (i < options.size() - 1) std::cout << " ";
    }
    std::cout << std::endl;

    std::string newOptions;
    std::cout << "请输入新的选项（空格分隔）: ";
    std::getline(std::cin, newOptions);

    if (newOptions.empty()) {
        std::cout << "选项不能为空!" << std::endl;
        return false;
    }

    std::vector<std::string> newOptionsList;
    std::istringstream iss(newOptions);
    for (std::string opt; iss >> opt; ) {
        newOptionsList.push_back(opt);
    }

    question.setOptions(newOptionsList);
    return true;
}

bool QuestionModifier::modifyQuestionAnswer(Question& question) {
    std::cout << "当前答案: ";
    std::vector<std::string> answers = question.getAnswer();
    for (size_t i = 0; i < answers.size(); ++i) {
        std::cout << answers[i];
        if (i < answers.size() - 1) std::cout << ",";
    }
    std::cout << std::endl;

    std::string newAnswers;
    std::cout << "请输入新的答案（逗号分隔）: ";
    std::getline(std::cin, newAnswers);

    if (newAnswers.empty()) {
        std::cout << "答案不能为空!" << std::endl;
        return false;
    }

    std::vector<std::string> newAnswerList;
    std::stringstream ss(newAnswers);
    for (std::string seg; std::getline(ss, seg, ','); ) {
        newAnswerList.push_back(seg);
    }

    question.setAnswer(newAnswerList);
    return true;
}

bool QuestionModifier::modifyQuestionTag(Question& question) {
    std::cout << "当前知识点: " << question.getTag() << std::endl;

    std::string newTag;
    std::cout << "请输入新的知识点: ";
    std::getline(std::cin, newTag);

    if (newTag.empty()) {
        std::cout << "知识点不能为空!" << std::endl;
        return false;
    }

    // Assuming Question class has setTag() method - add it if it doesn't exist
    question.setTag(newTag);
    return true;
}

bool QuestionModifier::modifyQuestionDifficulty(Question& question) {
    std::cout << "当前难度: " << question.getDifficulty() << std::endl;

    std::string difficultyStr;
    std::cout << "请输入新的难度系数（1-5）: ";
    std::getline(std::cin, difficultyStr);

    try {
        int difficulty = std::stoi(difficultyStr);
        if (difficulty < 1 || difficulty > 5) {
            std::cout << "难度系数必须在1-5之间!" << std::endl;
            return false;
        }

        // Assuming Question class has setDifficulty() method - add it if it doesn't exist
        question.setDifficulty(difficulty);
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "无效的难度系数!" << std::endl;
        return false;
    }
}

int QuestionModifier::findQuestionIndex(const std::string& id) {//寻找题目ID
    for (size_t i = 0; i < questions.size(); ++i) {
        if (questions[i].getId() == id) {
            return static_cast<int>(i);//转换为int
        }
    }
    return -1;//无
}

void QuestionModifier::displayQuestion(const Question& question) {
    std::cout << "\n当前题目信息：" << std::endl;
    std::cout << "[" << question.getId() << "]" << std::endl;
    std::cout << "题干：" << question.getContent() << std::endl;

    std::cout << "选项：";
    std::vector<std::string> options = question.getOptions();
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << options[i];
        if (i < options.size() - 1) std::cout << " ";
    }
    std::cout << std::endl;

    std::cout << "答案：";
    std::vector<std::string> answers = question.getAnswer();
    for (size_t i = 0; i < answers.size(); ++i) {
        std::cout << answers[i];
        if (i < answers.size() - 1) std::cout << ",";
    }
    std::cout << std::endl;

    std::cout << "知识点：" << question.getTag() << std::endl;
    std::cout << "难度：" << question.getDifficulty() << std::endl;
}

bool QuestionModifier::saveAllQuestions() {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        return false;
    }

    for (const auto& question : questions) {
        ofs << "[" << question.getId() << "]\n"
            << "题干：" << question.getContent() << "\n"
            << "选项：";

        std::vector<std::string> options = question.getOptions();
        for (size_t i = 0; i < options.size(); ++i) {
            ofs << options[i];
            if (i < options.size() - 1) ofs << " ";
        }

        ofs << "\n答案：";
        std::vector<std::string> answers = question.getAnswer();
        for (size_t i = 0; i < answers.size(); ++i) {
            ofs << answers[i];
            if (i < answers.size() - 1) ofs << ",";
        }

        ofs << "\n知识点：" << question.getTag() << "\n"
            << "难度：" << question.getDifficulty() << "\n\n";
    }

    ofs.close();
    return true;
}