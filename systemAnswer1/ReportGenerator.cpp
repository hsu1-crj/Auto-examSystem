#include "ReportGenerator.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>
#include <ctime>
#include <random>

// ��ʼ����̬����
bool ReportGenerator::isManualGeneration = false;

// �����ܱ���
void ReportGenerator::generateWeeklyReport(const std::string& studentId) {
    // ����Ƿ���һ
    if (isTimeToGenerateReport() && !isManualGeneration) {
        std::cout << "��ʼ������һѧϰ����..." << std::endl;
    }

    // ������ȷ��
    float correctRate = calculateCorrectRate(studentId);

    // ͳ�ƴ���֪ʶ��
    std::map<std::string, int> errorsByTag = countErrorsByTag(studentId);

    // ��ȡ�Ƽ���ϰ�� - ֱ�Ӵ���ѧ��ID���ú����ڲ������߼�
    std::vector<std::string> recommendExercises = getRecommendedExercises(errorsByTag, studentId);

    // ���ɱ����ļ���
    std::string filename = "report_" + studentId + ".txt";

    // д�뱨��
    std::ofstream report(filename);
    if (report.is_open()) {
        report << "��ȷ�ʣ�" << static_cast<int>(correctRate * 100) << "%" << std::endl;

        report << "����֪ʶ�㣺";
        bool firstTag = true;
        // �����������
        std::vector<std::pair<std::string, int>> sortedErrors;
        for (const auto& error : errorsByTag) {
            if (error.second > 0) {
                sortedErrors.push_back(error);
            }
        }

        // �����������������
        std::sort(sortedErrors.begin(), sortedErrors.end(),
            [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                return a.second > b.second;
            });

        // �������֪ʶ��
        if (sortedErrors.empty()) {
            report << "���ޱ���֪ʶ��";
        }
        else {
            // ���ǰ3������֪ʶ��
            for (size_t i = 0; i < sortedErrors.size() && i < 3; ++i) {
                if (!firstTag) report << "��";
                report << sortedErrors[i].first << "������" << sortedErrors[i].second << "�Σ�";
                firstTag = false;
            }
        }
        report << std::endl;

        // ����Ƽ���ϰ
        report << "�Ƽ���ϰ��";
        if (recommendExercises.empty()) {
            report << "�����Ƽ���ϰ";
        }
        else {
            for (size_t i = 0; i < recommendExercises.size(); ++i) {
                report << "��ĿID " << recommendExercises[i];
                if (i < recommendExercises.size() - 1) report << ", ";
            }
        }
        report << std::endl;

        // ��ӱ�������ʱ��
        time_t now = time(nullptr);//time_t��ȡʱ�䣬���洢��now
        struct tm t;
        localtime_s(&t, &now);//�洢����ʱ����ʱ���
        char timeBuf[64];//�洢��ʽʱ���ַ���
        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M", &t);//4y 2mon 2d 2h 2min
        report << "\n��������ʱ��: " << timeBuf << std::endl;

        report.close();
        std::cout << "������ѧϰ���棺" << filename << std::endl;

        // �����ϸ�Ƽ�����
        std::cout << "\n�Ƽ����ݣ�" << std::endl;
        if (!sortedErrors.empty()) {
            for (size_t i = 0; i < sortedErrors.size() && i < 3; ++i) {
                std::cout << "- ���֪ʶ�� [" << sortedErrors[i].first << "] (����"
                    << sortedErrors[i].second << "��) �Ƽ������Ŀ" << std::endl;
            }
        }
        else {
            std::cout << "- δ�������Ա���֪ʶ�㣬�Ƽ��ۺ���ϰ��Ŀ" << std::endl;
        }
    }
    else {
        std::cerr << "�޷����������ļ���" << std::endl;
    }
}

