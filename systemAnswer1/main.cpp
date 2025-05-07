#include <iostream>
#include "Question.h"
#include "ExamGenerator.h"
#include "Student.h"
#include <fstream>
#include "QuestionModifier.h"
#include "ExamModifier.h"
#include "ReportGenerator.h"
#include "StudentLogin.h"
#include <windows.h>
#include <algorithm>
#include <cctype>
#include "TeacherLogin.h"

using namespace std;

// 辅助函数：去除字符串前后的空白字符
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// 教师端功能
void addQuestion() {
    Question newQuestion = Question::createFromInput();
    newQuestion.saveToFile("questions.txt");
    std::cout << "题目已成功保存到 questions.txt 文件。" << std::endl;
}

void modifierQuestion() {
    QuestionModifier modifier("questions.txt");
    std::cout << "=== 题目修改系统 ===" << std::endl;
    modifier.modifyQuestion();
}

void generateExam() {
    std::vector<Question> allQuestions = Question::loadFromFile("questions.txt");
    ExamGenerator generator;
    std::string tag;
    int minDiff, maxDiff, count;

    std::cout << "请输入知识点: ";
    std::cin >> tag;
    std::cout << "请输入最小难度: ";
    std::cin >> minDiff;
    std::cout << "请输入最大难度: ";
    std::cin >> maxDiff;
    std::cout << "请输入题目数量: ";
    std::cin >> count;

    auto exam = generator.generateExam(allQuestions, tag, minDiff, maxDiff, count);

    // 测试：查看筛选的题目数量
    std::cout << "筛选出的题目数量: " << exam.size() << std::endl;

    // 如果 exam.size() == 0，说明没有符合条件的题
    generator.exportExam(exam, "exam_001.txt");
    std::cout << "试卷已生成到 exam_001.txt 文件。" << std::endl;
}

void modifierExam() {
    ExamModifier modifier("questions.txt");
    std::cout << "=== 试卷修改系统 ===" << std::endl;
    modifier.replaceQuestionInExam();
}

// 学生端功能
void takeExam() {
    // 检查是否已登录
    if (g_currentStudentId.empty()) {
        std::cout << "请先登录再测试\n";
        if (!loginStudent()) {
            return;
        }
    }

    // 加载题库
    std::vector<Question> allQuestions = Question::loadFromFile("questions.txt");
    if (allQuestions.empty()) {
        std::cerr << "警告：未找到题库数据，无法进行测试" << std::endl;
        return;
    }

    // 加载试卷
    std::ifstream examFile("exam_001.txt");
    if (!examFile.is_open()) {
        std::cerr << "无法打开试卷文件: exam_001.txt" << std::endl;
        return;
    }

    std::string line;
    std::vector<Question> examQuestions;

    // 从试卷文件中读取题目ID，然后从题库中找到对应题目
    while (std::getline(examFile, line)) {
        if (line.find('[') == 0) {
            std::string id = line.substr(1, line.find(']') - 1);
            for (const auto& q : allQuestions) {
                if (q.getId() == id) {
                    examQuestions.push_back(q);
                    break;
                }
            }
        }
    }
    examFile.close();

    if (examQuestions.empty()) {
        std::cerr << "试卷中没有有效题目" << std::endl;
        return;
    }

    // 创建答题记录文件
    std::ofstream answerLog("answer_log.txt", std::ios::app);

    std::cout << "请回答以下问题：" << std::endl;
    std::vector<std::string> userAnswers;

    for (size_t i = 0; i < examQuestions.size(); i++) {
        const auto& q = examQuestions[i];
        std::cout << "\n题目 " << (i + 1) << ": " << q.getContent() << std::endl;
        std::cout << "选项：";
        for (const auto& opt : q.getOptions()) {
            std::cout << opt << " ";
        }
        std::cout << "\n请输入答案: ";

        std::string userAnswer;
        std::getline(std::cin, userAnswer);
        // 去除用户输入可能包含的前后空白字符
        userAnswer = trim(userAnswer);
        userAnswers.push_back(userAnswer);
    }

    // 使用全局变量中的学生ID创建Student对象
    Student stu(g_currentStudentId, g_currentStudentPassword);
    int score = 0;
    int questionValue = examQuestions.empty() ? 0 : 100 / examQuestions.size();

    // 评分并记录答题情况
    for (size_t i = 0; i < examQuestions.size(); ++i) {
        if (i < userAnswers.size()) {
            // 获取正确答案并打印调试信息
            std::vector<std::string> correctAns = examQuestions[i].getAnswer();

            std::cout << "\n题目 " << (i + 1) << ":" << std::endl;
            std::cout << "- 用户答案: '" << userAnswers[i] << "'" << std::endl;
            std::cout << "- 正确答案: '";
            for (const auto& ans : correctAns) {
                std::cout << ans << " ";
            }
            std::cout << "'" << std::endl;

            // 精确字符串比较用户答案和正确答案
            bool matched = false;
            for (const auto& ans : correctAns) {
                // 去除可能的空白字符
                std::string trimmedAns = trim(ans);

                std::cout << "  比较: '" << userAnswers[i] << "' vs '" << trimmedAns << "'" << std::endl;

                if (userAnswers[i] == trimmedAns) {
                    matched = true;
                    break;
                }
            }

            // 记录答题情况：学生ID,题目ID,知识点,是否正确
            if (answerLog.is_open()) {
                answerLog << g_currentStudentId << ","
                    << examQuestions[i].getId() << ","
                    << examQuestions[i].getTag() << ","
                    << (matched ? "1" : "0") << "\n";
            }

            if (matched) {
                score += questionValue;
                std::cout << "  回答正确!" << std::endl;
            }
            else {
                std::cout << "  回答错误." << std::endl;
            }
        }
    }

    // 记录成绩
    stu.recordScore(score, "scores.csv");
    std::cout << "你的得分: " << score << std::endl;
    answerLog.close();

    // 生成学习报告
    ReportGenerator::generateWeeklyReport(g_currentStudentId);
}

