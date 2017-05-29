#include "textclassifier.h"

//На вход подаем примеры открытых и шифрованных текстов
//На этапе обучения даем еще и правильный ответ через answer (если answer < 0 - обучение завершилось)
TextClassifier::resultType TextClassifier::Process(const std::string &example, int answer)
//Строку лучше по ссылке передавать во избежании лишнего копирования
{
    TextClassifier::resultType result;
	result.classNumber = -1;
    int n1, n2;//Лучше перенести во внутрь цикла, ведь они нужны только там
	for (int i = 0; i < example.length() - _wordSize; i++)
	{
		std::string sec = example.substr(i, _wordSize);
        //Немного не та логика: даже если идет обучение, мы все равно должны определить,
        //к какому классу принадлежит текст по результатам предыдущего обучения (для контроля ошибок),
        //так что здесь if-else не подходит
        //if надо убрать, оставить в цикле только else ветку, а обучение (т.е. кусок под if)
        //запихнуть после этого цикла внутри второго похожего цикла, но до return result;
        if (answer > 0)
			switch (answer)
			{
			case 1: 
				_openText[sec]++;
				break;
			case 2:
				_encryptText[sec]++;
				break;
			}
        else
		{
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
				result.probability = (double)(n1 - n2) / n1;
                //а почему так, а не n1/double(n1+n2)?
				break;
			}
			if (n1 < n2) {
				result.classNumber = 1;
				result.symbolsToRecognize = i + _wordSize;
				result.probability = (double)(n2 - n1) / n2;
                //аналогично
				break;
			}
		}
	}

    //Здесь совершается полезная работа:
    //1. Выделяем из example подстроку substr длиной _wordSize, начиная с 0 элемента
    //2. Подставляем substr в хранилища _openText и _encryptText и извлекаем из них
    //n1 - число повторений в _openText и n2 - число повторений в _encryptText
    //3. Если n1 == n2, то наращиваем счетчик просмотренных символов и выделяем
    //новую подстроку уже начиная с 1 символа, а потом повторяем шаг 2
    //Если n1 > n2 - текст открытый, если n1 < n2 - шифрованный
    //4. Если answer >= 0, то мы пока что обучаемся. В таком случае пробегаем строкой substr
    //по всему example и обновляем статистики_openText и _encryptText
    //5. Если answer < 0 - ничего не сканируем и не обновляем.

    //Вам надо будет заполнить поля result
    //result.classNumber - номер класса, который определил классификатор:
    //0 - открытый, 1 - закрытый, -1 - не знаю (дошли до конца example, но решение принять не смогли)
    //result.symbolsToRecognize - число символов из example, которое потребовалось для анализа
    //result.probability - вероятность достоверности вашего ответа (определяем по n1 и n2)
    return result;
}
