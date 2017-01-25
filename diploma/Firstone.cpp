#include <fstream> 
#include <iostream> 
#include <map> 
#include <bitset>
#include <locale.h>
#include <time.h>
#include <cstring>
#include <cmath>

using namespace std;

#pragma region Files

#define INPUT "input.txt"
#define LETTERS "input_only_letters.txt"
#define LETTERS_BYTE "letters_byte.txt"
#define SYMBOL1 "count_1_symbol.txt"
#define SYMBOL2 "count_2_symbols.txt"
#define INPUT_CRYPT_BYTE "input_crypt_byte.txt"
#define INPUT_CRYPT "input_crypt.txt"
#define INPUT_DECRYPT "input_decrypt.txt"
#define KEY "key.txt"

#pragma endregion

const int BIT_COUNT = 5;

struct cmp_str
{
	bool operator()(char const *a, char const *b) const
	{
		return strcmp(a, b) < 0;
	}
};

//ÐÑËÎÑ ðåãèñòð ñäâèãà ñ ëèíåéíîé îáðàòíîé ñâÿçüþ 
int LFSR(void)
{
	//static unsigned long S = 0x00000001;
	//S = ((((S >> 31) ^ (S >> 30) ^ (S >> 29) ^ (S >> 27) ^ (S >> 25) ^ S) & 0x00000001) << 31) | (S >> 1);
	//return S & 0x00000001;
	static unsigned long S = 0x00000001;
	S = ((((S >> 32) ^ (S >> 22) ^ (S >> 2) ^ (S >> 1)) & 0x00000001) << 31) | (S >> 1);
	return S & 0x00000001;
}

int removing_symbols() {
	ifstream f_input;
	ofstream f_letters, f_letters_byte;

	f_input.open(INPUT);
	f_letters_byte.open(LETTERS_BYTE);
	f_letters.open(LETTERS);

	char symbol;
	int symbols_count = 0;
	bitset<BIT_COUNT> symbol_byte;
	while ((symbol = tolower(f_input.get())) != -1)
		if (symbol <= 'z' && symbol >= 'a' || symbol == ' ')
		{
			f_letters << symbol;

			if (symbol == ' ')
				symbol -= 32;
			else
				symbol -= 96;

			symbol_byte = bitset<BIT_COUNT>(symbol);
			symbols_count++;
			f_letters_byte << symbol_byte;
		}
	symbols_count *= BIT_COUNT;

	f_input.close();
	f_letters.close();
	f_letters_byte.close();

	return symbols_count;
}

void counting_symbols() {
	ifstream f_letters;
	ofstream f_count1, f_count2;

	f_letters.open(LETTERS);
	f_count1.open(SYMBOL1);
	f_count2.open(SYMBOL2);

	char symbol1, symbol2;
	map<char, int> mp1;
	map<char*, int, cmp_str> mp2;

	char *sec;
	while (f_letters.get(symbol1) && f_letters.get(symbol2))
	{
		sec = new char[3];
		sec[0] = symbol1;
		sec[1] = symbol2;
		sec[2] = 0;
		f_letters.seekg(-1, ios_base::cur);
		mp1[symbol1]++;
		mp2[sec]++;
	}

	for (map<char, int>::iterator p = mp1.begin(); p != mp1.end(); ++p) // ïðîõîäèì ïî âñåì ýëåìåíòàì 
		f_count1 << p->first << ':' << p->second << endl;
	
	for (map<char*, int>::iterator p1 = mp2.begin(); p1 != mp2.end(); ++p1)
		f_count2 << p1->first << ' ' << p1->second << endl;

	f_letters.close();
	f_count1.close();
	f_count2.close();
}

void encryption(int symbols_count) {
	ifstream f_letters_byte;
	ofstream f_crypt_byte, f_key, f_crypt;
	char s[BIT_COUNT];

	f_letters_byte.open(LETTERS_BYTE);
	f_crypt.open(INPUT_CRYPT);
	f_crypt_byte.open(INPUT_CRYPT_BYTE);
	f_key.open(KEY);

	for (int i = 0; i < 32; i++)
		LFSR();

	for (int i = 1; i <= symbols_count; i++) {
		int byte = LFSR(), symbol = f_letters_byte.get();
		f_key << byte;
		f_crypt_byte << (byte ^ (symbol - '0'));
		s[BIT_COUNT - 1 - ((i - 1) % BIT_COUNT)] = (byte ^ (symbol - '0'));
		if (i % BIT_COUNT == 0) {
			for (int j = BIT_COUNT - 1; j >= 0; j--) {
				symbol += (int)s[j] * pow(2, j);
			}
			f_crypt << (char)symbol;
		}
	}

	f_letters_byte.close();
	f_crypt_byte.close();
	f_key.close();
	f_crypt.close();
}

void decryption(int symbols_count) {
	ofstream f_decrypt;
	ifstream input_crypt_byte, key;
	char s[BIT_COUNT];

	f_decrypt.open(INPUT_DECRYPT);
	input_crypt_byte.open(INPUT_CRYPT_BYTE);
	key.open(KEY);

	for (int i = 1; i <= symbols_count; i++) {
		s[BIT_COUNT - 1 - ((i - 1) % BIT_COUNT)] = ((key.get() - '0') ^ (input_crypt_byte.get() - '0'));
		if (i % BIT_COUNT == 0) {
			int symbol = 0;
			for (int j = BIT_COUNT - 1; j >= 0; j--) {
				symbol += (int)s[j] * pow(2, j);
			}

			if (symbol == 0)
				symbol += 32;
			else
				symbol += 96;
			f_decrypt << (char)symbol;
		}
	}

	f_decrypt.close();
	input_crypt_byte.close();
	key.close();
}

int main()
{
	long start = clock();
    cout << "Removing of extra characters and converting to a binary system: ";
	int symbols_count = removing_symbols();
	long finish = clock();
	cout << (finish - start) / 1000.0 << " s" << endl;


	cout << "Counting single and double symbols: ";
	start = clock();
	counting_symbols();
	finish = clock();
	cout << (finish - start) / 1000.0 << " s" << endl;
	

	cout << "Creating key and encrypting: ";
	start = clock();
	encryption(symbols_count);
	finish = clock();
	cout << (finish - start) / 1000.0 << " s" << endl;


	cout << "Decrypting: ";
	start = clock();
	decryption(symbols_count);
	finish = clock();
	cout << (finish - start) / 1000.0 << " s" << endl;

	system("pause");
	return 0;
}
