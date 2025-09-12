
## Постановка задачи
Лабораторная работа 3 (Система логирования)

Создать систему логирования, применая композицию (аггрегацию),
с возможностью фильтрации и различных способов вывода информации.
Использовать либо протоколы, либо интерфейсы взависимости от используемого языка программирования.

1. Создать протокол/интерфейс фильров ILogFilter / LogFilterProtocol:
  - match(self, text: str)

2. Создать неколько классов реализующих данный протокол/интерфейс
 - SimpleLogFilter - для фильтрации по вхождению паттерна, задаеваемого текстом, в текст сообщения
 - ReLogFilter - для фильтрации по вхождению паттерна, задаваемого регулярным выражением, в текст сообщения

3. Создать протокол/интерфейс обработчиков ILogHandler / LogHandlerProtocol:
 - handle(self, text: str)

4. Создать неколько классов реализующих данный протокол/интерфейс
 - FileHandler - для записи логов в файл
 - SocketHandler - для отправки логов через сокет
 - ConsoleHandler - для вывода логово в консоль
 - SyslogHandler - для записи логов в системные логи

5. Реализовать класс Logger, который принимает список ILogFilter / LogFilterProtocol и список ILogHandler / LogHandlerProtocol и релизает:
 - log(self, text: str) - которая прогоняет логи через фильтры и отдает обработчикам

6. Продемностировать работы спроектированной системы классов


## class LogFilterProtocol
```cpp
class LogFilterProtocol {
public:
    virtual ~LogFilterProtocol() = default;
    virtual bool match(const std::string& text) = 0; 
};

```

Этот класс определяет контракт, который должны соблюдать все фильтры в системе, обеспечивая единообразие и возможность легкого расширения функциональности.




## class SimpleLogFilter
```cpp
class LogFilterProtocol {
public:
    virtual ~LogFilterProtocol() = default;
    virtual bool match(const std::string& text) = 0; 
};

// Реализации фильтров
class SimpleLogFilter : public LogFilterProtocol {
private:
    std::string pattern;
    
public:
    explicit SimpleLogFilter(const std::string& pattern) : pattern(pattern) {
        if (pattern.empty()) {
            throw std::invalid_argument("Pattern cannot be empty");
        }
        // Приводим к нижнему регистру для case-insensitive поиска
        std::transform(this->pattern.begin(), this->pattern.end(), this->pattern.begin(),
                      [](unsigned char c) { return std::tolower(c); });
    }
    
    bool match(const std::string& text) override {
        if (text.empty()) return false;
        
        std::string lower_text = text;
        std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return lower_text.find(pattern) != std::string::npos;
    }
};
```

Итак, класс SimpleLogFilter - Это класс для простого текстового фильтра логов. 

Имеет следующую структуру: Сам класс является наследником класса LogFilterProtocol. 
В полях класса содержится единственная строка pattern. 

**Конструктор:** *explicit* - предотвращает неявные преобразования типов при вызове конструктора. 

содержит в себе проверку не пустой ли pattern, в противном случае выводится ошибка  

далее, если  pattern не пустой паттерн функция transform при помощи лямбда функции:  *[](unsigned char c) { return std::tolower(c); }* перезаписывает паттерн в нижнем регистре. 

**Переопределенный метод match(override):** начинается с условия, если текст не пустой, то приводит текст к нижнему регистру, затем происходит поиск паттерна в тексте:  возвращает либо найденую позицию первого вхождения pattern в text либо *npos* - специальная констаннта означающая not found.  


## class ReLogFilter

```cpp
class ReLogFilter : public LogFilterProtocol {
private:
    std::regex regex_pattern;
    bool valid;
    
public:
    explicit ReLogFilter(const std::string& regex_pattern) : valid(true) {
        if (regex_pattern.empty()) {
            throw std::invalid_argument("Regex pattern cannot be empty");
        }
        try {
            this->regex_pattern = std::regex(regex_pattern);
        } catch (const std::regex_error& e) {
            std::cerr << "Ошибка компиляции регулярного выражения '" 
                      << regex_pattern << "': " << e.what() << std::endl;
            valid = false;
        }
    }
    
    bool match(const std::string& text) override {
        if (!valid || text.empty()) return false;
        return std::regex_search(text, regex_pattern);
    }
    
    bool isValid() const { return valid; }
};
```

Фильтр для регулярных выражений. Он ищет в тексте совпадения по заданному regex-паттерну.