int main() {
    SetConsoleOutputCP(936);  // 936是GBK编码
    SetConsoleCP(936);
    std::cout << "中文正常显示" << std::endl;
    int role;
    std::cout << "欢迎使用考试系统！\n";
    std::cout << "请选择角色：\n1. 教师\n2. 学生\n请输入选择：";
    std::string roleInput;
    std::getline(std::cin, roleInput);

    try {
        role = std::stoi(roleInput);
    }
    catch (...) {
        role = -1;
    }

    if (role == 1) {

        if (loginTeacher()) {

            int choice;
            while (true) {
                std::cout << "教师选项\n1. 添加题目\n2. 修改题目\n3. 生成卷\n4. 替换卷中题目\n5. 退出\n请输入选择：";
                std::cin >> choice;
                std::cin.ignore(); // 清除换行符
                if (choice == 1) {
                    addQuestion();
                }
                else if (choice == 2) {
                    modifierQuestion();
                }
                else if (choice == 3) {
                    generateExam();
                }
                else if (choice == 4) {
                    modifierExam();
                }
                else if (choice == 5) {
                    break;
                }
                else {
                    std::cout << "无效选择，请重新输入。" << std::endl;
                }

            }
        }
    }
    else if (role == 2) {
        if (loginStudent()) {
            int choice = 0;
            do {
                std::cout << "\n======= 学生系统菜单 =======\n";
                std::cout << "当前学生: " << g_currentStudentId << "\n";
                std::cout << "1. 开始测试\n";
                std::cout << "2. 查看历史成绩\n";
                std::cout << "3. 生成学习报告\n";
                std::cout << "0. 退出\n";
                std::cout << "请选择: ";

                std::string input;
                std::getline(std::cin, input);
                try {
                    choice = std::stoi(input);
                }
                catch (...) {
                    choice = -1;
                }

                switch (choice) {
                case 1:
                    takeExam();
                    break;
                case 2:
                    std::cout << "请查看相应学习报告\n";
                    break;
                case 3:
                    // 生成学习报告
                    if (!g_currentStudentId.empty()) {
                        ReportGenerator::isManualGeneration = true;  // 设置为手动生成
                        ReportGenerator::generateWeeklyReport(g_currentStudentId);
                        ReportGenerator::isManualGeneration = false; // 重置标记
                    }
                    else {
                        std::cout << "请先登录再生成报告\n";
                    }
                    break;
                case 0:
                    std::cout << "谢谢使用，再见！\n";
                    break;
                default:
                    std::cout << "无效选择，请重试。\n";
                }
            } while (choice != 0);
        }
    }
    else {
        std::cout << "无效选择" << std::endl;
    }

    return 0;
}