// ͳ��ÿ��֪ʶ��Ĵ������
std::map<std::string, int> ReportGenerator::countErrorsByTag(const std::string& studentId) {
    std::map<std::string, int> errorsByTag;
    std::map<std::string, int> totalByTag; // ÿ��֪ʶ����ܴ�����

    // ��ȡanswer_log.txt�ļ�����ȡ��ʵ�Ĵ����¼
    std::ifstream answerLog("answer_log.txt");
    if (answerLog.is_open()) {
        std::string line;
        while (std::getline(answerLog, line)) {
            std::istringstream iss(line);
            std::string id, qid, tag, result;

            // ����ÿ�У���ʽ��studentId,questionId,tag,isCorrect(0/1)
            if (std::getline(iss, id, ',') &&
                std::getline(iss, qid, ',') &&
                std::getline(iss, tag, ',') &&
                std::getline(iss, result, ',')) {

                if (id == studentId) {
                    totalByTag[tag]++;
                    if (result == "0") {
                        errorsByTag[tag]++;
                    }
                }
            }
        }
        answerLog.close();
    }

    return errorsByTag;
}

// ��ȡ�Ƽ���ϰ�� - ���ݴ��������Ƽ������Ŀ
std::vector<std::string> ReportGenerator::getRecommendedExercises(
    const std::map<std::string, int>& errorsByTag, const std::string& studentId, int count) {
    std::vector<std::string> exercises;

    // ֪ʶ�㵽��ĿID��ӳ��
    std::map<std::string, std::vector<std::string>> tagToQuestions;

    // ��ȡ����ļ�������֪ʶ�㵽��Ŀ��ӳ��
    loadQuestionDatabase(tagToQuestions);

    // ��ȡѧ���Ѿ���������Ŀ�������Ƽ��ظ���Ŀ
    std::set<std::string> doneQuestions = getStudentDoneQuestions(studentId);

    // ���û�д����¼�����ѡ��һЩ��Ŀ
    if (errorsByTag.empty()) {
        return getRandomQuestions(tagToQuestions, count, doneQuestions);
    }

    // �������������֪ʶ��
    std::vector<std::pair<std::string, int>> sortedErrors;
    for (const auto& error : errorsByTag) {
        if (error.second > 0) {
            sortedErrors.push_back(error);
        }
    }
    std::sort(sortedErrors.begin(), sortedErrors.end(),
        [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
            return a.second > b.second;
        });

    // �������
    std::cout << "\nΪѧ�� " << studentId << " ��������֪ʶ�㣺" << std::endl;
    for (const auto& error : sortedErrors) {
        std::cout << "- ֪ʶ�� [" << error.first << "] �������: " << error.second << std::endl;
    }

    // ����ÿ��֪ʶ���Ƽ���Ŀ�������ܹ��Ƽ�count����
    std::map<std::string, int> tagQuestionCounts;
    int totalErrors = 0;
    for (const auto& error : sortedErrors) {
        totalErrors += error.second;
    }

    // ���ݴ��������Ϊÿ��֪ʶ��Ӧ�Ƽ�����Ŀ����
    for (const auto& error : sortedErrors) {
        float proportion = totalErrors > 0 ? static_cast<float>(error.second) / totalErrors : 0;
        int numToRecommend = std::max(1, static_cast<int>(proportion * count));
        tagQuestionCounts[error.first] = numToRecommend;
        std::cout << "- �ƻ�Ϊ֪ʶ�� [" << error.first << "] �Ƽ� " << numToRecommend << " ����Ŀ" << std::endl;
    }

    // ���ݴ����ѡ����Ŀ
    std::set<std::string> selectedExercises;
    for (const auto& error : sortedErrors) {
        const std::string& tag = error.first;
        int numToRecommend = tagQuestionCounts[tag];

        // ���Ҹ�֪ʶ�����Ŀ
        if (tagToQuestions.find(tag) != tagToQuestions.end() && !tagToQuestions[tag].empty()) {
            const auto& questions = tagToQuestions[tag];

            // ���˵�ѧ���Ѿ���������Ŀ
            std::vector<std::string> availableQuestions;
            for (const auto& qid : questions) {
                if (doneQuestions.find(qid) == doneQuestions.end()) {
                    availableQuestions.push_back(qid);
                }
            }

            // ������˺�û�п�����Ŀ����ʹ�����и�֪ʶ����Ŀ
            if (availableQuestions.empty() && !questions.empty()) {
                std::cout << "- ֪ʶ�� [" << tag << "] û��δ��������Ŀ������������Ŀ��ѡ��" << std::endl;
                availableQuestions = questions;
            }

            // ���ѡ����Ŀ
            if (!availableQuestions.empty()) {
                std::vector<std::string> selectedFromTag = selectRandomQuestions(availableQuestions, numToRecommend);
                std::cout << "- Ϊ֪ʶ�� [" << tag << "] ѡ���� " << selectedFromTag.size() << " ����Ŀ" << std::endl;

                for (const auto& qid : selectedFromTag) {
                    selectedExercises.insert(qid);
                    if (selectedExercises.size() >= count) break;
                }
            }
            else {
                std::cout << "- ֪ʶ�� [" << tag << "] û�п��õ���Ŀ" << std::endl;
            }
        }
        else {
            std::cout << "- �����û�й���֪ʶ�� [" << tag << "] ����Ŀ" << std::endl;
        }

        if (selectedExercises.size() >= count) break;
    }

    // ����ת��Ϊ����
    exercises.assign(selectedExercises.begin(), selectedExercises.end());

    // ���ѡ�����Ŀ��������ѡ��һЩ�����Ŀ����
    if (exercises.size() < count) {
        int remainingCount = count - exercises.size();
        std::cout << "- �Ƽ���Ŀ���㣬��Ҫ����ѡ�� " << remainingCount << " ����Ŀ����" << std::endl;

        std::vector<std::string> additionalExercises = getRandomQuestions(tagToQuestions, remainingCount, doneQuestions);
        for (const auto& ex : additionalExercises) {
            if (std::find(exercises.begin(), exercises.end(), ex) == exercises.end()) {
                exercises.push_back(ex);
            }
        }
    }

    // ���ʵ���Ҳ�����Ŀ��ʹ��Ĭ����Ŀ
    if (exercises.empty()) {
        std::cout << "- δ�ҵ��ʺϵ���Ŀ��ʹ��Ĭ���Ƽ���Ŀ" << std::endl;
        exercises = { "005", "012", "017" };  // Ĭ���Ƽ���Ŀ
    }

    std::cout << "- �����Ƽ���Ŀ: ";
    for (const auto& ex : exercises) {
        std::cout << ex << " ";
    }
    std::cout << std::endl;

    return exercises;
}

