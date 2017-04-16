#include <fstream> 
#include <iostream> 
#include <map> 
#include <bitset>
#include <locale.h>
#include <time.h>
#include <cstring>
#include <cmath>
#include <cstdio>

using namespace std;

#pragma region Files

static string INPUT = "input.txt";
static string INPUT_NEW = "input_new.txt";
static string INPUT_BYTE = "input_byte.txt";
static string SYMBOL1 = "count_1_symbol.txt";
static string SYMBOL2 = "count_2_symbols.txt";
static string CRYPT_BYTE = "input_crypt_byte.txt";
static string CRYPT = "input_crypt.txt";
static string KEY = "key.txt";
static string INITKEY = "key_init.txt";

#pragma endregion

const int BIT_COUNT = 5;
static unsigned long key_init;
static unsigned long symbols_count;

struct cmp_str
{
	bool operator()(char const *a, char const *b) const
	{
		return strcmp(a, b) < 0;
	}
};

//РСЛОС регистр сдвига с линейной обратной связью 
int LFSR()
{
	static unsigned long S = key_init;
	S = ((((S >> 31) ^ (S >> 30) ^ (S >> 29) ^ (S >> 27) ^ (S >> 25) ^ S) & 0x00000001) << 31) | (S >> 1);
	//S = ((((S >> 32) ^ (S >> 22) ^ (S >> 2) ^ (S >> 1)) & 0x00000001) << 31) | (S >> 1);
	return S & 0x00000001;
}

char transform_symbol(int symbol, bool direction = true)
{
	if (direction) {
		if (symbol == ' ')
			symbol -= 32;  //0
		else
			if (symbol >= 'a' && symbol <= 'z')
				symbol -= 96; //1-26
			else
				if (symbol == '.')
					symbol -= 19; //27
				else
					if (symbol == ',')
						symbol -= 16; //28
					else
						if (symbol == '!')
							symbol -= 4; //29
						else
							if (symbol == '?')
								symbol -= 33; //30
							else
								if (symbol == '"')
									symbol -= 3; //31
								else
									symbol = -1;
	}
	else {
		if (symbol == 0)
			symbol += 32;
		else
			if (symbol >= 1 && symbol <= 26)
				symbol += 96;
			else
				if (symbol == 27)
					symbol += 19;
				else
					if (symbol == 28)
						symbol += 16;
					else
						if (symbol == 29)
							symbol += 4;
						else
							if (symbol == 30)
								symbol += 33;
							else
								if (symbol == 31)
									symbol += 3;
								else
									symbol = -1;
	}

	return symbol;
}

void removing_symbols() {
	ifstream f_input;
	ofstream f_input_byte, f_input_new;

	f_input.open(INPUT);
	f_input_new.open(INPUT_NEW);
	f_input_byte.open(INPUT_BYTE);

	char symbol, new_symbol;
	symbols_count = 0;
	bitset<BIT_COUNT> symbol_byte;
	while ((symbol = tolower(f_input.get())) != -1) {
		new_symbol = transform_symbol(symbol);
		if (new_symbol != -1)
		{
			symbol_byte = bitset<BIT_COUNT>(new_symbol);
			symbols_count++;
			f_input_byte << symbol_byte;
			f_input_new << symbol;
		}
	}
	symbols_count *= BIT_COUNT;

	f_input.close();
	f_input_new.close();
	f_input_byte.close();

	remove(INPUT.c_str());
	rename(INPUT_NEW.c_str(), INPUT.c_str());
}

void counting_symbols() {
	ifstream f_input;
	ofstream f_count1, f_count2;

	f_input.open(INPUT);
	f_count1.open(SYMBOL1);
	f_count2.open(SYMBOL2);

	char symbol1, symbol2;
	map<char, int> mp1;
	map<char*, int, cmp_str> mp2;

	char *sec;
	while (f_input.get(symbol1) && f_input.get(symbol2))
	{
		sec = new char[3];
		sec[0] = symbol1;
		sec[1] = symbol2;
		sec[2] = 0;
		f_input.seekg(-1, ios_base::cur);
		mp1[symbol1]++;
		mp2[sec]++;
	}

	for (map<char, int>::iterator p = mp1.begin(); p != mp1.end(); ++p) // проходим по всем элементам 
		f_count1 << p->first << ':' << p->second << endl;

	for (map<char*, int>::iterator p = mp2.begin(); p != mp2.end(); ++p)
		f_count2 << p->first << ' ' << p->second << endl;

	f_input.close();
	f_count1.close();
	f_count2.close();
}

void init_key() {
	ifstream f_key;

	f_key.open(INITKEY);
	string key;
	getline(f_key, key);

	key_init = atoi(key.c_str());

	f_key.close();
}

void create_key() {
	ofstream f_key;

	init_key();

	f_key.open(KEY);

	for (int i = 0; i < 32; i++)
		LFSR();

	for (int i = 1; i <= symbols_count; i++)
		f_key << LFSR();

	f_key.close();
}