Структура: наследник класса *LogFilterProtocol* содержит два поля: *std::regex regex_pattern* - *std::regex* - это класс в C++, который представляет скомпилированное регулярное выражение. Это "шаблон для поиска" в тексте, который был подготовлен для быстрого использования. *bool valid* - Флаг валидности regex.
Конструктор - компилирует regex, метод *bool match* - Проверяет соответствует ли текст regex-паттерну, метод *bool isValid* - Проверяет валиден ли regex.

**Конструктор** valid инициализируется значением true ДО входа в тело конструктора проверка 
1. Прверка не пустого регулярного выражения.
2. Попытка компиляции regex.

**метод bool match**
1. Проверка валидности regex и пустоты текста
2. Поиск совпадения с regex

**isValid** возвращает просто true




 


## class LogHandlerProtocol

Это абстрактный базовый класс (интерфейс), который определяет "контракт" для всех обработчиков логов - куда и как записывать логи.

**Метод 1: virtual ~LogHandlerProtocol() = default;**
Виртуальный деструктор - обеспечивает правильное удаление объектов производных классов.

**Метод 2: virtual void handle(const std::string& text) = 0;**
Чисто виртуальный метод - определяет основную функцию обработчика: обработка текстового сообщения.

**Метод 3: virtual void flush() {}**
Виртуальный метод с реализацией по умолчанию - принудительно сбрасывает буферы.


## class FileHandler
```cpp
class FileHandler : public LogHandlerProtocol {
private:
    std::string filename;
    std::ofstream file;
    bool file_opened;
    
public:
    explicit FileHandler(const std::string& filename) : filename(filename), file_opened(false) {
        if (filename.empty()) {
            throw std::invalid_argument("Filename cannot be empty");
        }
        openFile();
    }
    
    ~FileHandler() {
        if (file.is_open()) {
            file.close();
        }
    }
    
    void openFile() {
        file.open(filename, std::ios_base::app);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        file_opened = true;
    }
    
    void handle(const std::string& text) override {
        if (!file_opened) {
            try {
                openFile();
            } catch (const std::exception& e) {
                std::cerr << "Ошибка открытия файла: " << e.what() << std::endl;
                return;
            }
        }
        
        if (file.is_open()) {
            file << text << std::endl;
            if (file.fail()) {
                std::cerr << "Ошибка записи в файл: " << filename << std::endl;
                file_opened = false;
            }
        }
    }
    
    void flush() override {
        if (file.is_open()) {
            file.flush();
        }
    }
};
```

**FileHandler** - это обработчик логов, который записывает сообщения в файл. Наследует от *LogHandlerProtocol* и реализует интерфейс работы с файлами.

Структура: 

  имеет три поля :

  1. строка *filename* - имя файла для записи, 
 
  2. *ofstream file* - файловый  поток  для работы с файлом 

  3. булевая переменная *file_opened* - флаг успешного открытия файла

**Конструктор** сохраняем имя файла, Инициализируем *file_opened = false* (файл еще не открыт),  проверяем, что имя файла не пустое. пытаемся открыть файл.

**метод openFile** открывает файл для добавления в конец, затем идет условие если файл не открыт выбрасывает ошибку, в противном случае меняет флаг на *true*

**Метод handle** запись в файл.

    1. Проверяем открыт ли файл, попытка открыть файл заново, 
    в противном случае  выводим ошибку.
    2. Записываем в файл. условие: если файл открыт 
    записываем в него текст, проверка успешности записи , 
    если *ofstream.fail() == true* выводим ошибку и меняем флаг
    *file_opened* на *false*

**метод flush** сброс буфера, flush() гарантирует немедленную запись на диск

**Деструктор: закрытие файла** file.close() - закрывает файл.


## class socketHandler

