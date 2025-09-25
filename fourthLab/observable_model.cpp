#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <algorithm>

// 1. Протокол слушателя изменений свойства
class IPropertyChangedListener {
public:
    virtual ~IPropertyChangedListener() = default;
    virtual void on_property_changed(void* obj, const std::string& property_name) = 0;
};

// 2. Протокол для управления слушателями изменений
class INotifyDataChanged {
public:
    virtual ~INotifyDataChanged() = default;
    virtual void add_property_changed_listener(std::shared_ptr<IPropertyChangedListener> listener) = 0;
    virtual void remove_property_changed_listener(std::shared_ptr<IPropertyChangedListener> listener) = 0;
};

// 3. Базовый класс с уведомлениями об изменениях
class ObservableModel : public INotifyDataChanged {
protected:
    std::vector<std::shared_ptr<IPropertyChangedListener>> changed_listeners_;

public:
    ObservableModel() = default;
    virtual ~ObservableModel() = default;

    void add_property_changed_listener(std::shared_ptr<IPropertyChangedListener> listener) override {
        changed_listeners_.push_back(listener);
    }

    void remove_property_changed_listener(std::shared_ptr<IPropertyChangedListener> listener) override {
        auto it = std::find(changed_listeners_.begin(), changed_listeners_.end(), listener);
        if (it != changed_listeners_.end()) {
            changed_listeners_.erase(it);
        }
    }

protected:
    void notify_property_changed(const std::string& property_name) {
        for (auto& listener : changed_listeners_) {
            listener->on_property_changed(this, property_name);
        }
    }
};

// 4. Протокол слушателя валидации изменений
class IPropertyChangingListener {
public:
    virtual ~IPropertyChangingListener() = default;
    virtual bool on_property_changing(void* obj, const std::string& property_name, 
                                     const std::string& old_value, const std::string& new_value) = 0;
};

// 5. Протокол для управления валидаторами
class INotifyDataChanging {
public:
    virtual ~INotifyDataChanging() = default;
    virtual void add_property_changing_listener(std::shared_ptr<IPropertyChangingListener> listener) = 0;
    virtual void remove_property_changing_listener(std::shared_ptr<IPropertyChangingListener> listener) = 0;
};

// 6. Расширенный класс с валидацией изменений
class ValidatableModel : public ObservableModel, public INotifyDataChanging {
protected:
    std::vector<std::shared_ptr<IPropertyChangingListener>> changing_listeners_;

public:
    ValidatableModel() = default;
    virtual ~ValidatableModel() = default;

    void add_property_changing_listener(std::shared_ptr<IPropertyChangingListener> listener) override {
        changing_listeners_.push_back(listener);
    }

    void remove_property_changing_listener(std::shared_ptr<IPropertyChangingListener> listener) override {
        auto it = std::find(changing_listeners_.begin(), changing_listeners_.end(), listener);
        if (it != changing_listeners_.end()) {
            changing_listeners_.erase(it);
        }
    }

protected:
    bool validate_property_change(const std::string& property_name, 
                                 const std::string& old_value, const std::string& new_value) {
        for (auto& listener : changing_listeners_) {
            if (!listener->on_property_changing(this, property_name, old_value, new_value)) {
                return false;
            }
        }
        return true;
    }
};

// 7. Реализация демонстрационного класса
class Person : public ValidatableModel {
private:
    std::string name_;
    int age_;

public:
    Person() : name_(""), age_(0) {}

    const std::string& get_name() const { return name_; }
    
    void set_name(const std::string& value) {
        std::string old_value = name_;
        if (validate_property_change("name", old_value, value)) {
            name_ = value;
            notify_property_changed("name");
        }
    }

    int get_age() const { return age_; }
    
    void set_age(int value) {
        std::string old_value = std::to_string(age_);
        std::string new_value = std::to_string(value);
        if (validate_property_change("age", old_value, new_value)) {
            age_ = value;
            notify_property_changed("age");
        }
    }
};

// Реализации слушателей и валидаторов
class DataChangeLogger : public IPropertyChangedListener {
public:
    void on_property_changed(void* obj, const std::string& property_name) override {
        std::cout << "[Изменение] Свойство " << property_name 
                  << " объекта изменено" << std::endl;
    }
};

class PositiveNumberValidator : public IPropertyChangingListener {
public:
    bool on_property_changing(void* obj, const std::string& property_name, 
                             const std::string& old_value, const std::string& new_value) override {
        try {
            int value = std::stoi(new_value);
            if (value < 0) {
                std::cout << "[Валидация] " << property_name 
                          << " не может быть отрицательным!" << std::endl;
                return false;
            }
        } catch (const std::exception&) {
            // Если не число, пропускаем валидацию
        }
        return true;
    }
};

class NameLengthValidator : public IPropertyChangingListener {
public:
    bool on_property_changing(void* obj, const std::string& property_name, 
                             const std::string& old_value, const std::string& new_value) override {
        if (property_name == "name" && new_value.length() < 3) {
            std::cout << "[Валидация] Имя должно быть не короче 3 символов!" << std::endl;
            return false;
        }
        return true;
    }
};

// Демонстрация работы
int main() {
    std::cout << "Создаем объект Person..." << std::endl;
    Person person;
    
    // Добавляем слушателей
    auto logger = std::make_shared<DataChangeLogger>();
    person.add_property_changed_listener(logger);
    
    // Добавляем валидаторы
    auto number_validator = std::make_shared<PositiveNumberValidator>();
    auto name_validator = std::make_shared<NameLengthValidator>();
    person.add_property_changing_listener(number_validator);
    person.add_property_changing_listener(name_validator);
    
    // Тестовые изменения
    std::cout << "\nПопытка установить возраст -5:" << std::endl;
    person.set_age(-5);  // Будет отклонено
    
    std::cout << "\nПопытка установить имя 'Al':" << std::endl;
    person.set_name("Al");  // Будет отклонено
    
    std::cout << "\nУстанавливаем корректные значения:" << std::endl;
    person.set_name("Alice");
    person.set_age(25);
    
    std::cout << "\nТекущее состояние:" << std::endl;
    std::cout << "Имя: " << person.get_name() 
              << ", Возраст: " << person.get_age() << std::endl;
    
    return 0;
}
