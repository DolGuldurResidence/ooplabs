#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H

#include <string>
#include <map>
#include <functional>
#include <memory>
#include "Command.h"

// Forward declaration
class Keyboard;

// Фабрика для создания команд
class CommandFactory {
private:
    std::map<std::string, std::function<std::shared_ptr<Command>(Keyboard*, const std::map<std::string, std::string>&)>> creators;
    std::map<std::string, std::function<std::map<std::string, std::string>(Command*)>> serializers;

public:
    // Регистрация типа команды
    void register_command(const std::string& command_type, 
                         std::function<std::shared_ptr<Command>(Keyboard*, const std::map<std::string, std::string>&)> creator,
                         std::function<std::map<std::string, std::string>(Command*)> serializer);
    
    // Создание команды по типу и параметрам
    std::shared_ptr<Command> create_command(const std::string& command_type, 
                                           Keyboard* receiver, 
                                           const std::map<std::string, std::string>& params);
    
    // Сериализация команды в параметры
    std::map<std::string, std::string> serialize_command(Command* command);
    
    // Получение типа команды
    std::string get_command_type(Command* command);
};

#endif // COMMANDFACTORY_H