```cpp
class SocketHandler : public LogHandlerProtocol {
private:
    std::string host;
    int port;
    bool winsock_initialized;
    
    void initializeWinsock() {
#ifdef _WIN32
        if (!winsock_initialized) {
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                throw std::runtime_error("WSAStartup failed");
            }
            winsock_initialized = true;
        }
#endif
    }
    
    void cleanupWinsock() {
#ifdef _WIN32
        if (winsock_initialized) {
            WSACleanup();
            winsock_initialized = false;
        }
#endif
    }
    
public:
    SocketHandler(const std::string& host, int port) 
        : host(host), port(port), winsock_initialized(false) {
        if (host.empty()) {
            throw std::invalid_argument("Host cannot be empty");
        }
        if (port <= 0 || port > 65535) {
            throw std::invalid_argument("Port must be between 1 and 65535");
        }
    }
    
    ~SocketHandler() {
        cleanupWinsock();
    }
    
    void handle(const std::string& text) override {
        try {
            initializeWinsock();
        } catch (const std::exception& e) {
            std::cerr << "Socket error: " << e.what() << std::endl;
            return;
        }

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            std::cerr << "Socket error: cannot create socket" << std::endl;
            return;
        }
        
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        
        if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
            std::cerr << "Socket error: invalid address" << std::endl;
#ifdef _WIN32
            closesocket(sock);
#else
            close(sock);
#endif
            return;
        }
        
        if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Socket error: cannot connect to " << host << ":" << port << std::endl;
#ifdef _WIN32
            closesocket(sock);
#else
            close(sock);
#endif
            return;
        }
        
        std::string message = text + "\n";
        int bytes_sent = send(sock, message.c_str(), message.length(), 0);
        if (bytes_sent < 0) {
            std::cerr << "Socket error: failed to send data" << std::endl;
        }
        
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
    }
};
```

SocketHandler - это обработчик логов, который отправляет сообщения по сети через TCP-сокеты. Наследует от LogHandlerProtocol и реализует интерфейс сетевой отправки.

**Структура класса**
Поля класса : строка *host* - IP или домен сервера, целое число *port* - Порт сервера, *bool winsock_initialized* - Флаг инициализации Winsock (Windows). 

**Конструктор -** валидация параметров. проверяет хост не должен быть пустым, порт должен быть в диапозоне 1-65535.

**метод initializeWinsock -** Инициализация Winsock (Windows). *WSADATA wsaData* - Структура, которая заполняется информацией о инициализированной версии Winsock.
Содержит:
1. Версию Winsock

2. Описание реализации

3. Максимальные размеры сокетов

4. Другую системную информацию

*WSAStartup(MAKEWORD(2, 2), &wsaData)* Инициализирует Winsock DLL (библиотеку сокетов Windows).

**метод cleanupWinsock:**
еспечивает:

1. Корректное освобождение сетевых ресурсов Windows

2. Предотвращение утечек памяти

2. Автоматическую очистку при разрушении объекта

**Деструктор:** просто вызывает метод *cleanupWinsock* 

**метод handle:** 
1. Попытка инициализации сокета
2. Создание сокета AF_INET - IPv4 протокол, SOCK_STREAM - TCP (надёжный потоковый), 0 - протокол по умолчанию (TCP). Возвращает: Дескриптор сокета (≥0) при успехе, -1 при ошибке. 
3. Настройка адреса сервера. *htons(port)* Преобразует порт из host byte order в network byte order (big-endian), *inet_pton()*: Преобразует IP-адрес из строки в binary form. 
4. Подключение к серверу. *connect()*: Устанавливает TCP-соединение с сервером. 
5. Отправка данных *send()* Отправляет данные через установленное соединение.



## class ConsoleHandler
Выводит сообщение в стандартный поток вывода (обычно консоль)

## class SysLogHandler
Имитирует запись в системный лог, добавляя префикс [SYSLOG]


## class Logger 
```cpp
class Logger {
private:
    std::vector<std::shared_ptr<LogFilterProtocol>> filters;
    std::vector<std::shared_ptr<LogHandlerProtocol>> handlers;
    bool use_timestamps;
    bool filter_mode_and; // true = И (все фильтры), false = ИЛИ (хотя бы один)
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
public:
    Logger(std::vector<std::shared_ptr<LogFilterProtocol>> filters = {},
           std::vector<std::shared_ptr<LogHandlerProtocol>> handlers = {},
           bool use_timestamps = false,
           bool filter_mode_and = true)
        : filters(std::move(filters)), handlers(std::move(handlers)),
          use_timestamps(use_timestamps), filter_mode_and(filter_mode_and) {}
    
    void log(const std::string& text) {
        if (text.empty()) return;
        
        // Проверяем фильтры
        bool passed_filter = false;
        if (filters.empty()) {
            passed_filter = true; // Нет фильтров - пропускаем все
        } else if (filter_mode_and) {
            // Режим И: все фильтры должны пропустить
            passed_filter = true;
            for (const auto& filter : filters) {
                if (!filter->match(text)) {
                    passed_filter = false;
                    break;
                }
            }
        } else {
            // Режим ИЛИ: хотя бы один фильтр должен пропустить
            for (const auto& filter : filters) {
                if (filter->match(text)) {
                    passed_filter = true;
                    break;
                }
            }
        }
        
        if (!passed_filter) return;
        
        // Формируем финальное сообщение
        std::string final_message = text;
        if (use_timestamps) {
            final_message = "[" + getCurrentTimestamp() + "] " + text;
        }
        
        // Передаем сообщение всем обработчикам
        for (const auto& handler : handlers) {
            try {
                handler->handle(final_message);
            } catch (const std::exception& e) {
                std::cerr << "Ошибка в обработчике: " << e.what() << std::endl;
            }
        }
    }
    
    void flush() {
        for (const auto& handler : handlers) {
            handler->flush();
        }
    }
    
    void setFilterMode(bool use_and) { filter_mode_and = use_and; }
    void setUseTimestamps(bool use) { use_timestamps = use; }
};
```

