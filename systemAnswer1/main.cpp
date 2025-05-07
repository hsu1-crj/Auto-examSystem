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

// ����������ȥ���ַ���ǰ��Ŀհ��ַ�
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// ��ʦ�˹���
void addQuestion() {
    Question newQuestion = Question::createFromInput();
    newQuestion.saveToFile("questions.txt");
    std::cout << "��Ŀ�ѳɹ����浽 questions.txt �ļ���" << std::endl;
}

void modifierQuestion() {
    QuestionModifier modifier("questions.txt");
    std::cout << "=== ��Ŀ�޸�ϵͳ ===" << std::endl;
    modifier.modifyQuestion();
}

void generateExam() {
    std::vector<Question> allQuestions = Question::loadFromFile("questions.txt");
    ExamGenerator generator;
    std::string tag;
    int minDiff, maxDiff, count;

    std::cout << "������֪ʶ��: ";
    std::cin >> tag;
    std::cout << "��������С�Ѷ�: ";
    std::cin >> minDiff;
    std::cout << "����������Ѷ�: ";
    std::cin >> maxDiff;
    std::cout << "��������Ŀ����: ";
    std::cin >> count;

    auto exam = generator.generateExam(allQuestions, tag, minDiff, maxDiff, count);

    // ���ԣ��鿴ɸѡ����Ŀ����
    std::cout << "ɸѡ������Ŀ����: " << exam.size() << std::endl;

    // ��� exam.size() == 0��˵��û�з�����������
    generator.exportExam(exam, "exam_001.txt");
    std::cout << "�Ծ������ɵ� exam_001.txt �ļ���" << std::endl;
}

void modifierExam() {
    ExamModifier modifier("questions.txt");
    std::cout << "=== �Ծ��޸�ϵͳ ===" << std::endl;
    modifier.replaceQuestionInExam();
}

// ѧ���˹���
void takeExam() {
    // ����Ƿ��ѵ�¼
    if (g_currentStudentId.empty()) {
        std::cout << "���ȵ�¼�ٲ���\n";
        if (!loginStudent()) {
            return;
        }
    }

    // �������
    std::vector<Question> allQuestions = Question::loadFromFile("questions.txt");
    if (allQuestions.empty()) {
        std::cerr << "���棺δ�ҵ�������ݣ��޷����в���" << std::endl;
        return;
    }

    // �����Ծ�
    std::ifstream examFile("exam_001.txt");
    if (!examFile.is_open()) {
        std::cerr << "�޷����Ծ��ļ�: exam_001.txt" << std::endl;
        return;
    }

    std::string line;
    std::vector<Question> examQuestions;

    // ���Ծ��ļ��ж�ȡ��ĿID��Ȼ���������ҵ���Ӧ��Ŀ
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
        std::cerr << "�Ծ���û����Ч��Ŀ" << std::endl;
        return;
    }

    // ���������¼�ļ�
    std::ofstream answerLog("answer_log.txt", std::ios::app);

    std::cout << "��ش��������⣺" << std::endl;
    std::vector<std::string> userAnswers;

    for (size_t i = 0; i < examQuestions.size(); i++) {
        const auto& q = examQuestions[i];
        std::cout << "\n��Ŀ " << (i + 1) << ": " << q.getContent() << std::endl;
        std::cout << "ѡ�";
        for (const auto& opt : q.getOptions()) {
            std::cout << opt << " ";
        }
        std::cout << "\n�������: ";

        std::string userAnswer;
        std::getline(std::cin, userAnswer);
        // ȥ���û�������ܰ�����ǰ��հ��ַ�
        userAnswer = trim(userAnswer);
        userAnswers.push_back(userAnswer);
    }

    // ʹ��ȫ�ֱ����е�ѧ��ID����Student����
    Student stu(g_currentStudentId, g_currentStudentPassword);
    int score = 0;
    int questionValue = examQuestions.empty() ? 0 : 100 / examQuestions.size();

    // ���ֲ���¼�������
    for (size_t i = 0; i < examQuestions.size(); ++i) {
        if (i < userAnswers.size()) {
            // ��ȡ��ȷ�𰸲���ӡ������Ϣ
            std::vector<std::string> correctAns = examQuestions[i].getAnswer();

            std::cout << "\n��Ŀ " << (i + 1) << ":" << std::endl;
            std::cout << "- �û���: '" << userAnswers[i] << "'" << std::endl;
            std::cout << "- ��ȷ��: '";
            for (const auto& ans : correctAns) {
                std::cout << ans << " ";
            }
            std::cout << "'" << std::endl;

            // ��ȷ�ַ����Ƚ��û��𰸺���ȷ��
            bool matched = false;
            for (const auto& ans : correctAns) {
                // ȥ�����ܵĿհ��ַ�
                std::string trimmedAns = trim(ans);

                std::cout << "  �Ƚ�: '" << userAnswers[i] << "' vs '" << trimmedAns << "'" << std::endl;

                if (userAnswers[i] == trimmedAns) {
                    matched = true;
                    break;
                }
            }

            // ��¼���������ѧ��ID,��ĿID,֪ʶ��,�Ƿ���ȷ
            if (answerLog.is_open()) {
                answerLog << g_currentStudentId << ","
                    << examQuestions[i].getId() << ","
                    << examQuestions[i].getTag() << ","
                    << (matched ? "1" : "0") << "\n";
            }

            if (matched) {
                score += questionValue;
                std::cout << "  �ش���ȷ!" << std::endl;
            }
            else {
                std::cout << "  �ش����." << std::endl;
            }
        }
    }

    // ��¼�ɼ�
    stu.recordScore(score, "scores.csv");
    std::cout << "��ĵ÷�: " << score << std::endl;
    answerLog.close();

    // ����ѧϰ����
    ReportGenerator::generateWeeklyReport(g_currentStudentId);
}

