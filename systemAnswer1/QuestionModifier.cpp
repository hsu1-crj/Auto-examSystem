#include "QuestionModifier.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

QuestionModifier::QuestionModifier(const std::string& fname) : filename(fname) {
    // Load all questions from file
    questions = Question::loadFromFile(filename);
}
// ��ӵ�QuestionModifier���еĵ��Ժ���
void QuestionModifier::listAllQuestionIds() {
    std::cout << "�ļ��е���ĿID�б�:" << std::endl;
    for (const auto& q : questions) {
        std::cout << "ID: [" << q.getId() << "]" << std::endl;
    }
}
bool QuestionModifier::modifyQuestion() {
    listAllQuestionIds();
    std::string id;
    std::cout << "������Ҫ�޸ĵ���ĿID: ";
    std::getline(std::cin, id);


    int index = findQuestionIndex(id);
    if (index == -1) {
        std::cout << "δ�ҵ�IDΪ " << id << " ����Ŀ!" << std::endl;
        return false;
    }

    // Display the current question
    displayQuestion(questions[index]);

    // Ask which part to modify
    std::cout << "\n��ѡ��Ҫ�޸ĵĲ��֣�" << std::endl;
    std::cout << "1. ���" << std::endl;
    std::cout << "2. ѡ��" << std::endl;
    std::cout << "3. ��" << std::endl;
    std::cout << "4. ֪ʶ��" << std::endl;
    std::cout << "5. �Ѷ�" << std::endl;
    std::cout << "0. ����" << std::endl;

    std::string choice;
    std::cout << "������ѡ��: ";
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
        std::cout << "��Чѡ��!" << std::endl;
        return false;
    }

    if (modified) {
        if (saveAllQuestions()) {
            std::cout << "��Ŀ�޸ĳɹ������浽�ļ�!" << std::endl;
            return true;
        }
        else {
            std::cout << "�����ļ�ʧ��!" << std::endl;
            return false;
        }
    }

    return false;
}

bool QuestionModifier::modifyQuestionContent(Question& question) {
    std::string newContent;
    std::cout << "�������µ����: ";
    std::getline(std::cin, newContent);

    if (newContent.empty()) {
        std::cout << "��ɲ���Ϊ��!" << std::endl;
        return false;
    }

    question.setContent(newContent);
    return true;
}

bool QuestionModifier::modifyQuestionOptions(Question& question) {
    std::cout << "��ǰѡ��: ";
    std::vector<std::string> options = question.getOptions();
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << options[i];
        if (i < options.size() - 1) std::cout << " ";
    }
    std::cout << std::endl;

    std::string newOptions;
    std::cout << "�������µ�ѡ��ո�ָ���: ";
    std::getline(std::cin, newOptions);

    if (newOptions.empty()) {
        std::cout << "ѡ���Ϊ��!" << std::endl;
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
    std::cout << "��ǰ��: ";
    std::vector<std::string> answers = question.getAnswer();
    for (size_t i = 0; i < answers.size(); ++i) {
        std::cout << answers[i];
        if (i < answers.size() - 1) std::cout << ",";
    }
    std::cout << std::endl;

    std::string newAnswers;
    std::cout << "�������µĴ𰸣����ŷָ���: ";
    std::getline(std::cin, newAnswers);

    if (newAnswers.empty()) {
        std::cout << "�𰸲���Ϊ��!" << std::endl;
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
    std::cout << "��ǰ֪ʶ��: " << question.getTag() << std::endl;

    std::string newTag;
    std::cout << "�������µ�֪ʶ��: ";
    std::getline(std::cin, newTag);

    if (newTag.empty()) {
        std::cout << "֪ʶ�㲻��Ϊ��!" << std::endl;
        return false;
    }

    // Assuming Question class has setTag() method - add it if it doesn't exist
    question.setTag(newTag);
    return true;
}

bool QuestionModifier::modifyQuestionDifficulty(Question& question) {
    std::cout << "��ǰ�Ѷ�: " << question.getDifficulty() << std::endl;

    std::string difficultyStr;
    std::cout << "�������µ��Ѷ�ϵ����1-5��: ";
    std::getline(std::cin, difficultyStr);

    try {
        int difficulty = std::stoi(difficultyStr);
        if (difficulty < 1 || difficulty > 5) {
            std::cout << "�Ѷ�ϵ��������1-5֮��!" << std::endl;
            return false;
        }

        // Assuming Question class has setDifficulty() method - add it if it doesn't exist
        question.setDifficulty(difficulty);
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "��Ч���Ѷ�ϵ��!" << std::endl;
        return false;
    }
}

int QuestionModifier::findQuestionIndex(const std::string& id) {//Ѱ����ĿID
    for (size_t i = 0; i < questions.size(); ++i) {
        if (questions[i].getId() == id) {
            return static_cast<int>(i);//ת��Ϊint
        }
    }
    return -1;//��
}

void QuestionModifier::displayQuestion(const Question& question) {
    std::cout << "\n��ǰ��Ŀ��Ϣ��" << std::endl;
    std::cout << "[" << question.getId() << "]" << std::endl;
    std::cout << "��ɣ�" << question.getContent() << std::endl;

    std::cout << "ѡ�";
    std::vector<std::string> options = question.getOptions();
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << options[i];
        if (i < options.size() - 1) std::cout << " ";
    }
    std::cout << std::endl;

    std::cout << "�𰸣�";
    std::vector<std::string> answers = question.getAnswer();
    for (size_t i = 0; i < answers.size(); ++i) {
        std::cout << answers[i];
        if (i < answers.size() - 1) std::cout << ",";
    }
    std::cout << std::endl;

    std::cout << "֪ʶ�㣺" << question.getTag() << std::endl;
    std::cout << "�Ѷȣ�" << question.getDifficulty() << std::endl;
}

bool QuestionModifier::saveAllQuestions() {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        return false;
    }

    for (const auto& question : questions) {
        ofs << "[" << question.getId() << "]\n"
            << "��ɣ�" << question.getContent() << "\n"
            << "ѡ�";

        std::vector<std::string> options = question.getOptions();
        for (size_t i = 0; i < options.size(); ++i) {
            ofs << options[i];
            if (i < options.size() - 1) ofs << " ";
        }

        ofs << "\n�𰸣�";
        std::vector<std::string> answers = question.getAnswer();
        for (size_t i = 0; i < answers.size(); ++i) {
            ofs << answers[i];
            if (i < answers.size() - 1) ofs << ",";
        }

        ofs << "\n֪ʶ�㣺" << question.getTag() << "\n"
            << "�Ѷȣ�" << question.getDifficulty() << "\n\n";
    }

    ofs.close();
    return true;
}