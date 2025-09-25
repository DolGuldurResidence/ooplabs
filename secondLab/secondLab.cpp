#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <limits>

using namespace std;

enum class Color {
	BLACK = 30,
	RED = 31,
	GREEN = 32,
	YELLOW = 33,
	BLUE = 34,
	MAGENTA = 35,
	CYAN = 36,
	WHITE = 37,
	RESET = 0
};

Color stringToColor(string colorStr) {
	for (char& ch : colorStr) {
		ch = static_cast<char>(toupper(static_cast<unsigned char>(ch))); //safety type conversion |f.e. int 1 -> ch "1"
	}
	static const unordered_map<string, Color> colorMap = { //
		{"BLACK", Color::BLACK},
		{"RED", Color::RED},
		{"GREEN", Color::GREEN},
		{"YELLOW", Color::YELLOW},
		{"BLUE", Color::BLUE},
		{"MAGENTA", Color::MAGENTA},
		{"CYAN", Color::CYAN},
		{"WHITE", Color::WHITE}
	};
	auto it = colorMap.find(colorStr); 
	return it == colorMap.end() ? Color::WHITE : it->second; //if iter of map -> to out of range: def color white, else iter -> value   
}

//https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
class ANSICodes {
public:
	static string clearScreen() { 
        return "\033[2J\033[H";
    }
	static string setColor(Color color) {
        return "\033[" + to_string(static_cast<int>(color)) + "m"; 
    }
	static string resetColor() {
        return "\033[0m";
    }
	static string moveCursor(int row, int col) {
        return "\033[" + to_string(row) + ";" + to_string(col) + "H";
    }
};

//load templates symbols from text{fontId}.txt
class FontLoader {
private:
	static unordered_map<string, map<char, vector<string>>> templatesByFont; //un_map(str-map(ch-list(str)))
	static unordered_map<string, bool> loaded;

public:
	static void loadFont(const string& fontId) { //static method can be used without creating object:) 
		if (loaded.count(fontId)) { // if loaded unempty leaving method 
			return;
		}

		string filename = "text" + fontId + ".txt";
		ifstream file(filename);
		if (!file.is_open()) {
			cerr << "Ошибка загрузки файла шрифта: " << filename << endl;
			templatesByFont[fontId] = {}; // empty map for this ID 
			loaded[fontId] = true; // mark as done
			return;
		}

		string line; 
		char currentChar = 0;
		vector<string> currentTemplate;

		while (getline(file, line)) {
			//delete\r 
			if (!line.empty() && line.back() == '\r') {
				line.pop_back();
			}

			if (line.empty()) {  // making massive, if line empty
				if (!currentTemplate.empty() && currentChar != 0) { //if cT not empty and cC is valid 
					templatesByFont[fontId][currentChar] = currentTemplate; //fill tBF 
					currentTemplate.clear();
				}
				continue;
			}

			if (currentTemplate.empty() && line.size() == 1) { //if "Ch" in line  
				currentChar = static_cast<char>(toupper(static_cast<unsigned char>(line[0]))); //set ch for map as key
			} else {
				currentTemplate.push_back(line);
			}
		}

		if (!currentTemplate.empty() && currentChar != 0) {
			templatesByFont[fontId][currentChar] = currentTemplate; //packing done template to map
		}

		loaded[fontId] = true;
		file.close();
	}

	static const map<char, vector<string>>& getFont(const string& fontId) {   //return tBF by font id   
		return templatesByFont[fontId];
	}
};

unordered_map<string, map<char, vector<string>>> FontLoader::templatesByFont; // allocate memory for ts vars out of class 119-120
unordered_map<string, bool> FontLoader::loaded;

