#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <optional>
#include <sstream>

using namespace std;

// 1. Класс User - представляет пользователя системы
class User {
private:
    int id;                    // Уникальный идентификатор
    string name;               // Имя пользователя
    string login;              // Логин для входа
    string password;           // Пароль (скрыт при выводе)
    optional<string> email;    // Email (необязательное поле)
    optional<string> address;  // Адрес (необязательное поле)

public:
    // Конструктор - создает нового пользователя
    User(int id, const string& name, const string& login, const string& password, 
         const optional<string>& email = nullopt, const optional<string>& address = nullopt)
        : id(id), name(name), login(login), password(password), email(email), address(address) {}

    // Геттеры - методы для получения значений полей
    int getId() const { return id; }
    const string& getName() const { return name; }
    const string& getLogin() const { return login; }
    const string& getPassword() const { return password; }
    const optional<string>& getEmail() const { return email; }
    const optional<string>& getAddress() const { return address; }

    // Сеттеры - методы для изменения значений полей
    void setName(const string& name) { this->name = name; }
    void setLogin(const string& login) { this->login = login; }
    void setPassword(const string& password) { this->password = password; }
    void setEmail(const optional<string>& email) { this->email = email; }
    void setAddress(const optional<string>& address) { this->address = address; }

    // Оператор для сортировки по имени (требование задания)
    bool operator<(const User& other) const {
        return name < other.name;
    }

    // Оператор равенства для сравнения пользователей
    bool operator==(const User& other) const {
        return id == other.id;
    }

    // Вывод информации о пользователе (без пароля)
    friend ostream& operator<<(ostream& os, const User& user) {
        os << "ID: " << user.id << ", Name: " << user.name << ", Login: " << user.login;
        if (user.email.has_value()) {
            os << ", Email: " << user.email.value();
        }
        if (user.address.has_value()) {
            os << ", Address: " << user.address.value();
        }
        return os;
    }
};

// 2. Интерфейс IDataRepository - базовый интерфейс для работы с данными
template<typename T>
class IDataRepository {
public:
    virtual ~IDataRepository() = default;  // Виртуальный деструктор
    virtual vector<T> getAll() = 0;        // Получить все записи
    virtual optional<T> getById(int id) = 0;  // Найти по ID
    virtual void add(const T& item) = 0;   // Добавить запись
    virtual void update(const T& item) = 0; // Обновить запись
    virtual void remove(const T& item) = 0; // Удалить запись
};

// 3. Интерфейс IUserRepository - специализированный для пользователей
class IUserRepository : public IDataRepository<User> {
public:
    virtual ~IUserRepository() = default;
    virtual optional<User> getByLogin(const string& login) = 0;  // Поиск по логину
};

// 4. Простой JSON-сериализатор (упрощенная версия)
class SimpleJsonSerializer {
public:
    // Преобразование пользователя в JSON строку
    static string userToJson(const User& user) {
        ostringstream json;
        json << "{";
        json << "\"id\":" << user.getId() << ",";
        json << "\"name\":\"" << user.getName() << "\",";
        json << "\"login\":\"" << user.getLogin() << "\",";
        json << "\"password\":\"" << user.getPassword() << "\",";
        
        if (user.getEmail().has_value()) {
            json << "\"email\":\"" << user.getEmail().value() << "\",";
        } else {
            json << "\"email\":null,";
        }
        
        if (user.getAddress().has_value()) {
            json << "\"address\":\"" << user.getAddress().value() << "\"";
        } else {
            json << "\"address\":null";
        }
        
        json << "}";
        return json.str();
    }