// ��ȡѧ���Ѿ���������Ŀ
std::set<std::string> ReportGenerator::getStudentDoneQuestions(const std::string& studentId) {
    std::set<std::string> doneQuestions;

    std::ifstream answerLog("answer_log.txt");
    if (answerLog.is_open()) {
        std::string line;
        while (std::getline(answerLog, line)) {
            std::istringstream iss(line);
            std::string id, qid, tag, result;

            // ����ÿ�У���ʽ��studentId,questionId,tag,isCorrect(0/1)
            if (std::getline(iss, id, ',') &&
                std::getline(iss, qid, ',') &&
                std::getline(iss, tag, ',') &&
                std::getline(iss, result, ',')) {

                if (id == studentId) {
                    doneQuestions.insert(qid);
                }
            }
        }
        answerLog.close();
    }

    return doneQuestions;
}

// ������м�����Ŀ��֪ʶ��
void ReportGenerator::loadQuestionDatabase(std::map<std::string, std::vector<std::string>>& tagToQuestions) {
    // ���ȳ��Զ�ȡ��ʦ�˵�����ļ� question.txt (ANSI����)
    std::ifstream questionsFile("questions.txt", std::ios::binary); // �Զ�����ģʽ��ȷ��ANSI������ȷ��ȡ
    if (questionsFile.is_open()) {
        std::cout << "��⵽��ʦ������ļ� questions.txt (ANSI����)����ʼ����..." << std::endl;
        std::string line;
        std::string currentId;
        std::string currentTag;
        bool inQuestionBlock = false;
        int questionCount = 0;

        while (std::getline(questionsFile, line)) {
            // ȥ��BOMͷ��������ڣ�
            if (line.size() >= 3 &&
                static_cast<unsigned char>(line[0]) == 0xEF &&
                static_cast<unsigned char>(line[1]) == 0xBB &&
                static_cast<unsigned char>(line[2]) == 0xBF) {
                line = line.substr(3);
            }

            line = trimString(line);

            // �������к�ע���У���#��ͷ��
            if (line.empty() || line[0] == '#') {
                continue;
            }

            // �����ĿID�У���ʽ�磺[001]
            if (line[0] == '[' && line.find(']') != std::string::npos) {
                size_t endBracket = line.find(']');
                currentId = line.substr(1, endBracket - 1);
                inQuestionBlock = true;
                continue;
            }

            // ����Ŀ���м��֪ʶ����
            if (inQuestionBlock && line.find("֪ʶ�㣺") == 0) {
                currentTag = line.substr(strlen("֪ʶ�㣺")); // ʹ��strlenȷ��ANSI��������ȷ��ȡ
                currentTag = trimString(currentTag);

                if (!currentId.empty() && !currentTag.empty()) {
                    tagToQuestions[currentTag].push_back(currentId);
                    questionCount++;
                    std::cout << "�����ɹ�����Ŀ " << currentId << " -> ֪ʶ�� [" << currentTag << "]" << std::endl;
                }

                // ����״̬��׼����һ����Ŀ��
                inQuestionBlock = false;
                currentId.clear();
                currentTag.clear();
            }
        }
        questionsFile.close();

        if (questionCount > 0) {
            std::cout << "��������ɣ������� " << questionCount << " ����Ŀ������ "
                << tagToQuestions.size() << " ��֪ʶ��" << std::endl;
            return;
        }
        else {
            std::cout << "���棺questions.txt ��û���ҵ���Ч��Ŀ����" << std::endl;
        }
    }
    else {
        std::cout << "δ�ҵ���ʦ������ļ� questions.txt" << std::endl;
    }

    //// �󱸷���������������ʽ������ļ���ͬ������ANSI���ݣ�
    //const char* altFiles[] = { "questions.txt", "question_tags.txt" };
    //for (const char* filename : altFiles) {
    //    std::ifstream altFile(filename, std::ios::binary);
    //    if (altFile.is_open()) {
    //        std::cout << "��⵽��������ļ� " << filename << " (ANSI����)�����Խ���..." << std::endl;
    //        // ... (����ԭ�еĽ����߼�����ȷ��ʹ��ANSI���ݷ�ʽ)
    //        altFile.close();
    //        if (!tagToQuestions.empty()) return;
    //    }
    //}

    //// ���պ󱸷�����ʹ��ANSI���ݵ�ģ������
    //std::cout << "���棺ʹ�����õ�ANSI����ʾ����Ŀ" << std::endl;
    //tagToQuestions["ָ��"] = { "001", "005", "009", "021", "025" };
    //tagToQuestions["�̳�"] = { "002", "006", "012", "018", "026" };
    //tagToQuestions["��̬"] = { "003", "007", "013", "019", "027" };
    //tagToQuestions["ģ��"] = { "004", "008", "017", "020", "028" };

    //// ANSI���ݵ����
    //for (const auto& tag : tagToQuestions) {
    //    std::cout << "ģ����⣺֪ʶ�� [" << tag.first << "] ������Ŀ: ";
    //    for (const auto& id : tag.second) {
    //        std::cout << id << " ";
    //    }
    //    std::cout << std::endl;
    //}
}

