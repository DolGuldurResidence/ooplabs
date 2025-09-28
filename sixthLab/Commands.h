#ifndef COMMANDS_H
#define COMMANDS_H

#include "Command.h"
#include <memory>

// Forward declaration
class Keyboard;

// Команда для печати символа
class PrintCommand : public Command {
private:
    Keyboard* receiver;
    char character;

public:
    PrintCommand(Keyboard* receiver, char character);
    std::string execute() override;
    std::string undo() override;
    std::string redo() override;
    
    // Геттер для сериализации
    char get_character() const { return character; }
};

// Команда для увеличения громкости
class VolumeUpCommand : public Command {
private:
    Keyboard* receiver;
    int step;

public:
    VolumeUpCommand(Keyboard* receiver, int step = 10);
    std::string execute() override;
    std::string undo() override;
    std::string redo() override;
    
    // Геттер для сериализации
    int get_step() const { return step; }
};

// Команда для уменьшения громкости
class VolumeDownCommand : public Command {
private:
    Keyboard* receiver;
    int step;

public:
    VolumeDownCommand(Keyboard* receiver, int step = 10);
    std::string execute() override;
    std::string undo() override;
    std::string redo() override;
    
    // Геттер для сериализации
    int get_step() const { return step; }
};

// Команда для запуска медиа-плеера
class MediaPlayerCommand : public Command {
private:
    Keyboard* receiver;

public:
    MediaPlayerCommand(Keyboard* receiver);
    std::string execute() override;
    std::string undo() override;
    std::string redo() override;
};

#endif // COMMANDS_H
