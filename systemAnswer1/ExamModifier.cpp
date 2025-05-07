#include "ExamModifier.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <codecvt>
#include <locale>
#include <string>
#include <algorithm>

ExamModifier::ExamModifier(const std::string& questionBank) : questionBankFile(questionBank) {
    // 加载所有题目
    allQuestions = Question::loadFromFile(questionBankFile);
}

bool ExamModifier::replaceQuestionInExam() {
    // 获取试卷名称
    std::string examFilename;
    std::cout << "请输入要修改的试卷文件名 (如 exam_001.txt): ";
    std::getline(std::cin, examFilename);

    // 读取试卷 - 使用自定义方法，不使用 Question::loadFromFile
    std::vector<Question> examQuestions = readExamFromFile(examFilename);
    if (examQuestions.empty()) {
        std::cout << "无法读取试卷文件或文件为空! 请检查文件名是否正确。" << std::endl;
        return false;
    }

    // 显示试卷中的题目
    std::cout << "当前试卷包含以下题目：" << std::endl;
    displayExamQuestions(examQuestions);

    // 获取要替换的题目编号
    std::string indexStr;
    std::cout << "请输入要替换的题目序号 (0-" << examQuestions.size() - 1 << "): ";
    std::getline(std::cin, indexStr);
    int index = std::stoi(indexStr);

    if (index < 0 || index >= examQuestions.size()) {
        std::cout << "无效的题目序号!" << std::endl;
        return false;
    }

    // 显示当前题目
    std::cout << "当前题目: " << std::endl;
    std::cout << "[" << examQuestions[index].getId() << "]" << std::endl;
    std::cout << "题干：" << examQuestions[index].getContent() << std::endl;
    std::cout << "知识点：" << examQuestions[index].getTag() << std::endl;
    std::cout << "难度：" << examQuestions[index].getDifficulty() << std::endl;

    // 获取新题目ID
    std::string newId;
    std::cout << "请输入新题目ID: ";
    std::getline(std::cin, newId);

    // 在题库中查找新题目
    int newQuestionIndex = -1;
    for (size_t i = 0; i < allQuestions.size(); ++i) {
        if (allQuestions[i].getId() == newId) {
            newQuestionIndex = static_cast<int>(i);
            break;
        }
    }

    if (newQuestionIndex == -1) {
        std::cout << "未找到ID为 " << newId << " 的题目!" << std::endl;
        return false;
    }

    // 显示新题目
    std::cout << "新题目: " << std::endl;
    std::cout << "[" << allQuestions[newQuestionIndex].getId() << "]" << std::endl;
    std::cout << "题干：" << allQuestions[newQuestionIndex].getContent() << std::endl;
    std::cout << "知识点：" << allQuestions[newQuestionIndex].getTag() << std::endl;
    std::cout << "难度：" << allQuestions[newQuestionIndex].getDifficulty() << std::endl;

    // 确认替换
    std::string confirm;
    std::cout << "确认替换题目? (y/n): ";
    std::getline(std::cin, confirm);

    if (confirm != "y" && confirm != "Y") {
        std::cout << "已取消替换!" << std::endl;
        return false;
    }

    // 替换题目
    examQuestions[index] = allQuestions[newQuestionIndex];

    // 保存修改后的试卷
    std::string newExamFilename;
    std::cout << "请输入新的试卷文件名 (留空则覆盖原文件): ";
    std::getline(std::cin, newExamFilename);

    if (newExamFilename.empty()) {
        newExamFilename = examFilename;
    }

    if (writeExamToFile(newExamFilename, examQuestions)) {
        std::cout << "试卷已成功保存：" << newExamFilename << std::endl;
        return true;
    }
    else {
        std::cout << "试卷保存失败！" << std::endl;
        return false;
    }
}

