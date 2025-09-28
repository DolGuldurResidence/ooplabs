#ifndef KEYBOARDSTATESAVER_H
#define KEYBOARDSTATESAVER_H

#include <string>
#include "CommandFactory.h"
#include "Keyboard.h"

// Класс для сохранения и загрузки состояния клавиатуры (паттерн Memento)
class KeyboardStateSaver {
private:
    CommandFactory* factory;

public:
    KeyboardStateSaver(CommandFactory* factory);
    
    // Сохранение состояния клавиатуры в JSON файл
    void save(const std::string& filename, Keyboard* keyboard);
    
    // Загрузка состояния клавиатуры из JSON файла
    void load(const std::string& filename, Keyboard* keyboard);
};

#endif // KEYBOARDSTATESAVER_H
