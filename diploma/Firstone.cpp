#include <fstream> 
#include <iostream> 
#include <map> 
#include <bitset>
#include <time.h>

using namespace std;

#pragma region Files

static string INPUT = "input.txt";
static string INPUT_NEW = "input_new.txt";
static string INPUT_BYTE = "input_byte.txt";
static string SYMBOL1 = "count_1_symbol.txt";
static string SYMBOL2 = "count_2_symbols.txt";
static string CRYPT = "input_crypt.txt";
static string KEY = "key.txt";
static string INITKEY = "key_init.txt";

#pragma endregion

const int BIT_COUNT = 5;
static unsigned long key_init;

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
		if (symbol >= 'a' && symbol <= 'z')
			symbol -= 96; //1-26
		else
			switch (symbol)
			{
			case (' '):
				symbol -= 32; //0
				break;
			case('.'):
				symbol -= 19; //27
				break;
			case(','):
				symbol -= 16; //28
				break;
			case('!'):
				symbol -= 4; //29
				break;
			case('?'):
				symbol -= 33; //30
				break;
			case('"'):
				symbol -= 3; //31
				break;
			default:
				symbol = -1;
			}
	}
	else {
		if (symbol >= 1 && symbol <= 26)
			symbol += 96;
		else
			switch (symbol)
			{
			case (0):
				symbol += 32; // 
				break;
			case(27):
				symbol += 19; //.
				break;
			case(28):
				symbol += 16; //,
				break;
			case(29):
				symbol += 4; //!
				break;
			case(30):
				symbol += 33; //?
				break;
			case(31):
				symbol += 3; //"
				break;
			default:
				symbol = -1;
			}
	}

	return symbol;
}

void removing_symbols() {
	ifstream f_input;
	ofstream f_input_new;

	f_input.open(INPUT);
	f_input_new.open(INPUT_NEW);

	char symbol, new_symbol;
	
	while ((symbol = tolower(f_input.get())) != -1) {
		new_symbol = transform_symbol(symbol);
		if (new_symbol != -1)
			f_input_new << symbol;
	}
	f_input.close();
	f_input_new.close();

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
		sec = new char[3]{ symbol1, symbol2, 0 };
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

int file_symbol_count(string path) {
	ifstream f_input;
	f_input.open(path);

	f_input.seekg(0, f_input.end);
	int symbols_count = f_input.tellg();

	f_input.close();

	return symbols_count;
}

void init_key() {
	ifstream f_key;

	f_key.open(INITKEY);
	string key;
	getline(f_key, key);

	key_init = atoi(key.c_str());

	f_key.close();
}

void encryption() {
	ifstream f_input;
	ofstream f_crypt;
	int symbols_count = file_symbol_count(INPUT), crypt_symbol, symbol, new_symbol;
	bitset<BIT_COUNT> symbol_byte;

	f_input.open(INPUT);
	f_crypt.open(CRYPT);

	init_key();

	for (int i = 0; i < 32; i++)
		LFSR();

	for (int i = 1; i <= symbols_count; i++) {
		symbol = f_input.get();
		new_symbol = transform_symbol(symbol);
		symbol_byte = bitset<BIT_COUNT>(new_symbol);
		crypt_symbol = 0;
		for (int j = BIT_COUNT - 1; j >= 0; j--)
		{
			int byte = LFSR();
			symbol = symbol_byte[j];
			crypt_symbol += (byte ^ symbol) * pow(2, j);
		}
		f_crypt << transform_symbol(crypt_symbol, false);
	}

	f_input.close();
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

double ratio_calc(string path) {
	ifstream f_input;

	int symbols_count = file_symbol_count(INPUT), count = file_symbol_count(path) - 1;
	double p0 = 1.0 / 1024;
	double r = 0;
	char symbol1, symbol2;
	char *sec;

	map<char*, int, cmp_str> bg = load_bigramm();

	f_input.open(path);
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
	}

	f_input.close();

	return r - count * log(p0);
}

double* find_limits(int c, string path) {
	const int TEST_COUNT = 10;
	string TEMP_TEXT = "input2.txt";
	ofstream f_input2;
	ifstream f_input;

	f_input.open(path);

	int count = c;
	char symbol;

	int symbols_count = file_symbol_count(path);

	double sum = 0, sum2 = 0;
	for (int j = 0; j < TEST_COUNT; j++) {
		f_input2.open(TEMP_TEXT);
		f_input.seekg(rand() % (symbols_count - count - 1), f_input.beg);
		for (int i = 0; i < count; i++) {
			f_input.get(symbol);
			f_input2 << symbol;
		}
		f_input2.close();
		double ratio = ratio_calc(TEMP_TEXT);
		sum += ratio / TEST_COUNT;
		sum2 += pow(ratio, 2) / TEST_COUNT;
	}

	f_input.close();

	return new double[2]{ sum, pow(sum2 - pow(sum, 2), 0.5) };
}

bool text_det(string text_path) {
	double ratio;
	string TEMP_TEXT = "input3.txt";
	int symbols = file_symbol_count(text_path);
	int count = pow(symbols, 0.1) + 1, old_count = 0;
	char symbol;
	ifstream f_input;
	ofstream f_output;
	f_input.open(text_path);
	f_output.open(TEMP_TEXT, ios_base::trunc);
	f_output.close();

	double* info = new double[2];
	double *info_crypt = new double[2];
	
	while (count < symbols) {
		f_output.open(TEMP_TEXT, ios_base::app);
		for (int i = 0; i < count - old_count; i++) {
			f_input.get(symbol);
			f_output << symbol;
		}
		old_count = count;
		f_output.close();
		ratio = ratio_calc(TEMP_TEXT);
		info = find_limits(count, INPUT);
		info_crypt = find_limits(count, CRYPT);
		if (ratio > info[0] - 3 * info[1] && ratio < info_crypt[0] + 3 * info_crypt[1])
			count *= count;
		else if (ratio > info[0] - 3 * info[1])
			return true;
		else if (ratio < info_crypt[0] + 3 * info_crypt[1])
			return false;
		else
			count *= count;
	}

	return true;
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

	long start = clock();
	cout << "Removing of extra characters and converting to a binary system: ";
	removing_symbols();
	long finish = clock();
	cout << (finish - start) / 1000.0 << " s" << endl;


	cout << "Counting single and double symbols: ";
	start = clock();
	counting_symbols();
	finish = clock();
	cout << (finish - start) / 1000.0 << " s" << endl;


	cout << "Encrypting: ";
	start = clock();
	encryption();
	finish = clock();
	cout << (finish - start) / 1000.0 << " s" << endl;

	srand(time(NULL));
	if (text_det("test.txt"))
		cout << "OPEN" << endl;
	else
		cout << "CLOSE" << endl;

	system("pause");
	return 0;
}
