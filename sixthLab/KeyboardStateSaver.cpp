#include "KeyboardStateSaver.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

KeyboardStateSaver::KeyboardStateSaver(CommandFactory* factory) : factory(factory) {
}

void KeyboardStateSaver::save(const string& filename, Keyboard* keyboard) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Cannot open file for writing: " + filename);
    }
    
    file << "{\n";
    file << "  \"key_bindings\": {\n";
    
    const auto& bindings = keyboard->get_key_bindings();
    bool first = true;
    
    for (const auto& pair : bindings) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        const string& key = pair.first;
        Command* command = pair.second.get();
        
        string command_type = factory->get_command_type(command);
        auto params = factory->serialize_command(command);
        
        file << "    \"" << key << "\": {\n";
        file << "      \"type\": \"" << command_type << "\",\n";
        file << "      \"params\": {\n";
        
        bool first_param = true;
        for (const auto& param : params) {
            if (!first_param) {
                file << ",\n";
            }
            first_param = false;
            file << "        \"" << param.first << "\": \"" << param.second << "\"";
        }
        
        file << "\n      }\n";
        file << "    }";
    }
    
    file << "\n  }\n";
    file << "}\n";
    
    file.close();
}

void KeyboardStateSaver::load(const string& filename, Keyboard* keyboard) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Cannot open file for reading: " + filename);
    }
    
    // Простой парсер JSON (для учебных целей)
    string content((istreambuf_iterator<char>(file)),
                        istreambuf_iterator<char>());
    file.close();
    
    keyboard->clear_bindings();
    
    // Находим секцию key_bindings
    size_t start = content.find("\"key_bindings\"");
    if (start == string::npos) {
        throw runtime_error("Invalid JSON format: key_bindings not found");
    }
    
    start = content.find("{", start);
    if (start == string::npos) {
        throw runtime_error("Invalid JSON format: key_bindings object not found");
    }
    
    // Простой парсинг (в реальном проекте лучше использовать библиотеку JSON)
    size_t pos = start + 1;
    while (pos < content.length()) {
        // Ищем ключ
        size_t key_start = content.find("\"", pos);
        if (key_start == string::npos) break;
        
        size_t key_end = content.find("\"", key_start + 1);
        if (key_end == string::npos) break;
        
        string key = content.substr(key_start + 1, key_end - key_start - 1);
        
        // Ищем тип команды
        size_t type_start = content.find("\"type\": \"", key_end);
        if (type_start == string::npos) break;
        
        type_start += 9; // длина "\"type\": \""
        size_t type_end = content.find("\"", type_start);
        if (type_end == string::npos) break;
        
        string command_type = content.substr(type_start, type_end - type_start);
        
        // Создаем команду
        map<string, string> params;
        
        // Для PrintCommand извлекаем char
        if (command_type == "PrintCommand") {
            size_t char_start = content.find("\"char\": \"", type_end);
            if (char_start != string::npos) {
                char_start += 9; // длина "\"char\": \""
                size_t char_end = content.find("\"", char_start);
                if (char_end != string::npos) {
                    string char_str = content.substr(char_start, char_end - char_start);
                    if (!char_str.empty()) {
                        params["char"] = char_str;
                    }
                }
            }
        }
        // Для VolumeUpCommand и VolumeDownCommand извлекаем step
        else if (command_type == "VolumeUpCommand" || command_type == "VolumeDownCommand") {
            size_t step_start = content.find("\"step\": \"", type_end);
            if (step_start != string::npos) {
                step_start += 9; // длина "\"step\": \""
                size_t step_end = content.find("\"", step_start);
                if (step_end != string::npos) {
                    params["step"] = content.substr(step_start, step_end - step_start);
                }
            }
        }
        
        // Создаем команду и добавляем привязку
        try {
            auto command = factory->create_command(command_type, keyboard, params);
            keyboard->add_binding(key, command);
        } catch (const exception& e) {
            cerr << "Error creating command: " << e.what() << endl;
        }
        
        // Переходим к следующей записи
        pos = content.find("}", type_end);
        if (pos == string::npos) break;
        pos++;
    }
}
