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
// ��ʦ�˹���
void addQuestion() {
    Question newQuestion = Question::createFromInput();
    newQuestion.saveToFile("questions.txt");
    std::cout << "��Ŀ�ѳɹ����浽 questions.txt �ļ���" << std::endl;
}
void modifierQuestion()
{
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

    // ���ԣ��鿴���ص���Ŀ����
    std::cout << "ɸѡ������Ŀ����: " << exam.size() << std::endl;

    // ��� exam.size() == 0��˵��û�з�����������
    generator.exportExam(exam, "exam_001.txt");
    std::cout << "�Ծ������ɵ� exam_001.txt �ļ���" << std::endl;
}
void modifierExam()
{
	ExamModifier modifier("questions.txt");
	std::cout << "=== �Ծ��޸�ϵͳ ===" << std::endl;
	modifier.replaceQuestionInExam();
}
// ѧ���˹���
void takeExam() {
    // ����Ƿ��ѵ�¼
    if (g_currentStudentId.empty()) {
        std::cout << "���ȵ�¼�ٴ���\n";
        if (!loginStudent()) {
            return;
        }
    }
    
    std::ifstream examFile("exam_001.txt");
    if (!examFile.is_open()) {
        std::cerr << "�޷����Ծ��ļ�: exam_001.txt" << std::endl;
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
    //�����¼�ļ�
    std::ofstream answerLog("answer_log.txt", std::ios::app);

    std::cout << "��ش��������⣺" << std::endl;
    for (const auto& q : examQuestions) {
        std::cout << q.getContent() << std::endl;
        std::cout << "ѡ�";
        for (const auto& opt : q.getOptions()) {
            std::cout << opt << " ";
        }
        std::cout << std::endl;

        std::string userAnswer;
        std::getline(std::cin, userAnswer);
        // ȥ���û�������ܰ�����ǰ��հ��ַ�
        userAnswer.erase(0, userAnswer.find_first_not_of(" \n\r\t"));
        userAnswer.erase(userAnswer.find_last_not_of(" \n\r\t") + 1);
        userAnswers.push_back(userAnswer);
    }

    // ʹ��ȫ�ֱ����е�ѧ��ID����Student����
    Student stu(g_currentStudentId, g_currentStudentPassword);
    int score = 0;
    int questionValue = examQuestions.empty() ? 0 : 100 / examQuestions.size();

    // �޸Ĵ˴���ʹ�þ�ȷ���ַ����Ƚϲ���ӵ�����Ϣ
    for (size_t i = 0; i < examQuestions.size(); ++i) {
        if (i < userAnswers.size()) {
            // ��ȡ��ȷ�𰸲���ӡ������Ϣ
            std::vector<std::string> correctAns = examQuestions[i].getAnswer();

            std::cout << "��Ŀ " << (i + 1) << ":" << std::endl;
            std::cout << "- �û���: '" << userAnswers[i] << "'" << std::endl;
            std::cout << "- ��ȷ��: '";
            for (const auto& ans : correctAns) {
                std::cout << ans << " ";
            }
            std::cout << "'" << std::endl;

            // ���ַ��Ƚ��û�������ȷ��
            bool matched = false;
            for (const auto& ans : correctAns) {
                // ȥ�����ܵĿհ��ַ�
                std::string trimmedAns = ans;
                trimmedAns.erase(0, trimmedAns.find_first_not_of(" \n\r\t"));
                trimmedAns.erase(trimmedAns.find_last_not_of(" \n\r\t") + 1);

                std::cout << "  �Ƚ�: '" << userAnswers[i] << "' vs '" << trimmedAns << "'" << std::endl;

                if (userAnswers[i] == trimmedAns) {
                    matched = true;
                    break;
                }
            }
            // ��¼��������ѧ��,��ĿID,֪ʶ��,�Ƿ���ȷ��
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

    stu.recordScore(score, "scores.csv");
    std::cout << "��ĵ÷�: " << score << std::endl;
    // ... ��¼�ɼ� ...
    answerLog.close();

    // ÿ��һ�Զ����ɱ��棨��ѡ��
    ReportGenerator::generateWeeklyReport(g_currentStudentId);
}
////��鴢��𰸵ĸ�ʽ һ�μ����Ӧ����������
//void checkQuestionDatabase() {
//    std::vector<Question> questions = Question::loadFromFile("questions.txt");
//
//    std::cout << "����й��� " << questions.size() << " �����⡣" << std::endl;
//
//    for (size_t i = 0; i < questions.size(); ++i) {
//        std::cout << "��Ŀ " << (i + 1) << " (ID: " << questions[i].getId() << ")" << std::endl;
//        std::cout << "����: " << questions[i].getContent() << std::endl;
//
//        std::cout << "ѡ��: ";
//        for (const auto& opt : questions[i].getOptions()) {
//            std::cout << "'" << opt << "' ";
//        }
//        std::cout << std::endl;
//
//        std::cout << "��: ";
//        for (const auto& ans : questions[i].getAnswer()) {
//            std::cout << "'" << ans << "' ";
//        }
//        std::cout << std::endl << std::endl;
//    }
//}
int main() {
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
    std::cin.ignore(); // ���Ի��з�

    if (role == 1) {
        int choice;
        while (true) {
            std::cout << "��ʦ��ѡ�\n1. �����Ŀ\n2. �޸���Ŀ\n3. ������\n4. �滻�����Ŀ\n5. �˳�\n������ѡ��";
            std::cin >> choice;
            std::cin.ignore(); // ���Ի��з�
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
                    /*checkQuestionDatabase();*/
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