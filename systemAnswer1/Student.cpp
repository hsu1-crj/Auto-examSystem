#include "Student.h"
#include <fstream>
#include <ctime>
#include <sstream>

Student::Student(const std::string& sId, const std::string& sPw)
    : id(sId), password(sPw) {
}

int Student::submitAnswer(const std::vector<std::string>& userAnswer,
    const std::vector<std::string>& correctAnswer) {
    int score = 0;

    // Check if the arrays have the same size
    if (userAnswer.size() == correctAnswer.size()) {
        bool allMatch = true;

        // Compare each answer at the same position
        for (size_t i = 0; i < userAnswer.size(); i++) {
            if (userAnswer[i] != correctAnswer[i]) {
                allMatch = false;
                break;
            }
        }

        if (allMatch) score = 100; // Full score for all correct answers
    }

    return score;
}
// ��recordScore�޸�Ϊconst����
void Student::recordScore(int score, const std::string& filename) const {
    std::ofstream ofs;
    bool fileExists = std::ifstream(filename).good();

    // ����ļ������ڣ�����ӱ�����
    if (!fileExists) {
        ofs.open(filename);
        if (ofs.is_open()) {
            // ����б��⣬ʹ Excel �ܸ��õ�ʶ����
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

    // д���¼��ʱ���ַ���������
    ofs << id << "," << score << "," << timeBuf << "\n";
    ofs.close();
}

// ��ʾ����д��һ���ı��ļ���������ȷ�ʡ�����֪ʶ�㡢�Ƽ���ϰID
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