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
    std::ifstream file(examFilename);
    if (!file.is_open()) {
        std::cerr << "�޷����ļ�: " << examFilename << std::endl;
        return questions;
    }

    std::string line;
    std::string id, content, tag;
    std::vector<std::string> options, answers;
    int difficulty = 1;

    while (std::getline(file, line)) {
        // ȥ��BOM����������
        if (!line.empty() && line.size() >= 3 &&
            (unsigned char)line[0] == 0xEF &&
            (unsigned char)line[1] == 0xBB &&
            (unsigned char)line[2] == 0xBF) {
            line = line.substr(3);
        }

        // ��������
        if (line.empty()) continue;

        // ����ID��
        if (line.find('[') != std::string::npos && line.find(']') != std::string::npos) {
            // ����Ѿ�����һ����������Ŀ����ӵ��б���
            if (!id.empty() && !content.empty()) {
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
        }
        // ���������
        else if (line.find("��ɣ�") != std::string::npos) {
            content = line.substr(line.find("��ɣ�") + 5);
        }
        // ����ѡ����
        else if (line.find("ѡ�") != std::string::npos) {
            std::string optLine = line.substr(line.find("ѡ�") + 5);
            std::istringstream iss(optLine);
            options.clear();
            std::string opt;
            while (iss >> opt) {
                options.push_back(opt);
            }
        }
        // �������
        else if (line.find("�𰸣�") != std::string::npos) {
            std::string ansLine = line.substr(line.find("�𰸣�") + 5);
            std::istringstream iss(ansLine);
            answers.clear();
            std::string ans;
            while (std::getline(iss, ans, ',')) {
                answers.push_back(ans);
            }
        }
        // ����֪ʶ����
        else if (line.find("֪ʶ�㣺") != std::string::npos) {
            tag = line.substr(line.find("֪ʶ�㣺") + 8);
        }
        // �����Ѷ���
        else if (line.find("�Ѷȣ�") != std::string::npos) {
            std::string diffStr = line.substr(line.find("�Ѷȣ�") + 5);
            try {
                difficulty = std::stoi(diffStr);
            }
            catch (...) {
                difficulty = 1;
            }
        }
    }

    // ������һ����Ŀ
    if (!id.empty() && !content.empty()) {
        Question q(id, content, options, answers, tag, difficulty);
        questions.push_back(q);
    }

    file.close();

    // �����ȻΪ�գ������ö����Ʒ�ʽ��ȡ������
    if (questions.empty()) {
        questions = readExamFromBinaryFile(examFilename);
    }

    return questions;
}

