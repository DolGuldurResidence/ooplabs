#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <any>
#include <stack>
#include <string>
#include <iostream>
#include <locale>
#include <codecvt>

// Перечисление для режимов жизненного цикла
enum class LifeStyle {
    PER_REQUEST,
    SCOPED,
    SINGLETON
};

// Класс для хранения информации о регистрации
class Registration {
public:
    std::function<void*()> factory;
    LifeStyle lifestyle;
    std::vector<std::any> params;
    void* instance = nullptr;
    std::mutex lock;
    
    Registration(std::function<void*()> f, LifeStyle ls, std::vector<std::any> p = {})
        : factory(f), lifestyle(ls), params(p) {}
};

// Основной класс инжектора зависимостей
class Injector {
private:
    std::unordered_map<std::string, std::unique_ptr<Registration>> registrations;
    std::stack<std::unordered_map<std::string, void*>> scoped_stack;
    std::mutex global_lock;
    std::vector<std::string> resolving_stack; // Трекер циклических зависимостей

public:
    // Регистрация зависимости
    template<typename InterfaceType, typename ClassType>
    void register_type(LifeStyle lifestyle, std::vector<std::any> params = {}) {
        std::string interface_name = typeid(InterfaceType).name();
        
        auto factory = [this, params]() -> void* {
            // Проверка циклических зависимостей
            std::string class_name = typeid(ClassType).name();
            for (const auto& resolving : resolving_stack) {
                if (resolving == class_name) {
                    throw std::runtime_error("Cyclic dependency detected: " + class_name);
                }
            }
            
            resolving_stack.push_back(class_name);
            
            // Рекурсивное разрешение параметров
            std::vector<std::any> resolved_params;
            for (const auto& param : params) {
                if (param.type() == typeid(std::string)) {
                    std::string param_str = std::any_cast<std::string>(param);
                    if (param_str.substr(0, 6) == "param:") {
                        // Явная передача значения
                        resolved_params.push_back(param_str.substr(6));
                    } else {
                        resolved_params.push_back(param);
                    }
                } else {
                    resolved_params.push_back(param);
                }
            }
            
            // Создание экземпляра класса
            void* instance = new ClassType();
            resolving_stack.pop_back();
            return instance;
        };
        
        std::lock_guard<std::mutex> lock(global_lock);
        registrations[interface_name] = std::make_unique<Registration>(factory, lifestyle, params);
    }
    
    // Регистрация с фабричным методом
    template<typename InterfaceType>
    void register_factory(std::function<void*()> factory, LifeStyle lifestyle) {
        std::string interface_name = typeid(InterfaceType).name();
        std::lock_guard<std::mutex> lock(global_lock);
        registrations[interface_name] = std::make_unique<Registration>(factory, lifestyle);
    }
    
    // Получение экземпляра
    template<typename InterfaceType>
    std::shared_ptr<InterfaceType> get_instance() {
        std::string interface_name = typeid(InterfaceType).name();
        
        std::lock_guard<std::mutex> lock(global_lock);
        auto it = registrations.find(interface_name);
        if (it == registrations.end()) {
            throw std::runtime_error("No registration for " + interface_name);
        }
        
        Registration* reg = it->second.get();
        
        switch (reg->lifestyle) {
            case LifeStyle::PER_REQUEST: {
                void* instance = reg->factory();
                return std::shared_ptr<InterfaceType>(static_cast<InterfaceType*>(instance));
            }
            
            case LifeStyle::SINGLETON: {
                std::lock_guard<std::mutex> reg_lock(reg->lock);
                if (reg->instance == nullptr) {
                    reg->instance = reg->factory();
                }
                return std::shared_ptr<InterfaceType>(static_cast<InterfaceType*>(reg->instance));
            }
            
            case LifeStyle::SCOPED: {
                if (scoped_stack.empty()) {
                    throw std::runtime_error("No active scope");
                }
                
                auto& scope = scoped_stack.top();
                auto scope_it = scope.find(interface_name);
                if (scope_it == scope.end()) {
                    void* instance = reg->factory();
                    scope[interface_name] = instance;
                    return std::shared_ptr<InterfaceType>(static_cast<InterfaceType*>(instance));
                }
                return std::shared_ptr<InterfaceType>(static_cast<InterfaceType*>(scope_it->second));
            }
        }
        
        return nullptr;
    }
    
