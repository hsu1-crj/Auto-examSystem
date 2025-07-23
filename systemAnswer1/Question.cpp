#include "Question.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> // for std::find

Question::Question() : difficulty(1) {}

Question::Question(const std::string& qId, const std::string& qContent,
    const std::vector<std::string>& qOptions,
    const std::vector<std::string>& qAnswer,
    const std::string& qTag, int qDifficulty)
    : id(qId), content(qContent), options(qOptions), answer(qAnswer),
    tag(qTag), difficulty(qDifficulty) {
}

void Question::saveToFile(const std::string& filename) const {
    std::ofstream ofs(filename, std::ios::app);
    if (ofs.is_open()) {
        ofs << "[" << id << "]\n"
            << "��ɣ�" << content << "\n"
            << "ѡ�";
        for (size_t i = 0; i < options.size(); ++i) {
            ofs << options[i];
            if (i < options.size() - 1) ofs << " ";
        }
        ofs << "\n�𰸣�";
        for (size_t i = 0; i < answer.size(); ++i) {
            ofs << answer[i];
            if (i < answer.size() - 1) ofs << ",";
        }
        ofs << "\n֪ʶ�㣺" << tag << "\n"
            << "�Ѷȣ�" << difficulty << "\n\n";
    }
    ofs.close();
}

std::vector<Question> Question::loadFromFile(const std::string& filename) {
    std::vector<Question> questions;
    std::ifstream ifs(filename);//����ļ���
    if (!ifs.is_open()) {
        std::cerr << "�޷�������ļ�: " << filename << std::endl;
        return questions;
    }

    std::string line, currentId, content, tag;
    std::vector<std::string> options, ans;
    int difficulty = 1;
    while (std::getline(ifs, line)) {
        if (line.find('[') == 0) {
            // �޸�ID��ȡ��ʽ��ʹ�ø��ɿ��ķ���
            size_t start = line.find('[') + 1;
            size_t end = line.find(']');
            if (end != std::string::npos && start < end) {//�Ƿ��ܲ�ѯ��]
                currentId = line.substr(start, end - start);
            }
        }
        else if (line.rfind("��ɣ�", 0) == 0) {
            content = line.substr(6);
        }
        else if (line.rfind("ѡ�", 0) == 0) {//�ҵ�ѡ�����������
            options.clear();
            std::istringstream iss(line.substr(6));
            std::string opt;
            while (iss >> opt) {
                options.push_back(opt);
            }
        }
        else if (line.rfind("�𰸣�", 0) == 0) {
            ans.clear();
            std::string answers = line.substr(6);
            std::stringstream sstream(answers);
            std::string seg;
            while (std::getline(sstream, seg, ',')) {
                ans.push_back(seg);
            }
        }
        else if (line.rfind("֪ʶ�㣺", 0) == 0) {
            tag = line.substr(8);
        }
        else if (line.rfind("�Ѷȣ�", 0) == 0) {
            difficulty = std::stoi(line.substr(6));
            Question q(currentId, content, options, ans, tag, difficulty);
            questions.push_back(q);
        }
    }
    return questions;
}

Question Question::createFromInput() {
    std::string id, content, tag, difficultyStr;
    int difficulty;
    std::vector<std::string> options, answer;

    std::cout << "��������Ŀ ID: ";
    std::getline(std::cin, id);

    std::cout << "���������: ";
    std::getline(std::cin, content);

    std::cout << "������ѡ��ո�ָ���: ";
    std::string opts;
    std::getline(std::cin, opts);
    std::istringstream iss(opts);//���ո���ȡÿ������
    for (std::string opt; iss >> opt; ) {
        options.push_back(opt);//����ĩβ���Ԫ��
    }

    std::cout << "������𰸣����ŷָ���: ";
    std::string ans;
    std::getline(std::cin, ans);
    std::stringstream sstream(ans);
    for (std::string seg; std::getline(sstream, seg, ','); ) {
        answer.push_back(seg);
    }

    std::cout << "������֪ʶ��: ";
    std::getline(std::cin, tag);

    std::cout << "�������Ѷ�ϵ����1-5��: ";
    std::getline(std::cin, difficultyStr);
    difficulty = std::stoi(difficultyStr);

    return Question(id, content, options, answer, tag, difficulty);
}

std::string Question::getId() const { return id; }
void Question::setId(const std::string& newId) { id = newId; }

std::string Question::getContent() const { return content; }
void Question::setContent(const std::string& newContent) { content = newContent; }

std::vector<std::string> Question::getAnswer() const { return answer; }
void Question::setAnswer(const std::vector<std::string>& newAnswer) { answer = newAnswer; }

std::vector<std::string> Question::getOptions() const { return options; }  // ��������ʵ��
void Question::setOptions(const std::vector<std::string>& newOptions) { options = newOptions; }  // ��������ʵ��

std::string Question::getTag() const { return tag; }
int Question::getDifficulty() const { return difficulty; }

bool Question::checkAnswer(const std::vector<std::string>& userAnswer) const {
    if (userAnswer.size() != answer.size()) return false;
    for (const auto& ans : userAnswer) {
        if (std::find(answer.begin(), answer.end(), ans) == answer.end()) {
            return false;
        }
    }
    return true;
}
//����Modifier
void Question::setTag(const std::string& newTag) { tag = newTag; }
void Question::setDifficulty(int newDifficulty) {
    if (newDifficulty >= 1 && newDifficulty <= 5) {
        difficulty = newDifficulty;
    }
}