int main() {
    SetConsoleOutputCP(936);  // 936��GBK����
    SetConsoleCP(936);
    std::cout << "����������ʾ" << std::endl;
    int role;
    std::cout << "��ӭʹ�ÿ���ϵͳ��\n";
    std::cout << "��ѡ���ɫ��\n1. ��ʦ\n2. ѧ��\n������ѡ��";
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
                std::cout << "��ʦѡ��\n1. �����Ŀ\n2. �޸���Ŀ\n3. ���ɾ�\n4. �滻������Ŀ\n5. �˳�\n������ѡ��";
                std::cin >> choice;
                std::cin.ignore(); // ������з�
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
                    std::cout << "��Чѡ�����������롣" << std::endl;
                }

            }
        }
    }
    else if (role == 2) {
        if (loginStudent()) {
            int choice = 0;
            do {
                std::cout << "\n======= ѧ��ϵͳ�˵� =======\n";
                std::cout << "��ǰѧ��: " << g_currentStudentId << "\n";
                std::cout << "1. ��ʼ����\n";
                std::cout << "2. �鿴��ʷ�ɼ�\n";
                std::cout << "3. ����ѧϰ����\n";
                std::cout << "0. �˳�\n";
                std::cout << "��ѡ��: ";

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
                    std::cout << "��鿴��Ӧѧϰ����\n";
                    break;
                case 3:
                    // ����ѧϰ����
                    if (!g_currentStudentId.empty()) {
                        ReportGenerator::isManualGeneration = true;  // ����Ϊ�ֶ�����
                        ReportGenerator::generateWeeklyReport(g_currentStudentId);
                        ReportGenerator::isManualGeneration = false; // ���ñ��
                    }
                    else {
                        std::cout << "���ȵ�¼�����ɱ���\n";
                    }
                    break;
                case 0:
                    std::cout << "ллʹ�ã��ټ���\n";
                    break;
                default:
                    std::cout << "��Чѡ�������ԡ�\n";
                }
            } while (choice != 0);
        }
    }
    else {
        std::cout << "��Чѡ��" << std::endl;
    }

    return 0;
}