void encryption() {
	ifstream f_letters_byte, f_key;
	ofstream f_crypt_byte, f_crypt;
	char s[BIT_COUNT];

	f_letters_byte.open(INPUT_BYTE);
	f_crypt.open(CRYPT);
	f_crypt_byte.open(CRYPT_BYTE);
	f_key.open(KEY);

	for (int i = 0; i < 32; i++)
		LFSR();

	for (int i = 1; i <= symbols_count; i++) {
		int byte = f_key.get() - '0', symbol = f_letters_byte.get(), crypt_symbol = 0;
		f_crypt_byte << (byte ^ (symbol - '0'));
		s[BIT_COUNT - 1 - ((i - 1) % BIT_COUNT)] = (byte ^ (symbol - '0'));
		if (i % BIT_COUNT == 0) {
			for (int j = BIT_COUNT - 1; j >= 0; j--) {
				crypt_symbol += (int)s[j] * pow(2, j);
			}
			f_crypt << transform_symbol(crypt_symbol, false);
		}
	}

	f_letters_byte.close();
	f_crypt_byte.close();
	f_key.close();
	f_crypt.close();
}

map<char*, int, cmp_str> load_bigramm() {
	ifstream f_count2;
	char symbol1, symbol2;
	map<char*, int, cmp_str> result;

	f_count2.open(SYMBOL2);
	char *sec;
	char *count;

	while (f_count2.get(symbol1) && f_count2.get(symbol2))
	{
		sec = new char[3];
		count = new char[10];
		sec[0] = symbol1;
		sec[1] = symbol2;
		sec[2] = 0;

		f_count2.get();
		f_count2.getline(count, 10);
		result[sec] = atoi(count);
	}

	return result;
}

double ratio_calc() {
	ifstream f_input;

	f_input.open("input2.txt");

	int symbols_count = 0;
	double p0 = 1.0 / 1024;
	double r = 0;
	char symbol1, symbol2;
	char *sec;

	map<char*, int, cmp_str> bg = load_bigramm();
	for (map<char*, int, cmp_str>::iterator it = bg.begin(); it != bg.end(); ++it)
	{
		symbols_count += it->second;
	}

	symbols_count++;

	int count = 0;

	while (f_input.get(symbol1) && f_input.get(symbol2))
	{
		sec = new char[3];
		sec[0] = symbol1;
		sec[1] = symbol2;
		sec[2] = 0;

		f_input.seekg(-1, ios_base::cur);
		if (bg.find(sec) == bg.end())
			r += log(1e-308);
		else
			r += log(bg.find(sec)->second / (symbols_count / BIT_COUNT - 1.0));

		count++;
	}

	return r - count * log(p0);
}

void find_limits() {
	const int MAX_SYMBOLS = 100000;
	const int TEST_COUNT = 10;
	const int MIN_SYMBOLS = 2;
	string LIMITS_OUT = "limits.txt";
	string LIMITS_IN = "input_crypt.txt";
	ofstream f_input2, f_output;
	ifstream f_input;

	f_output.open(LIMITS_OUT);
	f_input.open(LIMITS_IN);

	int count = MIN_SYMBOLS;
	char symbol;

	f_input.seekg(0, f_input.end);
	int symbols_count = f_input.tellg();

	while (count < MAX_SYMBOLS) {
		double sum = 0, sum2 = 0;
		for (int j = 0; j < TEST_COUNT; j++) {
			f_input2.open("input2.txt");
			f_input.seekg(rand() % (symbols_count - count - 1), f_input.beg);
			for (int i = 0; i < count; i++) {
				f_input.get(symbol);
				f_input2 << symbol;
			}
			f_input2.close();
			double ratio = ratio_calc();
			sum += ratio;
			sum2 += pow(ratio, 2);
			cout << ratio << " ";
		}
		cout << endl;
		f_output << count << " - " << sum / TEST_COUNT << ", d^2 = " << pow(sum2 / TEST_COUNT - pow(sum / TEST_COUNT, 2),1/2) << endl;
		count = count * 1.2 + 1;
	}

	//remove("input2.txt");
	f_input.close();
	f_output.close();
}

int main(int argc, char *argv[])
{
	if (argc > 1) {
		INITKEY = argv[1];
		if (argc > 2) {
			INPUT = argv[2];
			if (argc > 3)
				CRYPT = argv[3];
		}
	}

	/*long start = clock();
	cout << "Removing of extra characters and converting to a binary system: ";
	removing_symbols();
	long finish = clock();
	cout << (finish - start) / 1000.0 << " s" << endl;


	cout << "Counting single and double symbols: ";
	start = clock();
	counting_symbols();
	finish = clock();
	cout << (finish - start) / 1000.0 << " s" << endl;


	cout << "Creating key: ";
	start = clock();
	create_key();
	finish = clock();
	cout << (finish - start) / 1000.0 << " s" << endl;


	cout << "Encrypting: ";
	start = clock();
	encryption();
	finish = clock();
	cout << (finish - start) / 1000.0 << " s" << endl;
	*/
	srand(time(NULL));
	find_limits();

	system("pause");
	return 0;
}
