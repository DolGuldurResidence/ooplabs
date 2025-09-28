#include "Keyboard.h"
#include <iostream>

using namespace std;

Keyboard::Keyboard() : volume(0), is_media_player_running(false) {
}

void Keyboard::add_binding(const string& key, shared_ptr<Command> command) {
    key_bindings[key] = command;
}

void Keyboard::execute_command(const string& key) {
    auto it = key_bindings.find(key);
    if (it == key_bindings.end()) {
        return;
    }
    
    shared_ptr<Command> cmd = it->second;
    string msg = cmd->execute();
    output.push_back(msg);
    undo_stack.push_back(cmd);
    redo_stack.clear();
}

void Keyboard::undo() {
    if (undo_stack.empty()) {
        return;
    }
    
    shared_ptr<Command> cmd = undo_stack.back();
    undo_stack.pop_back();
    string msg = cmd->undo();
    output.push_back(msg);
    redo_stack.push_back(cmd);
}

void Keyboard::redo() {
    if (redo_stack.empty()) {
        return;
    }
    
    shared_ptr<Command> cmd = redo_stack.back();
    redo_stack.pop_back();
    string msg = cmd->redo();
    output.push_back(msg);
    undo_stack.push_back(cmd);
}

void Keyboard::print_char(char c) {
    text_buffer += c;
}

void Keyboard::remove_last_char() {
    if (!text_buffer.empty()) {
        text_buffer.pop_back();
    }
}

void Keyboard::increase_volume(int step) {
    volume += step;
}

void Keyboard::decrease_volume(int step) {
    volume -= step;
}

void Keyboard::launch_media_player() {
    is_media_player_running = true;
}

void Keyboard::close_media_player() {
    is_media_player_running = false;
}
