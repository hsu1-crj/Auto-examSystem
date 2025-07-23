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
            << "题干：" << content << "\n"
            << "选项：";
        for (size_t i = 0; i < options.size(); ++i) {
            ofs << options[i];
            if (i < options.size() - 1) ofs << " ";
        }
        ofs << "\n答案：";
        for (size_t i = 0; i < answer.size(); ++i) {
            ofs << answer[i];
            if (i < answer.size() - 1) ofs << ",";
        }
        ofs << "\n知识点：" << tag << "\n"
            << "难度：" << difficulty << "\n\n";
    }
    ofs.close();
}

std::vector<Question> Question::loadFromFile(const std::string& filename) {
    std::vector<Question> questions;
    std::ifstream ifs(filename);//输出文件流
    if (!ifs.is_open()) {
        std::cerr << "无法打开题库文件: " << filename << std::endl;
        return questions;
    }

    std::string line, currentId, content, tag;
    std::vector<std::string> options, ans;
    int difficulty = 1;
    while (std::getline(ifs, line)) {
        if (line.find('[') == 0) {
            // 修改ID提取方式，使用更可靠的方法
            size_t start = line.find('[') + 1;
            size_t end = line.find(']');
            if (end != std::string::npos && start < end) {//是否能查询到]
                currentId = line.substr(start, end - start);
            }
        }
        else if (line.rfind("题干：", 0) == 0) {
            content = line.substr(6);
        }
        else if (line.rfind("选项：", 0) == 0) {//找到选项：就正常进行
            options.clear();
            std::istringstream iss(line.substr(6));
            std::string opt;
            while (iss >> opt) {
                options.push_back(opt);
            }
        }
        else if (line.rfind("答案：", 0) == 0) {
            ans.clear();
            std::string answers = line.substr(6);
            std::stringstream sstream(answers);
            std::string seg;
            while (std::getline(sstream, seg, ',')) {
                ans.push_back(seg);
            }
        }
        else if (line.rfind("知识点：", 0) == 0) {
            tag = line.substr(8);
        }
        else if (line.rfind("难度：", 0) == 0) {
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

    std::cout << "请输入题目 ID: ";
    std::getline(std::cin, id);

    std::cout << "请输入题干: ";
    std::getline(std::cin, content);

    std::cout << "请输入选项（空格分隔）: ";
    std::string opts;
    std::getline(std::cin, opts);
    std::istringstream iss(opts);//按空格提取每个单词
    for (std::string opt; iss >> opt; ) {
        options.push_back(opt);//容器末尾添加元素
    }

    std::cout << "请输入答案（逗号分隔）: ";
    std::string ans;
    std::getline(std::cin, ans);
    std::stringstream sstream(ans);
    for (std::string seg; std::getline(sstream, seg, ','); ) {
        answer.push_back(seg);
    }

    std::cout << "请输入知识点: ";
    std::getline(std::cin, tag);

    std::cout << "请输入难度系数（1-5）: ";
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

std::vector<std::string> Question::getOptions() const { return options; }  // 新增方法实现
void Question::setOptions(const std::vector<std::string>& newOptions) { options = newOptions; }  // 新增方法实现

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
//关于Modifier
void Question::setTag(const std::string& newTag) { tag = newTag; }
void Question::setDifficulty(int newDifficulty) {
    if (newDifficulty >= 1 && newDifficulty <= 5) {
        difficulty = newDifficulty;
    }
}