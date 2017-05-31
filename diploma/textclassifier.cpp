#include "textclassifier.h"

//На вход подаем примеры открытых и шифрованных текстов
//На этапе обучения даем еще и правильный ответ через answer (если answer < 0 - обучение завершилось)
TextClassifier::resultType TextClassifier::Process(const std::string &example, int answer)
//Строку лучше по ссылке передавать во избежании лишнего копирования
{
    TextClassifier::resultType result;
	result.classNumber = -1;
	for (int i = 0; i < example.length() - _wordSize; i++)
	{
		int n1, n2;
		std::string sec = example.substr(i, _wordSize);
			
		if (_openText.find(sec) == _openText.end())
            n1 = 0;
		else
			n1 = _openText.find(sec)->second;
		if (_encryptText.find(sec) == _encryptText.end())
            n2 = 0;
		else
			n2 = _encryptText.find(sec)->second;
		if (n1 > n2) {
			result.classNumber = 0;
			result.symbolsToRecognize = i + _wordSize;
			result.probability = n1 / double(n1 + n2);
			if (answer < 0)
				break;
		}
		if (n1 < n2) {
			result.classNumber = 1;
			result.symbolsToRecognize = i + _wordSize;
			result.probability = n2 / double(n1 + n2);
			if (answer < 0)
				break;
		}

        //Надо все-таки этот switch засунуть в отдельный цикл, тогда и
        //'if (answer < 0)break;' не нужен (можно сразу break), а то он будет при обучении оценивать текст по последней
        //подстроке в example, что плохо, т.к. способы оценки при обучении и нормальной работе будут разные.
        switch (answer)
		{
		case 1:
			_openText[sec]++;
			break;
		case 2:
			_encryptText[sec]++;
			break;
		}
	}

    return result;
}
