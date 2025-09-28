#ifndef COMMAND_H
#define COMMAND_H

#include <string>

// Базовый абстрактный класс для всех команд
class Command {
public:
    virtual ~Command() = default;
    virtual std::string execute() = 0;
    virtual std::string undo() = 0;
    virtual std::string redo() = 0;
};

#endif // COMMAND_H