// ��ָ����Ŀ���������ѡ��������Ŀ
std::vector<std::string> ReportGenerator::selectRandomQuestions(
    const std::vector<std::string>& questions, int count) {
    if (questions.empty() || count <= 0) {
        return {};
    }

    std::vector<std::string> result = questions;
    // �����Ŀ����������ֱ�ӷ���������Ŀ
    if (count >= questions.size()) {
        return result;
    }

    // ���������Ŀ˳��
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(result.begin(), result.end(), g);

    // ѡ��ǰcount����Ŀ
    result.resize(count);
    return result;
}

// �����ȡ���е���Ŀ���ų�����������Ŀ
std::vector<std::string> ReportGenerator::getRandomQuestions(
    const std::map<std::string, std::vector<std::string>>& tagToQuestions,
    int count, const std::set<std::string>& doneQuestions) {
    std::vector<std::string> availableQuestions;

    // �ռ�����δ��������Ŀ
    for (const auto& pair : tagToQuestions) {
        for (const auto& qid : pair.second) {
            if (doneQuestions.find(qid) == doneQuestions.end()) {
                availableQuestions.push_back(qid);
            }
        }
    }

    // ���û��δ��������Ŀ��ʹ��������Ŀ
    if (availableQuestions.empty()) {
        for (const auto& pair : tagToQuestions) {
            for (const auto& qid : pair.second) {
                availableQuestions.push_back(qid);
            }
        }
    }

    // ȥ��
    std::sort(availableQuestions.begin(), availableQuestions.end());
    availableQuestions.erase(
        std::unique(availableQuestions.begin(), availableQuestions.end()),
        availableQuestions.end()
    );

    return selectRandomQuestions(availableQuestions, count);
}

