#include "Student.h"
#include <fstream>
#include <ctime>
#include <sstream>
#include <iostream>
#include <algorithm>

Student::Student(const std::string& sId, const std::string& sPw)
    : id(sId), password(sPw) {
}

int Student::submitAnswer(const std::vector<std::string>& userAnswer,
    const std::vector<std::string>& correctAnswer) {
    int score = 0;
    int totalQuestions = userAnswer.size();
    int correctCount = 0;

    // ���ÿ����
    for (size_t i = 0; i < userAnswer.size() && i < correctAnswer.size(); i++) {
        // ȥ��ǰ��հ��ַ�
        std::string userAns = trimString(userAnswer[i]);
        std::string correctAns = trimString(correctAnswer[i]);

        if (userAns == correctAns) {
            correctCount++;
        }
    }

    // ����÷�
    if (totalQuestions > 0) {
        score = (correctCount * 100) / totalQuestions;
    }

    return score;
}

// ��recordScore�޸�Ϊconst����
void Student::recordScore(int score, const std::string& filename) const {
    std::ofstream ofs;
    bool fileExists = std::ifstream(filename).good();

    // ����ļ������ڣ���ӱ�ͷ��
    if (!fileExists) {
        ofs.open(filename);
        if (ofs.is_open()) {
            // ����б��⣬ʹ Excel �ܸ��õ�ʶ������
            ofs << "ѧ��ID,����,����ʱ��\n";
            ofs.close();
        }
    }

    // ��׷��ģʽ���ļ�
    ofs.open(filename, std::ios::app);
    if (!ofs.is_open()) return;

    // ��ȡ��ǰʱ��
    time_t now = time(nullptr);
    struct tm t;
    localtime_s(&t, &now);
    char timeBuf[64];

    // ��ʽ��ʱ�䣬ʹ�����Ű���ʱ���ַ���
    strftime(timeBuf, sizeof(timeBuf), "\"%Y-%m-%d %H:%M\"", &t);

    // д���¼��ʱ���ַ��������ţ�
    ofs << id << "," << score << "," << timeBuf << "\n";
    ofs.close();
}

// ������Ҫд��һ���ı��ļ���������ȷ�ʡ�����֪ʶ�㡢�Ƽ���ϰID
void Student::generateReport(const std::string& filename, float correctRate,
    const std::string& weakPoints,
    const std::vector<std::string>& recommendIds)
{
    std::ofstream ofs(filename);
    if (!ofs.is_open()) return;
    ofs << "��ȷ�ʣ�" << correctRate * 100 << "%\n"
        << "����֪ʶ�㣺" << weakPoints << "\n"
        << "�Ƽ���ϰ��";
    for (const auto& id : recommendIds) {
        ofs << id << " ";
    }
    ofs << "\n";
    ofs.close();
}

// �Ӵ�������¼�л�ȡѧ���ı���֪ʶ��
std::map<std::string, int> Student::getWeakPoints() const {
    std::map<std::string, int> weakPoints;
    std::ifstream logFile("answer_log.txt");

    if (!logFile.is_open()) {
        return weakPoints;
    }

    std::string line;
    while (std::getline(logFile, line)) {
        std::istringstream ss(line);
        std::vector<std::string> fields;
        std::string field;

        while (std::getline(ss, field, ',')) {
            fields.push_back(field);
        }

        // ȷ����¼��ʽ��ȷ���ǵ�ǰѧ���ļ�¼
        if (fields.size() >= 4 && fields[0] == id) {
            std::string knowledgePoint = fields[2];
            bool correct = (fields[3] == "1");

            if (!correct) {
                // �ۼ�֪ʶ�������
                weakPoints[knowledgePoint]++;
            }
        }
    }

    return weakPoints;
}

// ����������ȥ���ַ���ǰ��հ��ַ�
std::string Student::trimString(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}
