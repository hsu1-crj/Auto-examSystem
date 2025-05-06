#pragma once
#include <string>
#include <vector>

class Question {
private:
    std::string id;
    std::string content;
    std::vector<std::string> options;
    std::vector<std::string> answer;
    std::string tag;        // ֪ʶ���ǩ
    int difficulty;         // �Ѷ�ϵ�� 1~5

public:
    Question();
    Question(const std::string& qId, const std::string& qContent,
        const std::vector<std::string>& qOptions,
        const std::vector<std::string>& qAnswer,
        const std::string& qTag, int qDifficulty);

    // �ļ���д
    void saveToFile(const std::string& filename) const;
    static std::vector<Question> loadFromFile(const std::string& filename);

    // ���û����봴����Ŀ
    static Question createFromInput();

    // Getter/Setter
    std::string getId() const;
    void setId(const std::string& newId);

    std::string getContent() const;
    void setContent(const std::string& newContent);

    std::vector<std::string> getAnswer() const;
    void setAnswer(const std::vector<std::string>& newAnswer);

    std::vector<std::string> getOptions() const;  // ��������
    void setOptions(const std::vector<std::string>& newOptions);

    std::string getTag() const;
    int getDifficulty() const;

    // ������
    bool checkAnswer(const std::vector<std::string>& userAnswer) const;
    //��Ŀ����
    void setTag(const std::string& newTag);
    void setDifficulty(int newDifficulty);
};