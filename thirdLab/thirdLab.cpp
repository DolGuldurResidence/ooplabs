#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <memory>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class LogFilterProtocol {
public:
    virtual ~LogFilterProtocol() = default;
    virtual bool match(const std::string& text) = 0; 
};

class SimpleLogFilter : public LogFilterProtocol {
private:
    std::string pattern;
    
public:
    explicit SimpleLogFilter(const std::string& pattern) : pattern(pattern) {
        if (pattern.empty()) {
            throw std::invalid_argument("Pattern cannot be empty");
        }

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

class LogHandlerProtocol {
public:
    virtual ~LogHandlerProtocol() = default;
    virtual void handle(const std::string& text) = 0;
    virtual void flush() {}
};

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

class ConsoleHandler : public LogHandlerProtocol {
public:
    void handle(const std::string& text) override {
        std::cout << text << std::endl;
    }
};

class SyslogHandler : public LogHandlerProtocol {
public:
    void handle(const std::string& text) override {
        // В реальной реализации здесь был бы вызов системных функций syslog
        // Для демонстрации выводим в консоль с префиксом
        std::cout << "[SYSLOG] " << text << std::endl;
    }
};

// Улучшенный класс Logger с поддержкой временных меток
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

// Демонстрация работы системы
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