// ȥ���ַ���ǰ��ո�
std::string ReportGenerator::trimString(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// ������ȷ��
float ReportGenerator::calculateCorrectRate(const std::string& studentId) {
    int totalAnswers = 0;
    int correctAnswers = 0;

    // ��answer_log.txt��ȡ�����¼������ȷ��
    std::ifstream answerLog("answer_log.txt");
    if (answerLog.is_open()) {
        std::string line;
        while (std::getline(answerLog, line)) {
            std::istringstream iss(line);
            std::string id, qid, tag, result;

            // ����ÿ�У���ʽ��studentId,questionId,tag,isCorrect(0/1)
            if (std::getline(iss, id, ',') &&
                std::getline(iss, qid, ',') &&
                std::getline(iss, tag, ',') &&
                std::getline(iss, result, ',')) {

                if (id == studentId) {
                    totalAnswers++;
                    if (result == "1") {
                        correctAnswers++;
                    }
                }
            }
        }
        answerLog.close();
    }

    // ���û�д�answer_log.txt�ҵ���¼�����Դ�scores.csv��ȡ
    if (totalAnswers == 0) {
        std::ifstream scores("scores.csv");
        if (scores.is_open()) {
            std::string line;
            // ������ͷ��
            std::getline(scores, line);

            while (std::getline(scores, line)) {
                std::istringstream iss(line);
                std::string id, score, date;

                if (std::getline(iss, id, ',') &&
                    std::getline(iss, score, ',')) {

                    if (id == studentId) {
                        totalAnswers++;
                        int scoreValue = std::stoi(score);
                        if (scoreValue >= 60) correctAnswers++;  // �������ڵ���60��Ϊ��ȷ
                    }
                }
            }
            scores.close();
        }
    }

    // ������ȷ��
    if (totalAnswers == 0) return 1.0f;  // ���û�д����¼������100%��ȷ��

    return static_cast<float>(correctAnswers) / totalAnswers;
}

// ����Ƿ���һ
bool ReportGenerator::isTimeToGenerateReport() {
    time_t now = time(nullptr);
    struct tm t;
    localtime_s(&t, &now);

    return t.tm_wday == 1;  // 1��ʾ��һ
}
