#include <fstream> 
#include <iostream> 
#include <map> 
#include <bitset>
#include <locale.h>
#include <time.h>
#include <cstring>
#include <cmath>

using namespace std;

#define INPUT "input.txt"
#define LETTERS "input_only_letters.txt"
#define LETTERS_BYTE "letters_byte.txt"
#define SYMBOL1 "count_1_symbol.txt"
#define SYMBOL2 "count_2_symbols.txt"
#define INPUT_CRYPT_BYTE "input_crypt_byte.txt"
#define INPUT_CRYPT "input_crypt.txt"
#define INPUT_DECRYPT "input_decrypt.txt"
#define KEY "key.txt"

struct cmp_str
{
	bool operator()(char const *a, char const *b) const
	{
		return strcmp(a, b) < 0;
	}
};

int LFSR(void)
{
	//static unsigned long S = 0x00000001;
	//S = ((((S >> 31) ^ (S >> 30) ^ (S >> 29) ^ (S >> 27) ^ (S >> 25) ^ S) & 0x00000001) << 31) | (S >> 1);
	//return S & 0x00000001;
	static unsigned long S = 0x00000001;
	S = ((((S >> 32) ^ (S >> 22) ^ (S >> 2) ^ (S >> 1)) & 0x00000001) << 31) | (S >> 1);
	return S & 0x00000001;
}

void create_gystogram(map<char, int> map) {

}

int main()
{
    setlocale(0, "RUS");

	ifstream f_input;
	ofstream f_letters, f_count1, f_count2, f_crypt_byte, f_decrypt, f_key, f_crypt, f_letters_byte;

	f_input.open(INPUT);
	f_letters_byte.open(LETTERS_BYTE);
	f_letters.open(LETTERS);
	f_count1.open(SYMBOL1);
	f_count2.open(SYMBOL2);
	f_crypt.open(INPUT_CRYPT);
	f_crypt_byte.open(INPUT_CRYPT_BYTE);
	f_decrypt.open(INPUT_DECRYPT);
	f_key.open(KEY);

	char symbol;
	int symbols_count = 0;
	bitset<8> symbol_byte;
	long start = clock();
    cout << "Удаление лишних символов и преобразование в двоичную систему: ";

	while ((symbol = f_input.get()) != -1)
		if (symbol <= 'Z' && symbol >= 'A' || symbol <= 'z' && symbol >= 'a' || symbol == ' ')
		{
			symbol_byte = bitset<8>(symbol);
			symbols_count++;
			f_letters_byte << symbol_byte;
			f_letters << symbol;
		}
	long finish = clock();
	cout << (finish - start) / 1000.0 << " c" << endl;

	symbols_count *= 8;

	f_input.close();
	f_letters.close();
	f_letters_byte.close();

	ifstream ifs(LETTERS);

	char symbol1, symbol2;
	map<char, int> mp1;
	map<char*, int, cmp_str> mp2;

	cout << "Подсчет одиночных и парных символов: ";
	start = clock();

    char sec[3];
	while (ifs.get(symbol1) && ifs.get(symbol2))
	{
        //char *sec = (char *)malloc(3);
		sec[0] = symbol1;
		sec[1] = symbol2;
		sec[2] = 0;
		ifs.seekg(-1, ios_base::cur);
		mp1[symbol1]++;
		mp2[sec]++;
	}
	finish = clock();
	cout << (finish - start) / 1000.0 << " c" << endl;
	create_gystogram(mp1);

	for (map<char, int>::iterator p = mp1.begin(); p != mp1.end(); ++p) // проходим по всем элементам 
		f_count1<<p->first <<':'<<p->second <<endl;
	f_count1.close();

	for (map<char*, int>::iterator p1 = mp2.begin(); p1 != mp2.end(); ++p1)
		f_count2<<p1->first<<' '<<p1->second<<endl;
	f_count2.close();

	for (int i = 0; i < 32; i++)
		LFSR();

	ifs.close();

	ifs.open(LETTERS_BYTE);

	char s[8];

	cout << "Создание ключа и шифрование: ";
	start = clock();

	for (int i = 1; i <= symbols_count; i++) {
		int byte = LFSR(), symbol = ifs.get();
		f_key << byte;
		f_crypt_byte << (byte ^ (symbol - '0'));
		s[7 - ((i - 1) % 8)] = (byte ^ (symbol - '0'));
		if (i % 8 == 0) {
			for (int j = 7; j >= 0; j--) {
				symbol += (int)s[j] * pow(2, j);
			}
			f_crypt << (char)symbol;
		}
	}
	finish = clock();
	cout << (finish - start) / 1000.0 << " c" << endl;

	ifs.close();
	f_crypt_byte.close();
	f_key.close();

	ifstream inf_1(INPUT_CRYPT_BYTE), inf_2(KEY);
	
	cout << "Дешифрование: ";
	start = clock();
	for (int i = 1; i <= symbols_count; i++) {
		s[7 - ((i-1) % 8)] = ((inf_2.get() - '0') ^ (inf_1.get() - '0'));
		if (i % 8 == 0) {
			int symbol = 0;
			for (int j = 7; j >= 0; j--) {
				symbol += (int)s[j] * pow(2, j);
			}
			f_decrypt << (char)symbol;
		}
	}
	finish = clock();
	cout << (finish - start) / 1000.0 << " c" << endl;
	f_decrypt.close();
	inf_1.close();
	inf_2.close();

	system("pause");
	return 0;
}
