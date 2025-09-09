#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cctype>

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

// https://dvmn.org/encyclopedia/python_strings/ansi-codes/
class ANSICodes {
public:
    static std::string clearScreen() {
        return "\033[2J\033[H";
    }
    
    static std::string setColor(Color color) {
        return "\033[" + std::to_string(static_cast<int>(color)) + "m";
    }
    
    static std::string resetColor() {
        return "\033[0m";
    }
    
    static std::string moveCursor(int row, int col) {
        return "\033[" + std::to_string(row) + ";" + std::to_string(col) + "H";
    }
};

// Класс для загрузки и хранения шаблонов символов
class FontLoader {
private:
    static std::unordered_map<std::string, std::map<char, 
    std::vector<std::string>>> templates;
    static std::unordered_map<std::string, bool> loaded;

public:
    static void loadFont(const std::string& fontId) {
        if (loaded[fontId]) {
            return;
        }
        
        std::string filename = "text" + fontId + ".txt"; 
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            std::cerr << "loading file error" << fontId << std::endl;
            return;
        }
        
        std::string line;
        char currentChar = 0;
        std::vector<std::string> currentTemplate;
        
        while (std::getline(file, line)) {
            if (line.empty()) {
                if (!currentTemplate.empty()) {
                    templates[fontId][currentChar] = currentTemplate;
                    currentTemplate.clear();
                }
                continue;
            }
            
            if (line.length() == 1 && currentTemplate.empty()) {
                currentChar = std::toupper(line[0]);
            } else {
                currentTemplate.push_back(line);
            }
        }
        
        if (!currentTemplate.empty()) {
            templates[fontId][currentChar] = currentTemplate;
        }
        
        loaded[fontId] = true;
        file.close();
    }
    
    static const std::map<char, std::vector<std::string>>& getFont(const std::string& fontId) {
        return templates[fontId];
    }
};

// Инициализация статических членов
std::unordered_map<std::string, std::map<char, std::vector<std::string>>> 
FontLoader::templates;
std::unordered_map<std::string, bool> FontLoader::loaded;

// Основной класс Printer
class Printer {
private:
    Color color;
    std::pair<int, int> position;
    std::string fontId;
    char symbol;

public:
    // Статический метод для вывода текста
    static void printStatic(const std::string& text, Color color, 
                          const std::pair<int, int>& position, 
                          char symbol = '*', 
                          const std::string& fontId = "1") {
        FontLoader::loadFont(fontId);
        const auto& font = FontLoader::getFont(fontId);
        
        if (font.empty()) {
            std::cerr << "Шрифт не загружен или пуст" << std::endl;
            return;
        }
        
        // Определяем высоту символов
        int height = font.begin()->second.size();
        std::vector<std::string> outputLines(height, "");
        
        // Формируем выходные строки
        for (char c : text) {
            c = std::toupper(c);
            if (font.find(c) == font.end()) {
                // Если символ не найден в шрифте, пропускаем его
                continue;
            }
            
            const auto& charTemplate = font.at(c);
            for (int i = 0; i < height; i++) {
                std::string line = charTemplate[i];
                // Заменяем символы в шаблоне на указанный символ
                for (char& ch : line) {
                    if (ch != ' ') {
                        ch = symbol;
                    }
                }
                outputLines[i] += line + " ";
            }
        }
        
        // Очищаем экран и перемещаем курсор
        std::cout << ANSICodes::clearScreen();
        
        // Перемещаем курсор на нужную позицию
        for (int i = 0; i < position.first - 1; i++) {
            std::cout << std::endl;
        }
        
        // Выводим текст
        for (const auto& line : outputLines) {
            std::cout << std::string(position.second - 1, ' ');
            std::cout << ANSICodes::setColor(color) << line << ANSICodes::resetColor() << std::endl;
        }
    }
    
    // Конструктор
    Printer(Color color, const std::pair<int, int>& position, 
           char symbol = '*', const std::string& fontId = "1")
        : color(color), position(position), fontId(fontId), symbol(symbol) {
        FontLoader::loadFont(fontId);
    }
    
    // Метод для вывода текста
    void print(const std::string& text) const {
        printStatic(text, color, position, symbol, fontId);
    }
    
    // Деструктор (для восстановления состояния консоли)
    ~Printer() {
        std::cout << ANSICodes::resetColor() << std::endl;
    }
};

// Функция для преобразования строки в цвет
Color stringToColor(const std::string& colorStr) {
    static const std::unordered_map<std::string, Color> colorMap = {
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
    if (it != colorMap.end()) {
        return it->second;
    }
    return Color::WHITE; // По умолчанию белый
}

int main() {
    // Демонстрация статического метода
    std::cout << "=== Демонстрация статического метода ===" << std::endl;
    Printer::printStatic("HELLO", Color::GREEN, {5, 5}, '#', "1");

    // Ждем пользовательский ввод
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.ignore();
    
    // Демонстрация работы через экземпляр класса
    std::cout << "=== Демонстрация работы через экземпляр класса ===" << std::endl;
    
    std::string userInput;
    std::cout << "Введите слово (латинскими буквами): ";
    std::getline(std::cin, userInput);
    
    std::cout << "Выберите цвет (BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE): ";
    std::string colorInput;
    std::getline(std::cin, colorInput);
    Color color = stringToColor(colorInput);
    
    std::cout << "Выберите символ для отображения: ";
    char symbol;
    std::cin >> symbol;
    
    std::cout << "Выберите шрифт (1 - обычный, 2 - большой): ";
    std::string fontChoice;
    std::cin >> fontChoice;
    
    // Создаем экземпляр Printer и используем его
    {
        Printer printer(color, {10, 10}, symbol, fontChoice);
        printer.print(userInput);
        
        // В этой области видимости printer активен
        // При выходе из области видимости будет вызван деструктор
    }
    
    std::cout << "Состояние консоли восстановлено." << std::endl;
    
    return 0;
}