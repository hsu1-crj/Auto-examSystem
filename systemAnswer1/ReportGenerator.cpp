#include "ReportGenerator.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>
#include <ctime>
#include <random>

// 初始化静态变量
bool ReportGenerator::isManualGeneration = false;

// 生成周报告
void ReportGenerator::generateWeeklyReport(const std::string& studentId) {
    // 检查是否是周一
    if (isTimeToGenerateReport() && !isManualGeneration) {
        std::cout << "开始生成周一学习报告..." << std::endl;
    }

    // 计算正确率
    float correctRate = calculateCorrectRate(studentId);

    // 统计错题知识点
    std::map<std::string, int> errorsByTag = countErrorsByTag(studentId);

    // 获取推荐练习题 - 直接传入学生ID，让函数内部处理逻辑
    std::vector<std::string> recommendExercises = getRecommendedExercises(errorsByTag, studentId);

    // 生成报告文件名
    std::string filename = "report_" + studentId + ".txt";

    // 写入报告
    std::ofstream report(filename);
    if (report.is_open()) {
        report << "正确率：" << static_cast<int>(correctRate * 100) << "%" << std::endl;

        report << "薄弱知识点：";
        bool firstTag = true;
        // 按错误次数排序
        std::vector<std::pair<std::string, int>> sortedErrors;
        for (const auto& error : errorsByTag) {
            if (error.second > 0) {
                sortedErrors.push_back(error);
            }
        }

        // 按错误次数降序排序
        std::sort(sortedErrors.begin(), sortedErrors.end(),
            [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                return a.second > b.second;
            });

        // 输出薄弱知识点
        if (sortedErrors.empty()) {
            report << "暂无薄弱知识点";
        }
        else {
            // 输出前3个薄弱知识点
            for (size_t i = 0; i < sortedErrors.size() && i < 3; ++i) {
                if (!firstTag) report << "、";
                report << sortedErrors[i].first << "（错误" << sortedErrors[i].second << "次）";
                firstTag = false;
            }
        }
        report << std::endl;

        // 输出推荐练习
        report << "推荐练习：";
        if (recommendExercises.empty()) {
            report << "暂无推荐练习";
        }
        else {
            for (size_t i = 0; i < recommendExercises.size(); ++i) {
                report << "题目ID " << recommendExercises[i];
                if (i < recommendExercises.size() - 1) report << ", ";
            }
        }
        report << std::endl;

        // 添加报告生成时间
        time_t now = time(nullptr);
        struct tm t;
        localtime_s(&t, &now);
        char timeBuf[64];
        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M", &t);
        report << "\n报告生成时间: " << timeBuf << std::endl;

        report.close();
        std::cout << "已生成学习报告：" << filename << std::endl;

        // 输出详细的推荐依据
        std::cout << "\n推荐依据：" << std::endl;
        if (!sortedErrors.empty()) {
            for (size_t i = 0; i < sortedErrors.size() && i < 3; ++i) {
                std::cout << "- 针对知识点 [" << sortedErrors[i].first << "] (错误"
                    << sortedErrors[i].second << "次) 推荐相关题目" << std::endl;
            }
        }
        else {
            std::cout << "- 未发现明显薄弱知识点，推荐基础练习题目" << std::endl;
        }
    }
    else {
        std::cerr << "无法创建报告文件！" << std::endl;
    }
}

