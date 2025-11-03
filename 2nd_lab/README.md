
## Class ANSICodes
https://dvmn.org/encyclopedia/python_strings/ansi-codes/

Это класс, который предоставляет статические методы для работы с ANSI escape sequences (управляющими последовательностями ANSI).

Что такое ANSI escape sequences?
Это специальные последовательности символов, которые используются для управления терминалом/консолью. Они начинаются с escape-символа \033 (или \x1b) и позволяют:

Менять цвет текста

Перемещать курсор

Очищать экран

И многое другое


## class fontLoader
Класс эффективно управляет загрузкой и доступом к различным шрифтам, используя статические члены для хранения состояния между вызовами.

1. Статические члены класса:
```
static std::unordered_map<std::string, std::map<char, std::vector<std::string>>> templates;
static std::unordered_map<std::string, bool> loaded;
```
templates - хранит все загруженные шрифты:

Ключ: string - ID шрифта (например: "1", "2")

Значение: map<char, vector<string>> - mapping символов на их шаблоны

loaded - отслеживает какие шрифты уже загружены:

Ключ: string - ID шрифта

Значение: bool - true если загружен

2. Открытие файла:

```
std::string filename = "text" + fontId + ".txt";  // "text1.txt"
std::ifstream file(filename);
```

3. Процесс чтения и парсинга:
Переменные для парсинга:

currentChar - текущий обрабатываемый символ

currentTemplate - накапливаемые строки шаблона

Логика чтения:
```
while (std::getline(file, line)) {
    if (line.empty()) {  // Пустая строка = конец шаблона
        if (!currentTemplate.empty()) {
            templates[fontId][currentChar] = currentTemplate;
            currentTemplate.clear();
        }
        continue;
    }
    
    if (line.length() == 1 && currentTemplate.empty()) {
        currentChar = std::toupper(line[0]);  // Новый символ
    } else {
        currentTemplate.push_back(line);  // Добавляем строку шаблона
    }
}
```
4.Финальное сохранение 

```
 if (!currentTemplate.empty()) {
    templates[fontId][currentChar] = currentTemplate;  // Последний шаблон
}
loaded[fontId] = true;  // Помечаем как загруженный
```

Метод getFont()

5. Получение шрифта:
```cpp
static const std::map<char, std::vector<std::string>>& getFont(const std::string& fontId) {
    return templates[fontId];  // Возвращаем ссылку на map символов
}
```
## class Printer
**Назначение класса**

Класс Printer 
предназначен для красивого вывода текста в консоль с использованием:

*Псевдографических шрифтов

*Произвольных цветов

*Заданной позиции на экране

*Пользовательских символов для отрисовки

1. Поля класса
```
Color color;                    // Цвет текста
std::pair<int, int> position;   // Позиция (строка, столбец)
std::string fontId;             // ID шрифта ("1", "2")
char symbol;                    // Символ для отрисовки ('*', '#', etc.)
```
2. Статический метод printStatic()
Назначение: Позволяет выводить текст без создания объекта

Параметры:

    *  text - текст для вывода

    *  color - цвет текста

    * position - позиция на экране

    * symbol - символ для отрисовки (по умолчанию '*')

    * fontId - ID шрифта (по умолчанию "1")



```cpp
// Загрузка шрифта
FontLoader::loadFont(fontId);
const auto& font = FontLoader::getFont(fontId);

// Определение высоты шрифта (по первому символу)
int height = font.begin()->second.size();

// Подготовка выходных строк
std::vector<std::string> outputLines(height, "");

// Обработка каждого символа текста
for (char c : text) {
    c = std::toupper(c);  // Приводим к верхнему регистру
    
    if (font.find(c) == font.end()) continue;  // Пропускаем неизвестные символы
    
    const auto& charTemplate = font.at(c);
    
    // Обрабатываем каждую строку шаблона
    for (int i = 0; i < height; i++) {
        std::string line = charTemplate[i];
        
        // Замена символов в шаблоне на пользовательский символ
        for (char& ch : line) {
            if (ch != ' ') ch = symbol;  // Заменяем все не-пробелы
        }
        
        outputLines[i] += line + " ";  // Добавляем пробел между символами
    }
}

```
Получается, загрузили шрифт, потом получили ссылку на шрифт.
Создали вектор пустых строк длинной строк одной буквы, шрифт 1 - 5, шрифт 2 - 6.
далее идет цикл где перебирается каждый символ текста, затем второй цикл, где мы проходим по каждой строке шаблона и в условии если символ не равен пробелу заменяем его на пользовательский.

3. Вывод на экран
```cpp
    // Очистка экрана
std::cout << ANSICodes::clearScreen();

// Перемещение на нужную строку
for (int i = 0; i < position.first - 1; i++) {
    std::cout << std::endl;
}

// Вывод каждой строки с отступами и цветом
for (const auto& line : outputLines) {
    std::cout << std::string(position.second - 1, ' ');  // Отступ слева
    std::cout << ANSICodes::setColor(color) << line << ANSICodes::resetColor() << std::endl;
}
```
Очищаем полностью экран консоли, затем перемещаем курсор на  нужную строку  соответствующую высоте шрифта