// ���ԴӶ������ļ��ж�ȡ��Ŀ������ͬ����
std::vector<Question> ExamModifier::readExamFromBinaryFile(const std::string& examFilename) {
    std::vector<Question> questions;
    std::ifstream file(examFilename, std::ios::binary);
    if (!file.is_open()) {
        return questions;
    }

    // ��ȡ�����ļ�����
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // ���BOM���Ƴ�
    bool hasBOM = false;
    if (content.size() >= 3 &&
        (unsigned char)content[0] == 0xEF &&
        (unsigned char)content[1] == 0xBB &&
        (unsigned char)content[2] == 0xBF) {
        content = content.substr(3);
        hasBOM = true;
    }

    // ��������
    std::istringstream iss(content);
    std::string line;
    std::string id, questionContent, tag;
    std::vector<std::string> options, answers;
    int difficulty = 1;

    while (std::getline(iss, line)) {
        // ��������
        if (line.empty()) continue;

        // ����ID��
        if (line.find('[') != std::string::npos && line.find(']') != std::string::npos) {
            // ����Ѿ�����һ����������Ŀ����ӵ��б���
            if (!id.empty() && !questionContent.empty()) {
                Question q(id, questionContent, options, answers, tag, difficulty);
                questions.push_back(q);

                // ���ñ���
                id = "";
                questionContent = "";
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
        }
        // ���Բ���"��ɣ�"�������Ƶ����ݣ���ʹ������Ҳ����ƥ��
        else if (line.find("���") != std::string::npos) {
            size_t pos = line.find("���");
            questionContent = line.substr(pos + 2); // ����"���"�Ϳ��ܵķָ���
        }
        // ���Բ���"ѡ�"�������Ƶ�����
        else if (line.find("ѡ��") != std::string::npos) {
            size_t pos = line.find("ѡ��");
            std::string optLine = line.substr(pos + 2); // ����"ѡ��"�Ϳ��ܵķָ���
            std::istringstream optIss(optLine);
            options.clear();
            std::string opt;
            while (optIss >> opt) {
                options.push_back(opt);
            }
        }
        // ���Բ���"�𰸣�"�������Ƶ�����
        else if (line.find("��") != std::string::npos) {
            size_t pos = line.find("��");
            std::string ansLine = line.substr(pos + 2); // ����"��"�Ϳ��ܵķָ���
            std::istringstream ansIss(ansLine);
            answers.clear();
            std::string ans;
            while (std::getline(ansIss, ans, ',')) {
                answers.push_back(ans);
            }
        }
        // ���Բ���"֪ʶ�㣺"�������Ƶ�����
        else if (line.find("֪ʶ��") != std::string::npos) {
            size_t pos = line.find("֪ʶ��");
            tag = line.substr(pos + 3); // ����"֪ʶ��"�Ϳ��ܵķָ���
        }
        // ���Բ���"�Ѷȣ�"�������Ƶ�����
        else if (line.find("�Ѷ�") != std::string::npos) {
            size_t pos = line.find("�Ѷ�");
            std::string diffStr = line.substr(pos + 2); // ����"�Ѷ�"�Ϳ��ܵķָ���
            // ������ȡ����
            for (char c : diffStr) {
                if (isdigit(c)) {
                    difficulty = c - '0';
                    break;
                }
            }
        }
    }

    // ������һ����Ŀ
    if (!id.empty() && !questionContent.empty()) {
        Question q(id, questionContent, options, answers, tag, difficulty);
        questions.push_back(q);
    }

    return questions;
}

bool ExamModifier::writeExamToFile(const std::string& examFilename, const std::vector<Question>& examQuestions) {
    // ʹ�ñ�׼��ofstream��ANSI����д�룬��ԭʼ���ɷ�ʽ����һ��
    std::ofstream ofs(examFilename);
    if (!ofs.is_open()) {
        return false;
    }

    for (const auto& question : examQuestions) {
        ofs << "[" << question.getId() << "]\n"
            << "��ɣ�" << question.getContent() << "\n"
            << "ѡ�";

        // ����ѡ��
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

// ���޸�displayExamQuestions����������������ʾ�����е�����
void ExamModifier::displayExamQuestions(const std::vector<Question>& examQuestions) {
    for (size_t i = 0; i < examQuestions.size(); ++i) {
        std::cout << "��Ŀ " << i << ":" << std::endl;
        std::cout << "[" << examQuestions[i].getId() << "]" << std::endl;

        // ��������е�����
        std::string content = examQuestions[i].getContent();
        std::string cleanedContent = cleanChineseText(content);
        std::cout << "��ɣ�" << cleanedContent << std::endl;

        // ����ѡ���е�����
        std::cout << "ѡ�";
        std::vector<std::string> options = examQuestions[i].getOptions();
        for (size_t j = 0; j < options.size(); ++j) {
            std::string cleanedOption = cleanChineseText(options[j]);
            std::cout << cleanedOption;
            if (j < options.size() - 1) std::cout << " ";
        }
        std::cout << std::endl;

        // ������е�����
        std::cout << "�𰸣�";
        std::vector<std::string> answers = examQuestions[i].getAnswer();
        for (size_t j = 0; j < answers.size(); ++j) {
            std::string cleanedAnswer = cleanChineseText(answers[j]);
            std::cout << cleanedAnswer;
            if (j < answers.size() - 1) std::cout << ",";
        }
        std::cout << std::endl;

        // ����֪ʶ���е�����
        std::string tag = examQuestions[i].getTag();
        std::string cleanedTag = cleanChineseText(tag);
        std::cout << "֪ʶ�㣺" << cleanedTag << std::endl;

        std::cout << "�Ѷȣ�" << examQuestions[i].getDifficulty() << std::endl << std::endl;
    }
}

// ���һ���µĸ����������������ı��е�����
std::string ExamModifier::cleanChineseText(const std::string& text) {
    std::string cleaned;
    for (size_t i = 0; i < text.length(); ++i) {
        unsigned char c = text[i];

        // ����ǿɼ���ASCII�ַ�������
        if (c >= 32 && c <= 126) {
            cleaned.push_back(c);
            continue;
        }

        // �����C++, C#�ȱ�����Գ�������ǰ�����룬ֱ������
        if (i + 1 < text.length() && text[i + 1] == '+') {
            if (text.substr(i + 1, 2) == "++") {
                cleaned.append("C++");
                i += 2; // ���� "++"
                continue;
            }
        }

        // ����ǳ���ѡ��ǰ�����루����A.B.C.D.ǰ�����룩���滻Ϊ��ȷ��ѡ���ʶ
        if (i + 1 < text.length() && text[i + 1] == '.') {
            if (i + 2 < text.length()) {
                char nextChar = text[i + 2];
                // ���������������ѡ���".const"�е�const
                if (isalpha(nextChar) || isdigit(nextChar)) {
                    // �ж����ǵڼ���ѡ��
                    size_t optionPos = cleaned.length();
                    int optionIndex = -1;

                    // �ҳ��Ѿ������ѡ������
                    size_t pos = 0;
                    while ((pos = cleaned.find(".", pos)) != std::string::npos) {
                        optionIndex++;
                        pos++;
                    }

                    // ����ѡ�����������ȷ��ѡ���ʶ
                    switch (optionIndex + 1) {
                    case 1: cleaned.append("A."); break;
                    case 2: cleaned.append("B."); break;
                    case 3: cleaned.append("C."); break;
                    case 4: cleaned.append("D."); break;
                    case 5: cleaned.append("E."); break;
                    default: cleaned.append("?."); break;
                    }
                    i += 1; // ���� "."
                    continue;
                }
            }
        }

        // ����������������ģʽ
        // ���������Ӹ��������ģʽʶ����滻����

        // Ĭ��������������ɼ����ַ�
        if (c < 32 || c > 126) {
            continue;
        }
    }

    return cleaned;
}