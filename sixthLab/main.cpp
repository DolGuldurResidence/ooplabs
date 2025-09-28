#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include "Keyboard.h"
#include "Commands.h"
#include "CommandFactory.h"
#include "KeyboardStateSaver.h"

using namespace std;

int main() {
    // Создаем клавиатуру и фабрику команд
    Keyboard keyboard;
    CommandFactory factory;
    
    // Регистрируем команды в фабрике
    factory.register_command("PrintCommand",
        [](Keyboard* receiver, const map<string, string>& params) -> shared_ptr<Command> {
            char character = params.at("char")[0];
            return make_shared<PrintCommand>(receiver, character);
        },
        [](Command* cmd) -> map<string, string> {
            PrintCommand* printCmd = dynamic_cast<PrintCommand*>(cmd);
            return {{"char", string(1, printCmd->get_character())}};
        });
    
    factory.register_command("VolumeUpCommand",
        [](Keyboard* receiver, const map<string, string>& params) -> shared_ptr<Command> {
            int step = 10;
            if (params.find("step") != params.end()) {
                step = stoi(params.at("step"));
            }
            return make_shared<VolumeUpCommand>(receiver, step);
        },
        [](Command* cmd) -> map<string, string> {
            VolumeUpCommand* volCmd = dynamic_cast<VolumeUpCommand*>(cmd);
            return {{"step", to_string(volCmd->get_step())}};
        });
    
    factory.register_command("VolumeDownCommand",
        [](Keyboard* receiver, const map<string, string>& params) -> shared_ptr<Command> {
            int step = 10;
            if (params.find("step") != params.end()) {
                step = stoi(params.at("step"));
            }
            return make_shared<VolumeDownCommand>(receiver, step);
        },
        [](Command* cmd) -> map<string, string> {
            VolumeDownCommand* volCmd = dynamic_cast<VolumeDownCommand*>(cmd);
            return {{"step", to_string(volCmd->get_step())}};
        });
    
    factory.register_command("MediaPlayerCommand",
        [](Keyboard* receiver, const map<string, string>& /*params*/) -> shared_ptr<Command> {
            return make_shared<MediaPlayerCommand>(receiver);
        },
        [](Command* /*cmd*/) -> map<string, string> {
            return {};
        });
    
    // Создаем saver для сохранения/загрузки состояния
    KeyboardStateSaver saver(&factory);
    
    // Привязываем команды к клавишам
    keyboard.add_binding("a", make_shared<PrintCommand>(&keyboard, 'a'));
    keyboard.add_binding("b", make_shared<PrintCommand>(&keyboard, 'b'));
    keyboard.add_binding("c", make_shared<PrintCommand>(&keyboard, 'c'));
    keyboard.add_binding("d", make_shared<PrintCommand>(&keyboard, 'd'));
    keyboard.add_binding("ctrl++", make_shared<VolumeUpCommand>(&keyboard, 20));
    keyboard.add_binding("ctrl+-", make_shared<VolumeDownCommand>(&keyboard, 20));
    keyboard.add_binding("ctrl+p", make_shared<MediaPlayerCommand>(&keyboard));
    
    // Сохраняем состояние клавиатуры
    try {
        saver.save("keyboard_state.json", &keyboard);
        cout << "Keyboard state saved to keyboard_state.json" << endl;
    } catch (const exception& e) {
        cerr << "Error saving keyboard state: " << e.what() << endl;
    }
    
    // Последовательность команд для демонстрации
    vector<string> inputs = {
        "a", "b", "c", "undo", "undo", "redo",
        "ctrl++", "ctrl+-", "ctrl+p", "d", "undo", "undo"
    };
    
    // Выполняем команды
    for (const string& key : inputs) {
        if (key == "undo") {
            keyboard.undo();
        } else if (key == "redo") {
            keyboard.redo();
        } else {
            keyboard.execute_command(key);
        }
    }
    
    // Выводим результаты в консоль и в файл
    ofstream output_file("output.txt");
    if (!output_file.is_open()) {
        cerr << "Cannot open output.txt for writing" << endl;
        return 1;
    }
    
    cout << "\n=== CONSOLE OUTPUT ===" << endl;
    for (const string& line : keyboard.get_output()) {
        cout << line << endl;
        output_file << line << endl;
    }
    
    output_file.close();
    cout << "\nOutput also saved to output.txt" << endl;
    
    // Демонстрируем загрузку состояния
    cout << "\n=== TESTING STATE LOADING ===" << endl;
    Keyboard new_keyboard;
    try {
        saver.load("keyboard_state.json", &new_keyboard);
        cout << "Restored Bindings: ";
        for (const auto& pair : new_keyboard.get_key_bindings()) {
            cout << pair.first << " ";
        }
        cout << endl;
        
        // Тестируем восстановленные привязки
        cout << "Testing restored bindings..." << endl;
        new_keyboard.execute_command("a");
        new_keyboard.execute_command("ctrl++");
        new_keyboard.execute_command("ctrl+p");
        
        cout << "Test results:" << endl;
        for (const string& line : new_keyboard.get_output()) {
            cout << "  " << line << endl;
        }
        
    } catch (const exception& e) {
        cerr << "Error loading keyboard state: " << e.what() << endl;
    }
    
    return 0;
}
