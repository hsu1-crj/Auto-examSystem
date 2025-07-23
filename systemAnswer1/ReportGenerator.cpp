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
    // 检查是否周一
    if (isTimeToGenerateReport() && !isManualGeneration) {
        std::cout << "开始生成周一学习报告..." << std::endl;
    }

    // 计算正确率
    float correctRate = calculateCorrectRate(studentId);

    // 统计错误知识点
    std::map<std::string, int> errorsByTag = countErrorsByTag(studentId);

    // 获取推荐练习题 - 直接传入学生ID给该函数内部处理逻辑
    std::vector<std::string> recommendExercises = getRecommendedExercises(errorsByTag, studentId);

    // 生成报告文件名
    std::string filename = "report_" + studentId + ".txt";

    // 写入报告
    std::ofstream report(filename);
    if (report.is_open()) {
        report << "正确率：" << static_cast<int>(correctRate * 100) << "%" << std::endl;

        report << "薄弱知识点：";
        bool firstTag = true;
        // 排序错误数据
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
                if (!firstTag) report << "，";
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
        time_t now = time(nullptr);//time_t获取时间，并存储到now
        struct tm t;
        localtime_s(&t, &now);//存储本地时间后的时间戳
        char timeBuf[64];//存储格式时间字符串
        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M", &t);//4y 2mon 2d 2h 2min
        report << "\n报告生成时间: " << timeBuf << std::endl;

        report.close();
        std::cout << "已生成学习报告：" << filename << std::endl;

        // 输出详细推荐依据
        std::cout << "\n推荐依据：" << std::endl;
        if (!sortedErrors.empty()) {
            for (size_t i = 0; i < sortedErrors.size() && i < 3; ++i) {
                std::cout << "- 针对知识点 [" << sortedErrors[i].first << "] (错误"
                    << sortedErrors[i].second << "次) 推荐相关题目" << std::endl;
            }
        }
        else {
            std::cout << "- 未发现明显薄弱知识点，推荐综合练习题目" << std::endl;
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

    // 读取answer_log.txt文件，获取真实的错误记录
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

// 获取推荐练习题 - 根据错误数据推荐相关题目
std::vector<std::string> ReportGenerator::getRecommendedExercises(
    const std::map<std::string, int>& errorsByTag, const std::string& studentId, int count) {
    std::vector<std::string> exercises;

    // 知识点到题目ID的映射
    std::map<std::string, std::vector<std::string>> tagToQuestions;

    // 读取题库文件，建立知识点到题目的映射
    loadQuestionDatabase(tagToQuestions);

    // 获取学生已经做过的题目，避免推荐重复题目
    std::set<std::string> doneQuestions = getStudentDoneQuestions(studentId);

    // 如果没有错误记录，随机选择一些题目
    if (errorsByTag.empty()) {
        return getRandomQuestions(tagToQuestions, count, doneQuestions);
    }

    // 按错误次数排序知识点
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

    // 输出分析
    std::cout << "\n为学生 " << studentId << " 分析薄弱知识点：" << std::endl;
    for (const auto& error : sortedErrors) {
        std::cout << "- 知识点 [" << error.first << "] 错误次数: " << error.second << std::endl;
    }

    // 计算每个知识点推荐题目数量，总共推荐count道题
    std::map<std::string, int> tagQuestionCounts;
    int totalErrors = 0;
    for (const auto& error : sortedErrors) {
        totalErrors += error.second;
    }

    // 根据错误比例，为每个知识点应推荐的题目数量
    for (const auto& error : sortedErrors) {
        float proportion = totalErrors > 0 ? static_cast<float>(error.second) / totalErrors : 0;
        int numToRecommend = std::max(1, static_cast<int>(proportion * count));
        tagQuestionCounts[error.first] = numToRecommend;
        std::cout << "- 计划为知识点 [" << error.first << "] 推荐 " << numToRecommend << " 道题目" << std::endl;
    }

    // 根据错误点选择题目
    std::set<std::string> selectedExercises;
    for (const auto& error : sortedErrors) {
        const std::string& tag = error.first;
        int numToRecommend = tagQuestionCounts[tag];

        // 查找该知识点的题目
        if (tagToQuestions.find(tag) != tagToQuestions.end() && !tagToQuestions[tag].empty()) {
            const auto& questions = tagToQuestions[tag];

            // 过滤掉学生已经做过的题目
            std::vector<std::string> availableQuestions;
            for (const auto& qid : questions) {
                if (doneQuestions.find(qid) == doneQuestions.end()) {
                    availableQuestions.push_back(qid);
                }
            }

            // 如果过滤后没有可用题目，则使用所有该知识点题目
            if (availableQuestions.empty() && !questions.empty()) {
                std::cout << "- 知识点 [" << tag << "] 没有未做过的题目，将从所有题目中选择" << std::endl;
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

    // 集合转换为数组
    exercises.assign(selectedExercises.begin(), selectedExercises.end());

    // 如果选择的题目不够，则选择一些随机题目补充
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
        std::cout << "- 未找到适合的题目，使用默认推荐题目" << std::endl;
        exercises = { "005", "012", "017" };  // 默认推荐题目
    }

    std::cout << "- 最终推荐题目: ";
    for (const auto& ex : exercises) {
        std::cout << ex << " ";
    }
    std::cout << std::endl;

    return exercises;
}

// 获取学生已经做过的题目
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

// 从题库中加载题目及知识点
void ReportGenerator::loadQuestionDatabase(std::map<std::string, std::vector<std::string>>& tagToQuestions) {
    // 优先尝试读取教师端的题库文件 question.txt (ANSI编码)
    std::ifstream questionsFile("questions.txt", std::ios::binary); // 以二进制模式打开确保ANSI编码正确读取
    if (questionsFile.is_open()) {
        std::cout << "检测到教师端题库文件 questions.txt (ANSI编码)，开始解析..." << std::endl;
        std::string line;
        std::string currentId;
        std::string currentTag;
        bool inQuestionBlock = false;
        int questionCount = 0;

        while (std::getline(questionsFile, line)) {
            // 去除BOM头（如果存在）
            if (line.size() >= 3 &&
                static_cast<unsigned char>(line[0]) == 0xEF &&
                static_cast<unsigned char>(line[1]) == 0xBB &&
                static_cast<unsigned char>(line[2]) == 0xBF) {
                line = line.substr(3);
            }

            line = trimString(line);

            // 跳过空行和注释行（以#开头）
            if (line.empty() || line[0] == '#') {
                continue;
            }

            // 检测题目ID行，格式如：[001]
            if (line[0] == '[' && line.find(']') != std::string::npos) {
                size_t endBracket = line.find(']');
                currentId = line.substr(1, endBracket - 1);
                inQuestionBlock = true;
                continue;
            }

            // 在题目块中检测知识点行
            if (inQuestionBlock && line.find("知识点：") == 0) {
                currentTag = line.substr(strlen("知识点：")); // 使用strlen确保ANSI编码下正确截取
                currentTag = trimString(currentTag);

                if (!currentId.empty() && !currentTag.empty()) {
                    tagToQuestions[currentTag].push_back(currentId);
                    questionCount++;
                    std::cout << "解析成功：题目 " << currentId << " -> 知识点 [" << currentTag << "]" << std::endl;
                }

                // 重置状态，准备下一个题目块
                inQuestionBlock = false;
                currentId.clear();
                currentTag.clear();
            }
        }
        questionsFile.close();

        if (questionCount > 0) {
            std::cout << "题库加载完成，共解析 " << questionCount << " 道题目，涵盖 "
                << tagToQuestions.size() << " 个知识点" << std::endl;
            return;
        }
        else {
            std::cout << "警告：questions.txt 中没有找到有效题目数据" << std::endl;
        }
    }
    else {
        std::cout << "未找到教师端题库文件 questions.txt" << std::endl;
    }

    //// 后备方案：尝试其他格式的题库文件（同样保持ANSI兼容）
    //const char* altFiles[] = { "questions.txt", "question_tags.txt" };
    //for (const char* filename : altFiles) {
    //    std::ifstream altFile(filename, std::ios::binary);
    //    if (altFile.is_open()) {
    //        std::cout << "检测到备用题库文件 " << filename << " (ANSI编码)，尝试解析..." << std::endl;
    //        // ... (保留原有的解析逻辑，但确保使用ANSI兼容方式)
    //        altFile.close();
    //        if (!tagToQuestions.empty()) return;
    //    }
    //}

    //// 最终后备方案：使用ANSI兼容的模拟数据
    //std::cout << "警告：使用内置的ANSI兼容示例题目" << std::endl;
    //tagToQuestions["指针"] = { "001", "005", "009", "021", "025" };
    //tagToQuestions["继承"] = { "002", "006", "012", "018", "026" };
    //tagToQuestions["多态"] = { "003", "007", "013", "019", "027" };
    //tagToQuestions["模板"] = { "004", "008", "017", "020", "028" };

    //// ANSI兼容的输出
    //for (const auto& tag : tagToQuestions) {
    //    std::cout << "模拟题库：知识点 [" << tag.first << "] 包含题目: ";
    //    for (const auto& id : tag.second) {
    //        std::cout << id << " ";
    //    }
    //    std::cout << std::endl;
    //}
}

// 从指定题目集合中随机选择若干题目
std::vector<std::string> ReportGenerator::selectRandomQuestions(
    const std::vector<std::string>& questions, int count) {
    if (questions.empty() || count <= 0) {
        return {};
    }

    std::vector<std::string> result = questions;
    // 如果题目数量不够，直接返回所有题目
    if (count >= questions.size()) {
        return result;
    }

    // 随机打乱题目顺序
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(result.begin(), result.end(), g);

    // 选择前count道题目
    result.resize(count);
    return result;
}

// 随机获取所有的题目，排除已做过的题目
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
            // 跳过表头行
            std::getline(scores, line);

            while (std::getline(scores, line)) {
                std::istringstream iss(line);
                std::string id, score, date;

                if (std::getline(iss, id, ',') &&
                    std::getline(iss, score, ',')) {

                    if (id == studentId) {
                        totalAnswers++;
                        int scoreValue = std::stoi(score);
                        if (scoreValue >= 60) correctAnswers++;  // 分数大于等于60视为正确
                    }
                }
            }
            scores.close();
        }
    }

    // 返回正确率
    if (totalAnswers == 0) return 1.0f;  // 如果没有答题记录，返回100%正确率

    return static_cast<float>(correctAnswers) / totalAnswers;
}

// 检查是否周一
bool ReportGenerator::isTimeToGenerateReport() {
    time_t now = time(nullptr);
    struct tm t;
    localtime_s(&t, &now);

    return t.tm_wday == 1;  // 1表示周一
}
