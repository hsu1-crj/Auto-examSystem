#include "ExamModifier.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <codecvt>
#include <locale>
#include <string>
#include <algorithm>

ExamModifier::ExamModifier(const std::string& questionBank) : questionBankFile(questionBank) {
    // ����������Ŀ
    allQuestions = Question::loadFromFile(questionBankFile);
}

bool ExamModifier::replaceQuestionInExam() {
    // ��ȡ�Ծ�����
    std::string examFilename;
    std::cout << "������Ҫ�޸ĵ��Ծ��ļ��� (�� exam_001.txt): ";
    std::getline(std::cin, examFilename);

    // ��ȡ�Ծ� - ʹ���Զ��巽������ʹ�� Question::loadFromFile
    std::vector<Question> examQuestions = readExamFromFile(examFilename);
    if (examQuestions.empty()) {
        std::cout << "�޷���ȡ�Ծ��ļ����ļ�Ϊ��! �����ļ����Ƿ���ȷ��" << std::endl;
        return false;
    }

    // ��ʾ�Ծ��е���Ŀ
    std::cout << "��ǰ�Ծ����������Ŀ��" << std::endl;
    displayExamQuestions(examQuestions);

    // ��ȡҪ�滻����Ŀ���
    std::string indexStr;
    std::cout << "������Ҫ�滻����Ŀ��� (0-" << examQuestions.size() - 1 << "): ";
    std::getline(std::cin, indexStr);
    int index = std::stoi(indexStr);

    if (index < 0 || index >= examQuestions.size()) {
        std::cout << "��Ч����Ŀ���!" << std::endl;
        return false;
    }

    // ��ʾ��ǰ��Ŀ
    std::cout << "��ǰ��Ŀ: " << std::endl;
    std::cout << "[" << examQuestions[index].getId() << "]" << std::endl;
    std::cout << "��ɣ�" << examQuestions[index].getContent() << std::endl;
    std::cout << "֪ʶ�㣺" << examQuestions[index].getTag() << std::endl;
    std::cout << "�Ѷȣ�" << examQuestions[index].getDifficulty() << std::endl;

    // ��ȡ����ĿID
    std::string newId;
    std::cout << "����������ĿID: ";
    std::getline(std::cin, newId);

    // ������в�������Ŀ
    int newQuestionIndex = -1;
    for (size_t i = 0; i < allQuestions.size(); ++i) {
        if (allQuestions[i].getId() == newId) {
            newQuestionIndex = static_cast<int>(i);
            break;
        }
    }

    if (newQuestionIndex == -1) {
        std::cout << "δ�ҵ�IDΪ " << newId << " ����Ŀ!" << std::endl;
        return false;
    }

    // ��ʾ����Ŀ
    std::cout << "����Ŀ: " << std::endl;
    std::cout << "[" << allQuestions[newQuestionIndex].getId() << "]" << std::endl;
    std::cout << "��ɣ�" << allQuestions[newQuestionIndex].getContent() << std::endl;
    std::cout << "֪ʶ�㣺" << allQuestions[newQuestionIndex].getTag() << std::endl;
    std::cout << "�Ѷȣ�" << allQuestions[newQuestionIndex].getDifficulty() << std::endl;

    // ȷ���滻
    std::string confirm;
    std::cout << "ȷ���滻��Ŀ? (y/n): ";
    std::getline(std::cin, confirm);

    if (confirm != "y" && confirm != "Y") {
        std::cout << "��ȡ���滻!" << std::endl;
        return false;
    }

    // �滻��Ŀ
    examQuestions[index] = allQuestions[newQuestionIndex];

    // �����޸ĺ���Ծ�
    std::string newExamFilename;
    std::cout << "�������µ��Ծ��ļ��� (�����򸲸�ԭ�ļ�): ";
    std::getline(std::cin, newExamFilename);

    if (newExamFilename.empty()) {
        newExamFilename = examFilename;
    }

    if (writeExamToFile(newExamFilename, examQuestions)) {
        std::cout << "�Ծ��ѳɹ����棺" << newExamFilename << std::endl;
        return true;
    }
    else {
        std::cout << "�Ծ���ʧ�ܣ�" << std::endl;
        return false;
    }
}