    // Создание области видимости
    void create_scope() {
        std::lock_guard<std::mutex> lock(global_lock);
        scoped_stack.push(std::unordered_map<std::string, void*>());
    }
    
    void end_scope() {
        std::lock_guard<std::mutex> lock(global_lock);
        if (!scoped_stack.empty()) {
            scoped_stack.pop();
        }
    }
};

// Интерфейсы
class Interface1 {
public:
    virtual ~Interface1() = default;
    virtual void do_work() = 0;
};

class Interface2 {
public:
    virtual ~Interface2() = default;
    virtual int calculate(int x, int y) = 0;
};

class Interface3 {
public:
    virtual ~Interface3() = default;
    virtual void report() = 0;
};

// Реализации для Interface1
class Class1Debug : public Interface1 {
public:
    void do_work() override {
        std::cout << "Class1Debug doing work in debug mode" << std::endl;
    }
};

class Class1Release : public Interface1 {
public:
    void do_work() override {
        std::cout << "Class1Release doing work in release mode" << std::endl;
    }
};

// Реализации для Interface2
class Class2Debug : public Interface2 {
public:
    int calculate(int x, int y) override {
        std::cout << "Class2Debug calculates " << x << " + " << y << std::endl;
        return x + y;
    }
};

class Class2Release : public Interface2 {
public:
    int calculate(int x, int y) override {
        std::cout << "Class2Release calculates " << x << " * " << y << std::endl;
        return x * y;
    }
};

// Реализации для Interface3
class Class3Debug : public Interface3 {
public:
    void report() override {
        std::cout << "Class3Debug report: DEBUG data" << std::endl;
    }
};

class Class3Release : public Interface3 {
public:
    void report() override {
        std::cout << "Class3Release report: RELEASE summary" << std::endl;
    }
};

// Конфигурационные функции
void config_debug(Injector& injector) {
    injector.register_type<Interface1, Class1Debug>(LifeStyle::PER_REQUEST);
    injector.register_type<Interface2, Class2Debug>(LifeStyle::SCOPED);
    injector.register_type<Interface3, Class3Debug>(LifeStyle::SINGLETON);
}

void config_release(Injector& injector) {
    injector.register_type<Interface1, Class1Release>(LifeStyle::PER_REQUEST);
    injector.register_type<Interface2, Class2Release>(LifeStyle::SCOPED);
    injector.register_type<Interface3, Class3Release>(LifeStyle::SINGLETON);
}

// Демонстрация сложных зависимостей
class DatabaseConfig {
public:
    std::string connection_string;
    
    DatabaseConfig(const std::string& conn_str) : connection_string(conn_str) {}
};

class Logger {
public:
    std::string log_level;
    
    Logger(const std::string& level) : log_level(level) {
        std::cout << "Logger created with level: " << level << std::endl;
    }
    
    void log(const std::string& message) {
        std::cout << "[" << log_level << "] " << message << std::endl;
    }
};

class UserService {
public:
    DatabaseConfig* db_config;
    Logger* logger;
    
    UserService(DatabaseConfig* db, Logger* log) : db_config(db), logger(log) {
        std::cout << "UserService created with DB: " << db->connection_string << std::endl;
    }
    
    std::string get_user(int id) {
        logger->log("Fetching user " + std::to_string(id));
        return "User_" + std::to_string(id);
    }
};

void config_complex(Injector& injector) {
    // Регистрация с параметрами
    injector.register_factory<DatabaseConfig>(
        []() -> void* { return new DatabaseConfig("postgres://user:pass@localhost/db"); },
        LifeStyle::SINGLETON
    );
    
    injector.register_factory<Logger>(
        []() -> void* { return new Logger("DEBUG"); },
        LifeStyle::PER_REQUEST
    );
    
    injector.register_factory<UserService>(
        [&injector]() -> void* {
            auto db_config = injector.get_instance<DatabaseConfig>();
            auto logger = injector.get_instance<Logger>();
            return new UserService(db_config.get(), logger.get());
        },
        LifeStyle::PER_REQUEST
    );
}
