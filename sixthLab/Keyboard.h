#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Command.h"

// Класс виртуальной клавиатуры
class Keyboard {
private:
    std::map<std::string, std::shared_ptr<Command>> key_bindings;
    std::vector<std::shared_ptr<Command>> undo_stack;
    std::vector<std::shared_ptr<Command>> redo_stack;
    std::string text_buffer;
    int volume;
    bool is_media_player_running;
    std::vector<std::string> output;

public:
    Keyboard();
    
    // Управление привязками клавиш
    void add_binding(const std::string& key, std::shared_ptr<Command> command);
    void execute_command(const std::string& key);
    
    // Операции undo/redo
    void undo();
    void redo();
    
    // Методы для команд (receiver методы)
    void print_char(char c);
    void remove_last_char();
    void increase_volume(int step);
    void decrease_volume(int step);
    void launch_media_player();
    void close_media_player();
    
    // Геттеры
    const std::string& get_text_buffer() const { return text_buffer; }
    int get_volume() const { return volume; }
    bool get_is_media_player_running() const { return is_media_player_running; }
    const std::vector<std::string>& get_output() const { return output; }
    const std::map<std::string, std::shared_ptr<Command>>& get_key_bindings() const { return key_bindings; }
    
    // Очистка привязок (для загрузки состояния)
    void clear_bindings() { key_bindings.clear(); }
};

#endif // KEYBOARD_H
