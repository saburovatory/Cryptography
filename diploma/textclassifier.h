#ifndef TEXTCLASSIFIER_H
#define TEXTCLASSIFIER_H

#include <map>
#include <string>

class TextClassifier
{
public:
    struct resultType
    {
        int classNumber = 0;
        int symbolsToRecognize = 0;
        double probability = 0;
    };

    resultType Process(const std::string example, int answer);

private:

    //Хранилище для статистики открытых текстов
    std::map<std::string, int> _openText;

    //Хранилище для статистики шифрованных текстов
    std::map<std::string, int> _encryptText;

    //Высота дерева, она же размер слова, т.е. 2 - биграммы, 3 - триграммы и т.п.
    //Это длинна подстроки, которую надо подставлять в качестве аргумента в _openText и _encryptText;
    int _wordSize = 3;
};

#endif // TEXTCLASSIFIER_H