Logger - это мощный и гибкий центр управления логированием, который: Фильтрует сообщения по сложным правилам (AND/OR), Форматирует сообщения (временные метки),Распределяет сообщения по обработчикам, Обрабатывает ошибки без падений,Предоставляет гибкие настройки, Управляет буферизацией.

**Структура:** имеет 4 поля, Вектор умных указателей на объекты, реализующие интерфейс *LogFilterProtocol filters* - Фильтры, *LogHandlerProtocol handlers* - Обработчики, флаг *use_timestamps* Добавления временных меток,флаг *filter_mode_and* Режим фильтрации: AND или OR. метод *getCurrentTimestamp()* - Генерация временной метки.

**метод Log** - основной процесс.
1. Проверка пустого сообщения
2. Вариант A: Нет фильтров - пропускаем всё
3. Вариант B: Режим AND - все фильтры должны совпасть
4. Вариант C: Режим OR - хотя бы один фильтр должен совпасть
5. Добавление временной метки, формирование финального сообщения
6. Отправка всем обработчикам



## main
```cpp
int main() {
    try {
        // Очищаем файл логов перед тестами
        std::ofstream clear_file("app.log", std::ios::trunc);
        clear_file.close();
        
        // Создаем фильтры
        auto error_filter = std::make_shared<SimpleLogFilter>("error");
        auto warning_filter = std::make_shared<SimpleLogFilter>("warning");
        auto http_filter = std::make_shared<ReLogFilter>(
            R"(http[s]?://(?:[a-zA-Z]|[0-9]|[$-_@.&+]|[!*\(\),]|(?:%[0-9a-fA-F][0-9a-fA-F]))+)"
        );
        
        // Пример ошибочного паттерна — для теста защиты
        auto bad_filter = std::make_shared<ReLogFilter>("[unclosed[");
        
        // Создаем обработчики
        auto console_handler = std::make_shared<ConsoleHandler>();
        auto file_handler = std::make_shared<FileHandler>("app.log");
        auto syslog_handler = std::make_shared<SyslogHandler>();
        
        // Создаем логгеры с разными конфигурациями
        Logger error_logger(
            {error_filter},
            {console_handler, file_handler, syslog_handler},
            true // с временными метками
        );
        
        Logger warning_logger(
            {warning_filter},
            {console_handler, file_handler}
        );
        
        Logger http_logger(
            {http_filter},
            {file_handler}
        );
        
        Logger bad_logger(
            {bad_filter},
            {console_handler}
        );
        
        // Тестируем логирование
        std::cout << "=== Тестирование системы логирования ===" << std::endl;
        
        error_logger.log("This is an error message");
        error_logger.log("This is a warning message");  // Не должно быть обработано
        warning_logger.log("This is a warning message");
        warning_logger.log("This is an info message");  // Не должно быть обработано
        http_logger.log("Visit our site at https://example.com");
        http_logger.log("This message has no URL");  // Не должно быть обработано
        
        if (bad_filter->isValid()) {
            bad_logger.log("Testing bad regex filter");
        } else {
            std::cout << "Bad regex filter is invalid, skipping test" << std::endl;
        }
        
        // Принудительно записываем буферы
        error_logger.flush();
        warning_logger.flush();
        http_logger.flush();
        
        std::cout << "\nСодержимое файла app.log:" << std::endl;
        std::ifstream file("app.log");
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::cout << line << std::endl;
            }
        } else {
            std::cerr << "Не удалось открыть файл app.log" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```
1. очистка файла логов перед тестами
2. создание фильтров
3. создание ошибочногго паттерна для теста защиты
4. создание обработчиков 
5. Создаем логгеры с разными конфигурациями
6. тесты  логироавния
7. Проверка, является ли regex-фильтр валидным перед использованием.
8. Принудительная запись буферов