// 自定义读取函数，支持处理多种编码格式
std::vector<Question> ExamModifier::readExamFromFile(const std::string& examFilename) {
    std::vector<Question> questions;
    std::ifstream file(examFilename);  // 不使用二进制模式，因为文件已经是正确编码
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << examFilename << std::endl;
        return questions;
    }

    std::string line;
    std::string id, content, tag;
    std::vector<std::string> options, answers;
    int difficulty = 1;

    while (std::getline(file, line)) {
        // 检查空行
        if (line.empty()) {
            continue;
        }

        // 处理题目ID
        if (line.find('[') != std::string::npos && line.find(']') != std::string::npos) {
            // 如果已经解析了一个完整题目，添加到列表
            if (!id.empty()) {
                Question q(id, content, options, answers, tag, difficulty);
                questions.push_back(q);

                // 重置变量
                id = "";
                content = "";
                tag = "";
                options.clear();
                answers.clear();
                difficulty = 1;
            }

            // 提取ID
            size_t start = line.find('[') + 1;
            size_t end = line.find(']');
            if (start < end) {
                id = line.substr(start, end - start);
            }
            continue;
        }

        // 处理题干
        if (line.find("题干：") != std::string::npos) {
            content = line.substr(line.find("题干：") + 5);  // 长度为6的中文字符
            continue;
        }

        // 处理选项
        if (line.find("选项：") != std::string::npos) {
            std::string optionsLine = line.substr(line.find("选项：") + 5);
            std::istringstream iss(optionsLine);
            std::string opt;
            while (iss >> opt) {
                options.push_back(opt);
            }
            continue;
        }

        // 处理答案
        if (line.find("答案：") != std::string::npos) {
            std::string answersLine = line.substr(line.find("答案：") + 5);
            std::istringstream iss(answersLine);
            std::string ans;

            // 使用逗号分隔答案
            while (std::getline(iss, ans, ',')) {
                // 去除可能的空白
                ans.erase(0, ans.find_first_not_of(" \t"));
                ans.erase(ans.find_last_not_of(" \t") + 1);

                if (!ans.empty()) {
                    answers.push_back(ans);
                }
            }
            continue;
        }

        // 处理知识点
        if (line.find("知识点：") != std::string::npos) {
            tag = line.substr(line.find("知识点：") + 7);  // 长度为9的中文字符
            continue;
        }

        // 处理难度
        if (line.find("难度：") != std::string::npos) {
            std::string difficultyStr = line.substr(line.find("难度：") + 5);
            try {
                difficulty = std::stoi(difficultyStr);
            }
            catch (...) {
                difficulty = 1;
            }
        }
    }

    // 添加最后一个题目
    if (!id.empty()) {
        Question q(id, content, options, answers, tag, difficulty);
        questions.push_back(q);
    }

    return questions;
}
// 尝试从二进制文件中读取题目，处理不同编码


bool ExamModifier::writeExamToFile(const std::string& examFilename, const std::vector<Question>& examQuestions) {
    std::ofstream ofs(examFilename);  // 不使用二进制模式，文件会以本地编码格式保存
    if (!ofs.is_open()) {
        return false;
    }

    for (const auto& question : examQuestions) {
        ofs << "[" << question.getId() << "]" << std::endl;
        ofs << "题干：" << question.getContent() << std::endl;

        ofs << "选项：";
        std::vector<std::string> options = question.getOptions();
        for (size_t i = 0; i < options.size(); ++i) {
            ofs << options[i];
            if (i < options.size() - 1) {
                ofs << " ";
            }
        }
        ofs << std::endl;

        ofs << "答案：";
        std::vector<std::string> answers = question.getAnswer();
        for (size_t i = 0; i < answers.size(); ++i) {
            ofs << answers[i];
            if (i < answers.size() - 1) {
                ofs << ",";
            }
        }
        ofs << std::endl;

        ofs << "知识点：" << question.getTag() << std::endl;
        ofs << "难度：" << question.getDifficulty() << std::endl << std::endl;
    }

    return true;
}

// 仅修改displayExamQuestions方法，用于清理显示内容中的乱码
void ExamModifier::displayExamQuestions(const std::vector<Question>& examQuestions) {
    for (size_t i = 0; i < examQuestions.size(); ++i) {
        std::cout << "题目 " << i << ":" << std::endl;
        std::cout << "[" << examQuestions[i].getId() << "]" << std::endl;

        // 直接显示内容，不进行任何过滤
        std::cout << "题干：" << examQuestions[i].getContent() << std::endl;

        std::cout << "选项：";
        std::vector<std::string> options = examQuestions[i].getOptions();
        for (size_t j = 0; j < options.size(); ++j) {
            std::cout << options[j];
            if (j < options.size() - 1) std::cout << " ";
        }
        std::cout << std::endl;

        std::cout << "答案：";
        std::vector<std::string> answers = examQuestions[i].getAnswer();
        for (size_t j = 0; j < answers.size(); ++j) {
            std::cout << answers[j];
            if (j < answers.size() - 1) std::cout << ",";
        }
        std::cout << std::endl;

        std::cout << "知识点：" << examQuestions[i].getTag() << std::endl;
        std::cout << "难度：" << examQuestions[i].getDifficulty() << std::endl << std::endl;
    }
}

// 添加一个新的辅助方法用于清理文本中的乱码
// 如果您不想删除这个函数，可以修改它
std::string ExamModifier::cleanChineseText(const std::string& text) {
    // 直接返回原文本，不进行任何过滤
    return text;
}