    // Преобразование JSON строки в пользователя (упрощенный парсер)
    static User jsonToUser(const string& json) {
        int id = 0;
        string name, login, password;
        optional<string> email, address;
        
        // Простой парсинг JSON (для демонстрации)
        // В реальном проекте лучше использовать библиотеку nlohmann/json
        
        size_t pos = json.find("\"id\":");
        if (pos != string::npos) {
            pos += 5;
            size_t end = json.find(",", pos);
            id = stoi(json.substr(pos, end - pos));
        }
        
        pos = json.find("\"name\":\"");
        if (pos != string::npos) {
            pos += 8;
            size_t end = json.find("\"", pos);
            name = json.substr(pos, end - pos);
        }
        
        pos = json.find("\"login\":\"");
        if (pos != string::npos) {
            pos += 9;
            size_t end = json.find("\"", pos);
            login = json.substr(pos, end - pos);
        }
        
        pos = json.find("\"password\":\"");
        if (pos != string::npos) {
            pos += 12;
            size_t end = json.find("\"", pos);
            password = json.substr(pos, end - pos);
        }
        
        pos = json.find("\"email\":\"");
        if (pos != string::npos) {
            pos += 9;
            size_t end = json.find("\"", pos);
            email = json.substr(pos, end - pos);
        }
        
        pos = json.find("\"address\":\"");
        if (pos != string::npos) {
            pos += 11;
            size_t end = json.find("\"", pos);
            address = json.substr(pos, end - pos);
        }
        
        return User(id, name, login, password, email, address);
    }
};

// 5. Реализация JsonDataRepository - хранение в JSON файле
class JsonDataRepository : public IDataRepository<User> {
private:
    string filename;        // Имя файла для хранения
    vector<User> users;     // Вектор пользователей в памяти

    // Загрузка данных из файла
    void loadFromFile() {
        users.clear();
        ifstream file(filename);
        if (!file.is_open()) {
            return; // Файл не существует, создадим пустой список
        }

        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                try {
                    users.push_back(SimpleJsonSerializer::jsonToUser(line));
                } catch (const exception& e) {
                    cerr << "Ошибка при загрузке пользователя: " << e.what() << endl;
                }
            }
        }
        file.close();
    }

    // Сохранение данных в файл
    void saveToFile() {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Ошибка при сохранении в файл: " << filename << endl;
            return;
        }

        for (const auto& user : users) {
            file << SimpleJsonSerializer::userToJson(user) << endl;
        }
        file.close();
    }

public:
    // Конструктор - загружает данные при создании
    JsonDataRepository(const string& filename) : filename(filename) {
        loadFromFile();
    }

    // Реализация методов интерфейса IDataRepository
    vector<User> getAll() override {
        return users;
    }

    optional<User> getById(int id) override {
        auto it = find_if(users.begin(), users.end(), 
                         [id](const User& user) { return user.getId() == id; });
        if (it != users.end()) {
            return *it;
        }
        return nullopt;
    }

    void add(const User& user) override {
        users.push_back(user);
        saveToFile();  // Автоматически сохраняем изменения
    }

    void update(const User& user) override {
        auto it = find_if(users.begin(), users.end(), 
                         [&user](const User& u) { return u.getId() == user.getId(); });
        if (it != users.end()) {
            *it = user;
            saveToFile();
        } else {
            throw runtime_error("Пользователь не найден");
        }
    }

    void remove(const User& user) override {
        auto it = find(users.begin(), users.end(), user);
        if (it != users.end()) {
            users.erase(it);
            saveToFile();
        } else {
            throw runtime_error("Пользователь не найден");
        }
    }
};

// 6. Реализация UserRepository - репозиторий для пользователей
class UserRepository : public IUserRepository {
private:
    JsonDataRepository jsonRepo;  // Композиция - используем JsonDataRepository

public:
    UserRepository(const string& filename) : jsonRepo(filename) {}

    // Делегирование методов базовому репозиторию
    vector<User> getAll() override {
        return jsonRepo.getAll();
    }

    optional<User> getById(int id) override {
        return jsonRepo.getById(id);
    }

    void add(const User& user) override {
        jsonRepo.add(user);
    }

    void update(const User& user) override {
        jsonRepo.update(user);
    }

    void remove(const User& user) override {
        jsonRepo.remove(user);
    }

    // Специальный метод для поиска по логину
    optional<User> getByLogin(const string& login) override {
        auto users = getAll();
        auto it = find_if(users.begin(), users.end(), 
                         [&login](const User& user) { return user.getLogin() == login; });
        if (it != users.end()) {
            return *it;
        }
        return nullopt;
    }
};

// 7. Интерфейс IAuthService - для авторизации
class IAuthService {
public:
    virtual ~IAuthService() = default;
    virtual void signIn(const User& user) = 0;    // Войти в систему
    virtual void signOut() = 0;                   // Выйти из системы
    virtual bool isAuthorized() const = 0;        // Проверить авторизацию
    virtual User getCurrentUser() const = 0;      // Получить текущего пользователя
};