class Printer {
private:
	Color color;
	pair<int, int> position; // {row, col}, 1-based
	string fontId;
	char symbol;

public:
	// static output
	static void printStatic(const string& text,
							Color color,
							const pair<int, int>& position,
							char symbol = '*',
							const string& fontId = "1") {
		FontLoader::loadFont(fontId);
		const auto& font = FontLoader::getFont(fontId);
		if (font.empty()) {
			cerr << "Шрифт не загружен или пуст: " << fontId << endl;
			return;
		}

        // normalize height by max highest tamplate
        int height = 0;
        for (const auto& kv : font) {
            height = max(height, static_cast<int>(kv.second.size())); //choose max between string in vec of templ and pervious height
        }
        vector<string> outputLines(height, "");

        // Собираем строки вывода с нормализацией ширины/высоты
        auto computeGlyphWidth = [](const vector<string>& tmpl) -> int {
            int w = 0;
            for (const string& row : tmpl) {
                w = max(w, static_cast<int>(row.size()));
            }
            return w;
        };

        // Ширина по умолчанию для неизвестных символов — по первому шаблону
        int defaultWidth = 0;
        if (!font.empty()) {
            defaultWidth = computeGlyphWidth(font.begin()->second);
        }

        for (char raw : text) {
			char c = static_cast<char>(toupper(static_cast<unsigned char>(raw)));
			auto it = font.find(c);
			if (it == font.end()) {
                int width = defaultWidth;
                for (int i = 0; i < height; i++) {
                    outputLines[i] += string(max(0, width), ' ') + " ";
                }
				continue;
			}
            const auto& tmpl = it->second;
            const int glyphHeight = static_cast<int>(tmpl.size());
            const int glyphWidth = computeGlyphWidth(tmpl);
            for (int i = 0; i < height; i++) {
                string line = (i < glyphHeight) ? tmpl[i] : string(glyphWidth, ' ');
                if (static_cast<int>(line.size()) < glyphWidth) {
                    line.append(glyphWidth - static_cast<int>(line.size()), ' ');
                }
                for (char& ch : line) {
                    if (ch != ' ') ch = symbol;
                }
                outputLines[i] += line + " ";
            }
		}

		// Очистка экрана и вертикальное смещение
		cout << ANSICodes::clearScreen();
		for (int i = 0; i < max(0, position.first - 1); i++) {
			cout << '\n';
		}

		// Вывод с цветом и горизонтальным смещением
		for (const auto& line : outputLines) {
			cout << string(max(0, position.second - 1), ' ');
			cout << ANSICodes::setColor(color) << line << ANSICodes::resetColor() << '\n';
		}
	}

	// Экземпляр с фиксированным стилем
	Printer(Color color, const pair<int, int>& position, char symbol = '*', const string& fontId = "1")
		: color(color), position(position), fontId(fontId), symbol(symbol) {
		FontLoader::loadFont(fontId);
	}

	void print(const string& text) const {
		printStatic(text, color, position, symbol, fontId);
	}

	~Printer() {
		// Восстановление состояния консоли
		cout << ANSICodes::resetColor();
	}
};



int main() {
	// Демонстрация статического метода
	cout << "=== Демонстрация статического метода ===\n";
	Printer::printStatic("HELLO", Color::GREEN, {5, 5}, '#', "1");

	cout << "Нажмите Enter для продолжения...";
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	// Демонстрация через экземпляр
	cout << "=== Демонстрация экземпляра ===\n";
	cout << "Введите слово (латиницей): ";
	string userInput;
	getline(cin, userInput);

	cout << "Выберите цвет (BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE): ";
	string colorInput;
	getline(cin, colorInput);
	Color color = stringToColor(colorInput);

	cout << "Выберите символ для отображения: ";
	char symbol = '*';
	{
		string s;
		getline(cin, s);
		if (!s.empty()) symbol = s[0];
	}

	cout << "Выберите шрифт (1 — обычный, 2 — большой): ";
	string fontChoice;
	{
		string s;
		getline(cin, s);
		fontChoice = (s == "2") ? "2" : "1";
	}

	{
		Printer printer(color, {10, 10}, symbol, fontChoice);
		printer.print(userInput);
	}

	cout << "\nСостояние консоли восстановлено.\n";
	return 0;
}