// 统计每个知识点的错误次数
std::map<std::string, int> ReportGenerator::countErrorsByTag(const std::string& studentId) {
    std::map<std::string, int> errorsByTag;
    std::map<std::string, int> totalByTag; // 每个知识点的总答题数

    // 读取answer_log.txt文件，获取真实的答题记录
    std::ifstream answerLog("answer_log.txt");
    if (answerLog.is_open()) {
        std::string line;
        while (std::getline(answerLog, line)) {
            std::istringstream iss(line);
            std::string id, qid, tag, result;

            // 解析每行，格式：studentId,questionId,tag,isCorrect(0/1)
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

// 获取推荐练习题 - 根据错误率推荐相关题目
std::vector<std::string> ReportGenerator::getRecommendedExercises(
    const std::map<std::string, int>& errorsByTag, const std::string& studentId, int count) {
    std::vector<std::string> exercises;

    // 创建知识点到题目ID的映射
    std::map<std::string, std::vector<std::string>> tagToQuestions;

    // 读取题库文件，构建知识点到题目的映射
    loadQuestionDatabase(tagToQuestions);

    // 读取学生已做过的题目，避免推荐重复题目
    std::set<std::string> doneQuestions = getStudentDoneQuestions(studentId);

    // 如果没有错题记录，但我们有题库，随机选择一些题目
    if (errorsByTag.empty()) {
        return getRandomQuestions(tagToQuestions, count, doneQuestions);
    }

    // 按错误次数对知识点排序
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

    // 调试输出
    std::cout << "\n为学生 " << studentId << " 分析薄弱知识点：" << std::endl;
    for (const auto& error : sortedErrors) {
        std::cout << "- 知识点 [" << error.first << "] 错误次数: " << error.second << std::endl;
    }

    // 按比例分配推荐题目数量，最多推荐count道题
    std::map<std::string, int> tagQuestionCounts;
    int totalErrors = 0;
    for (const auto& error : sortedErrors) {
        totalErrors += error.second;
    }

    // 根据错误次数分配每个知识点应推荐的题目数量
    for (const auto& error : sortedErrors) {
        float proportion = totalErrors > 0 ? static_cast<float>(error.second) / totalErrors : 0;
        int numToRecommend = std::max(1, static_cast<int>(proportion * count));
        tagQuestionCounts[error.first] = numToRecommend;
        std::cout << "- 计划为知识点 [" << error.first << "] 推荐 " << numToRecommend << " 道题目" << std::endl;
    }

    // 根据错误率选择题目
    std::set<std::string> selectedExercises;
    for (const auto& error : sortedErrors) {
        const std::string& tag = error.first;
        int numToRecommend = tagQuestionCounts[tag];

        // 如果这个知识点有题目
        if (tagToQuestions.find(tag) != tagToQuestions.end() && !tagToQuestions[tag].empty()) {
            const auto& questions = tagToQuestions[tag];

            // 过滤掉学生已经做过的题目
            std::vector<std::string> availableQuestions;
            for (const auto& qid : questions) {
                if (doneQuestions.find(qid) == doneQuestions.end()) {
                    availableQuestions.push_back(qid);
                }
            }

            // 如果过滤后没有可用题目，考虑使用已做过的题目
            if (availableQuestions.empty() && !questions.empty()) {
                std::cout << "- 知识点 [" << tag << "] 没有未做过的题目，将从已做题目中选择" << std::endl;
                availableQuestions = questions;
            }

            // 随机选择题目
            if (!availableQuestions.empty()) {
                std::vector<std::string> selectedFromTag = selectRandomQuestions(availableQuestions, numToRecommend);
                std::cout << "- 为知识点 [" << tag << "] 选择了 " << selectedFromTag.size() << " 道题目" << std::endl;

                for (const auto& qid : selectedFromTag) {
                    selectedExercises.insert(qid);
                    if (selectedExercises.size() >= count) break;
                }
            }
            else {
                std::cout << "- 知识点 [" << tag << "] 没有可用的题目" << std::endl;
            }
        }
        else {
            std::cout << "- 题库中没有关于知识点 [" << tag << "] 的题目" << std::endl;
        }

        if (selectedExercises.size() >= count) break;
    }

    // 将集合转换为向量
    exercises.assign(selectedExercises.begin(), selectedExercises.end());

    // 如果选择的题目不足，随机选择一些其他题目补充
    if (exercises.size() < count) {
        int remainingCount = count - exercises.size();
        std::cout << "- 推荐题目不足，需要额外选择 " << remainingCount << " 道题目补充" << std::endl;

        std::vector<std::string> additionalExercises = getRandomQuestions(tagToQuestions, remainingCount, doneQuestions);
        for (const auto& ex : additionalExercises) {
            if (std::find(exercises.begin(), exercises.end(), ex) == exercises.end()) {
                exercises.push_back(ex);
            }
        }
    }

    // 如果实在找不到题目，使用默认题目
    if (exercises.empty()) {
        std::cout << "- 未找到合适的题目，使用默认推荐题目" << std::endl;
        exercises = { "005", "012", "017" };  // 默认推荐题目
    }

    std::cout << "- 最终推荐题目: ";
    for (const auto& ex : exercises) {
        std::cout << ex << " ";
    }
    std::cout << std::endl;

    return exercises;
}

// 获取学生已做过的题目
std::set<std::string> ReportGenerator::getStudentDoneQuestions(const std::string& studentId) {
    std::set<std::string> doneQuestions;

    std::ifstream answerLog("answer_log.txt");
    if (answerLog.is_open()) {
        std::string line;
        while (std::getline(answerLog, line)) {
            std::istringstream iss(line);
            std::string id, qid, tag, result;

            // 解析每行，格式：studentId,questionId,tag,isCorrect(0/1)
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

// 从题库加载所有题目及其知识点
void ReportGenerator::loadQuestionDatabase(std::map<std::string, std::vector<std::string>>& tagToQuestions) {
    // 读取题库信息 - 先尝试从questions.txt读取
    std::ifstream questionsFile("questions.txt");
    if (questionsFile.is_open()) {
        std::string line;
        std::string currentId;
        std::string currentTag;

        while (std::getline(questionsFile, line)) {
            // 解析题目ID
            if (line.find("[") == 0 && line.find("]") != std::string::npos) {
                currentId = line.substr(1, line.find("]") - 1);
            }
            // 解析知识点标签 (假设格式为 "TAG:知识点")
            else if (line.find("TAG:") == 0) {
                currentTag = line.substr(4);
                currentTag = trimString(currentTag); // 清除前后空格
                if (!currentTag.empty() && !currentId.empty()) {
                    tagToQuestions[currentTag].push_back(currentId);
                    std::cout << "题库：将题目 " << currentId << " 归类到知识点 [" << currentTag << "]" << std::endl;
                }
            }
        }
        questionsFile.close();
    }

    // 如果没有读到任何题目，尝试从question_tags.txt读取
    if (tagToQuestions.empty()) {
        std::ifstream tagsFile("question_tags.txt");
        if (tagsFile.is_open()) {
            std::string line;
            while (std::getline(tagsFile, line)) {
                std::istringstream iss(line);
                std::string id, tag;

                // 假设每行的格式是 "题目ID,知识点"
                if (std::getline(iss, id, ',') && std::getline(iss, tag)) {
                    tag = trimString(tag); // 清除前后空格
                    if (!tag.empty() && !id.empty()) {
                        tagToQuestions[tag].push_back(id);
                        std::cout << "题库：将题目 " << id << " 归类到知识点 [" << tag << "]" << std::endl;
                    }
                }
            }
            tagsFile.close();
        }
    }

    // 如果仍然没有题目，创建一些模拟数据用于测试
    if (tagToQuestions.empty()) {
        std::cout << "警告：未找到题库数据，使用内置的示例题目" << std::endl;
        tagToQuestions["指针"] = { "001", "005", "009", "021", "025" };
        tagToQuestions["继承"] = { "002", "006", "012", "018", "026" };
        tagToQuestions["多态"] = { "003", "007", "013", "019", "027" };
        tagToQuestions["模板"] = { "004", "008", "017", "020", "028" };

        // 输出模拟题库
        for (const auto& tag : tagToQuestions) {
            std::cout << "模拟题库：知识点 [" << tag.first << "] 包含题目: ";
            for (const auto& id : tag.second) {
                std::cout << id << " ";
            }
            std::cout << std::endl;
        }
    }
}

// 从指定题目集合中随机选择若干题目
std::vector<std::string> ReportGenerator::selectRandomQuestions(
    const std::vector<std::string>& questions, int count) {
    if (questions.empty() || count <= 0) {
        return {};
    }

    std::vector<std::string> result = questions;
    // 如果题目数量不足，直接返回所有题目
    if (count >= questions.size()) {
        return result;
    }

    // 随机打乱题目顺序
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(result.begin(), result.end(), g);

    // 选择前count个题目
    result.resize(count);
    return result;
}

// 随机获取题库中的题目，排除已做过的题目
std::vector<std::string> ReportGenerator::getRandomQuestions(
    const std::map<std::string, std::vector<std::string>>& tagToQuestions,
    int count, const std::set<std::string>& doneQuestions) {
    std::vector<std::string> availableQuestions;

    // 收集所有未做过的题目
    for (const auto& pair : tagToQuestions) {
        for (const auto& qid : pair.second) {
            if (doneQuestions.find(qid) == doneQuestions.end()) {
                availableQuestions.push_back(qid);
            }
        }
    }

    // 如果没有未做过的题目，使用所有题目
    if (availableQuestions.empty()) {
        for (const auto& pair : tagToQuestions) {
            for (const auto& qid : pair.second) {
                availableQuestions.push_back(qid);
            }
        }
    }

    // 去重
    std::sort(availableQuestions.begin(), availableQuestions.end());
    availableQuestions.erase(
        std::unique(availableQuestions.begin(), availableQuestions.end()),
        availableQuestions.end()
    );

    return selectRandomQuestions(availableQuestions, count);
}

// 去除字符串前后空格
std::string ReportGenerator::trimString(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// 计算正确率
float ReportGenerator::calculateCorrectRate(const std::string& studentId) {
    int totalAnswers = 0;
    int correctAnswers = 0;

    // 从answer_log.txt读取答题记录计算正确率
    std::ifstream answerLog("answer_log.txt");
    if (answerLog.is_open()) {
        std::string line;
        while (std::getline(answerLog, line)) {
            std::istringstream iss(line);
            std::string id, qid, tag, result;

            // 解析每行，格式：studentId,questionId,tag,isCorrect(0/1)
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

    // 如果没有从answer_log.txt找到记录，尝试从scores.csv读取
    if (totalAnswers == 0) {
        std::ifstream scores("scores.csv");
        if (scores.is_open()) {
            std::string line;
            // 跳过标题行
            std::getline(scores, line);

            while (std::getline(scores, line)) {
                std::istringstream iss(line);
                std::string id, score, date;

                if (std::getline(iss, id, ',') &&
                    std::getline(iss, score, ',')) {

                    if (id == studentId) {
                        totalAnswers++;
                        int scoreValue = std::stoi(score);
                        if (scoreValue >= 60) correctAnswers++;  // 分数大于等于60分视为答对
                    }
                }
            }
            scores.close();
        }
    }

    // 避免除以零
    if (totalAnswers == 0) return 1.0f;  // 如果没有答题记录，返回100%正确率

    return static_cast<float>(correctAnswers) / totalAnswers;
}

// 检查是否是周一
bool ReportGenerator::isTimeToGenerateReport() {
    time_t now = time(nullptr);
    struct tm t;
    localtime_s(&t, &now);

    // 周一是1，周日是0 (0-6表示周日到周六)
    return t.tm_wday == 1;  // 1表示周一
}