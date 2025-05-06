#include <iostream>
#include "Question.h"
#include "ExamGenerator.h"
#include "Student.h"
#include <fstream>
#include "QuestionModifier.h"
#include "ExamModifier.h"
#include "ReportGenerator.h"
#include "StudentLogin.h"

using namespace std;
// 教师端功能
void addQuestion() {
    Question newQuestion = Question::createFromInput();
    newQuestion.saveToFile("questions.txt");
    std::cout << "题目已成功保存到 questions.txt 文件。" << std::endl;
}
void modifierQuestion()
{
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

    // 调试：查看返回的题目数量
    std::cout << "筛选到的题目数量: " << exam.size() << std::endl;

    // 如果 exam.size() == 0，说明没有符合条件的题
    generator.exportExam(exam, "exam_001.txt");
    std::cout << "试卷已生成到 exam_001.txt 文件。" << std::endl;
}
void modifierExam()
{
	ExamModifier modifier("questions.txt");
	std::cout << "=== 试卷修改系统 ===" << std::endl;
	modifier.replaceQuestionInExam();
}
// 学生端功能
void takeExam() {
    // 检查是否已登录
    if (g_currentStudentId.empty()) {
        std::cout << "请先登录再答题\n";
        if (!loginStudent()) {
            return;
        }
    }
    
    std::ifstream examFile("exam_001.txt");
    if (!examFile.is_open()) {
        std::cerr << "无法打开试卷文件: exam_001.txt" << std::endl;
        return;
    }

    std::string line;
    std::vector<Question> examQuestions;
    std::vector<std::string> userAnswers;
    while (std::getline(examFile, line)) {
        if (line.find('[') == 0) {
            std::string id = line.substr(1, line.size() - 2);
            for (const auto& q : Question::loadFromFile("questions.txt")) {
                if (q.getId() == id) {
                    examQuestions.push_back(q);
                    break;
                }
            }
        }
    }
    //答题记录文件
    std::ofstream answerLog("answer_log.txt", std::ios::app);

    std::cout << "请回答以下问题：" << std::endl;
    for (const auto& q : examQuestions) {
        std::cout << q.getContent() << std::endl;
        std::cout << "选项：";
        for (const auto& opt : q.getOptions()) {
            std::cout << opt << " ";
        }
        std::cout << std::endl;

        std::string userAnswer;
        std::getline(std::cin, userAnswer);
        // 去除用户输入可能包含的前后空白字符
        userAnswer.erase(0, userAnswer.find_first_not_of(" \n\r\t"));
        userAnswer.erase(userAnswer.find_last_not_of(" \n\r\t") + 1);
        userAnswers.push_back(userAnswer);
    }

    // 使用全局变量中的学生ID创建Student对象
    Student stu(g_currentStudentId, g_currentStudentPassword);
    int score = 0;
    int questionValue = examQuestions.empty() ? 0 : 100 / examQuestions.size();

    // 修改此处：使用精确的字符串比较并添加调试信息
    for (size_t i = 0; i < examQuestions.size(); ++i) {
        if (i < userAnswers.size()) {
            // 获取正确答案并打印调试信息
            std::vector<std::string> correctAns = examQuestions[i].getAnswer();

            std::cout << "题目 " << (i + 1) << ":" << std::endl;
            std::cout << "- 用户答案: '" << userAnswers[i] << "'" << std::endl;
            std::cout << "- 正确答案: '";
            for (const auto& ans : correctAns) {
                std::cout << ans << " ";
            }
            std::cout << "'" << std::endl;

            // 逐字符比较用户答案与正确答案
            bool matched = false;
            for (const auto& ans : correctAns) {
                // 去除可能的空白字符
                std::string trimmedAns = ans;
                trimmedAns.erase(0, trimmedAns.find_first_not_of(" \n\r\t"));
                trimmedAns.erase(trimmedAns.find_last_not_of(" \n\r\t") + 1);

                std::cout << "  比较: '" << userAnswers[i] << "' vs '" << trimmedAns << "'" << std::endl;

                if (userAnswers[i] == trimmedAns) {
                    matched = true;
                    break;
                }
            }
            // 记录答题结果（学号,题目ID,知识点,是否正确）
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

    stu.recordScore(score, "scores.csv");
    std::cout << "你的得分: " << score << std::endl;
    // ... 记录成绩 ...
    answerLog.close();

    // 每周一自动生成报告（可选）
    ReportGenerator::generateWeeklyReport(g_currentStudentId);
}
////检查储存答案的格式 一次检验后应该是正常的
//void checkQuestionDatabase() {
//    std::vector<Question> questions = Question::loadFromFile("questions.txt");
//
//    std::cout << "题库中共有 " << questions.size() << " 个问题。" << std::endl;
//
//    for (size_t i = 0; i < questions.size(); ++i) {
//        std::cout << "题目 " << (i + 1) << " (ID: " << questions[i].getId() << ")" << std::endl;
//        std::cout << "内容: " << questions[i].getContent() << std::endl;
//
//        std::cout << "选项: ";
//        for (const auto& opt : questions[i].getOptions()) {
//            std::cout << "'" << opt << "' ";
//        }
//        std::cout << std::endl;
//
//        std::cout << "答案: ";
//        for (const auto& ans : questions[i].getAnswer()) {
//            std::cout << "'" << ans << "' ";
//        }
//        std::cout << std::endl << std::endl;
//    }
//}
int main() {
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
    std::cin.ignore(); // 忽略换行符

    if (role == 1) {
        int choice;
        while (true) {
            std::cout << "教师端选项：\n1. 添加题目\n2. 修改题目\n3. 随机组卷\n4. 替换组卷题目\n5. 退出\n请输入选择：";
            std::cin >> choice;
            std::cin.ignore(); // 忽略换行符
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
    else if (role == 2) {
        if (loginStudent()) {
            int choice = 0;
            do {
                std::cout << "\n======= 学生系统菜单 =======\n";
                std::cout << "当前学号: " << g_currentStudentId << "\n";
                std::cout << "1. 开始答题\n";
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
                    /*checkQuestionDatabase();*/
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
        std::cout << "无效选择。" << std::endl;
    }

    return 0;
}