// 8. Реализация FileAuthService - авторизация с сохранением в файл
class FileAuthService : public IAuthService {
private:
    shared_ptr<IUserRepository> userRepo;  // Умный указатель на репозиторий
    string authFile;                       // Файл для хранения авторизации
    optional<User> currentUser;            // Текущий авторизованный пользователь

    // Загрузка авторизации из файла
    void loadAuth() {
        currentUser = nullopt;
        ifstream file(authFile);
        if (!file.is_open()) {
            return;
        }

        string line;
        if (getline(file, line) && !line.empty()) {
            try {
                int userId = stoi(line);
                currentUser = userRepo->getById(userId);
            } catch (const exception& e) {
                cerr << "Ошибка при загрузке авторизации: " << e.what() << endl;
            }
        }
        file.close();
    }

    // Сохранение авторизации в файл
    void saveAuth() {
        ofstream file(authFile);
        if (file.is_open()) {
            if (currentUser.has_value()) {
                file << currentUser->getId();
            }
            file.close();
        }
    }

public:
    FileAuthService(shared_ptr<IUserRepository> userRepo, const string& authFile = "auth.txt")
        : userRepo(userRepo), authFile(authFile) {
        loadAuth();  // Автоматически загружаем авторизацию при создании
    }

    void signIn(const User& user) override {
        currentUser = user;
        saveAuth();
    }

    void signOut() override {
        currentUser = nullopt;
        saveAuth();
    }

    bool isAuthorized() const override {
        return currentUser.has_value();
    }

    User getCurrentUser() const override {
        if (!currentUser.has_value()) {
            throw runtime_error("Пользователь не авторизован");
        }
        return currentUser.value();
    }
};

// 9. Демонстрация работы системы
int main() {
    cout << "=== Система авторизации ===" << endl;

    // Инициализация репозитория пользователей
    auto userRepo = make_shared<UserRepository>("users.txt");
    
    // Инициализация сервиса авторизации
    auto authService = make_shared<FileAuthService>(userRepo);

    try {
        // 1. Добавление пользователей
        cout << "\n1. Добавление пользователей:" << endl;
        User user1(1, "Alice", "alice", "pass123", "alice@example.com");
        User user2(2, "Bob", "bob", "bobpass", nullopt, "City");
        
        userRepo->add(user1);
        userRepo->add(user2);
        
        cout << "Пользователь Alice добавлен" << endl;
        cout << "Пользователь Bob добавлен" << endl;

        // 2. Авторизация пользователя
        cout << "\n2. Авторизация пользователя Alice:" << endl;
        authService->signIn(user1);
        cout << "Авторизован: " << authService->getCurrentUser().getName() << endl;

        // 3. Смена пользователя
        cout << "\n3. Смена пользователя на Bob:" << endl;
        authService->signIn(user2);
        cout << "Новый пользователь: " << authService->getCurrentUser().getName() << endl;

        // 4. Демонстрация автоматической авторизации
        cout << "\n4. Автоматическая авторизация при перезапуске:" << endl;
        auto newAuthService = make_shared<FileAuthService>(userRepo);
        if (newAuthService->isAuthorized()) {
            cout << "Автоматически авторизован: " << newAuthService->getCurrentUser().getName() << endl;
        } else {
            cout << "Пользователь не авторизован" << endl;
        }

        // 5. Выход из системы
        cout << "\n5. Выход из системы:" << endl;
        newAuthService->signOut();
        cout << "После выхода: авторизован — " << (newAuthService->isAuthorized() ? "да" : "нет") << endl;

        // 6. Демонстрация поиска по логину
        cout << "\n6. Поиск пользователя по логину:" << endl;
        auto foundUser = userRepo->getByLogin("alice");
        if (foundUser.has_value()) {
            cout << "Найден пользователь: " << foundUser.value() << endl;
        }

        // 7. Демонстрация сортировки пользователей
        cout << "\n7. Список всех пользователей (отсортированный по имени):" << endl;
        auto allUsers = userRepo->getAll();
        sort(allUsers.begin(), allUsers.end());  // Сортировка по имени
        for (const auto& user : allUsers) {
            cout << user << endl;
        }

    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }

    cout << "\n=== Демонстрация завершена ===" << endl;
    return 0;
}