// �Զ����ȡ������֧�ִ�����ֱ����ʽ
std::vector<Question> ExamModifier::readExamFromFile(const std::string& examFilename) {
    std::vector<Question> questions;
    std::ifstream file(examFilename);  // ��ʹ�ö�����ģʽ����Ϊ�ļ��Ѿ�����ȷ����
    if (!file.is_open()) {
        std::cerr << "�޷����ļ�: " << examFilename << std::endl;
        return questions;
    }

    std::string line;
    std::string id, content, tag;
    std::vector<std::string> options, answers;
    int difficulty = 1;

    while (std::getline(file, line)) {
        // ������
        if (line.empty()) {
            continue;
        }

        // ������ĿID
        if (line.find('[') != std::string::npos && line.find(']') != std::string::npos) {
            // ����Ѿ�������һ��������Ŀ����ӵ��б�
            if (!id.empty()) {
                Question q(id, content, options, answers, tag, difficulty);
                questions.push_back(q);

                // ���ñ���
                id = "";
                content = "";
                tag = "";
                options.clear();
                answers.clear();
                difficulty = 1;
            }

            // ��ȡID
            size_t start = line.find('[') + 1;
            size_t end = line.find(']');
            if (start < end) {
                id = line.substr(start, end - start);
            }
            continue;
        }

        // �������
        if (line.find("��ɣ�") != std::string::npos) {
            content = line.substr(line.find("��ɣ�") + 5);  // ����Ϊ6�������ַ�
            continue;
        }

        // ����ѡ��
        if (line.find("ѡ�") != std::string::npos) {
            std::string optionsLine = line.substr(line.find("ѡ�") + 5);
            std::istringstream iss(optionsLine);
            std::string opt;
            while (iss >> opt) {
                options.push_back(opt);
            }
            continue;
        }

        // �����
        if (line.find("�𰸣�") != std::string::npos) {
            std::string answersLine = line.substr(line.find("�𰸣�") + 5);
            std::istringstream iss(answersLine);
            std::string ans;

            // ʹ�ö��ŷָ���
            while (std::getline(iss, ans, ',')) {
                // ȥ�����ܵĿհ�
                ans.erase(0, ans.find_first_not_of(" \t"));
                ans.erase(ans.find_last_not_of(" \t") + 1);

                if (!ans.empty()) {
                    answers.push_back(ans);
                }
            }
            continue;
        }

        // ����֪ʶ��
        if (line.find("֪ʶ�㣺") != std::string::npos) {
            tag = line.substr(line.find("֪ʶ�㣺") + 7);  // ����Ϊ9�������ַ�
            continue;
        }

        // �����Ѷ�
        if (line.find("�Ѷȣ�") != std::string::npos) {
            std::string difficultyStr = line.substr(line.find("�Ѷȣ�") + 5);
            try {
                difficulty = std::stoi(difficultyStr);
            }
            catch (...) {
                difficulty = 1;
            }
        }
    }

    // ������һ����Ŀ
    if (!id.empty()) {
        Question q(id, content, options, answers, tag, difficulty);
        questions.push_back(q);
    }

    return questions;
}
// ���ԴӶ������ļ��ж�ȡ��Ŀ������ͬ����


bool ExamModifier::writeExamToFile(const std::string& examFilename, const std::vector<Question>& examQuestions) {
    std::ofstream ofs(examFilename);  // ��ʹ�ö�����ģʽ���ļ����Ա��ر����ʽ����
    if (!ofs.is_open()) {
        return false;
    }

    for (const auto& question : examQuestions) {
        ofs << "[" << question.getId() << "]" << std::endl;
        ofs << "��ɣ�" << question.getContent() << std::endl;

        ofs << "ѡ�";
        std::vector<std::string> options = question.getOptions();
        for (size_t i = 0; i < options.size(); ++i) {
            ofs << options[i];
            if (i < options.size() - 1) {
                ofs << " ";
            }
        }
        ofs << std::endl;

        ofs << "�𰸣�";
        std::vector<std::string> answers = question.getAnswer();
        for (size_t i = 0; i < answers.size(); ++i) {
            ofs << answers[i];
            if (i < answers.size() - 1) {
                ofs << ",";
            }
        }
        ofs << std::endl;

        ofs << "֪ʶ�㣺" << question.getTag() << std::endl;
        ofs << "�Ѷȣ�" << question.getDifficulty() << std::endl << std::endl;
    }

    return true;
}

// ���޸�displayExamQuestions����������������ʾ�����е�����
void ExamModifier::displayExamQuestions(const std::vector<Question>& examQuestions) {
    for (size_t i = 0; i < examQuestions.size(); ++i) {
        std::cout << "��Ŀ " << i << ":" << std::endl;
        std::cout << "[" << examQuestions[i].getId() << "]" << std::endl;

        // ֱ����ʾ���ݣ��������κι���
        std::cout << "��ɣ�" << examQuestions[i].getContent() << std::endl;

        std::cout << "ѡ�";
        std::vector<std::string> options = examQuestions[i].getOptions();
        for (size_t j = 0; j < options.size(); ++j) {
            std::cout << options[j];
            if (j < options.size() - 1) std::cout << " ";
        }
        std::cout << std::endl;

        std::cout << "�𰸣�";
        std::vector<std::string> answers = examQuestions[i].getAnswer();
        for (size_t j = 0; j < answers.size(); ++j) {
            std::cout << answers[j];
            if (j < answers.size() - 1) std::cout << ",";
        }
        std::cout << std::endl;

        std::cout << "֪ʶ�㣺" << examQuestions[i].getTag() << std::endl;
        std::cout << "�Ѷȣ�" << examQuestions[i].getDifficulty() << std::endl << std::endl;
    }
}

// ���һ���µĸ����������������ı��е�����
// ���������ɾ����������������޸���
std::string ExamModifier::cleanChineseText(const std::string& text) {
    // ֱ�ӷ���ԭ�ı����������κι���
    return text;
}