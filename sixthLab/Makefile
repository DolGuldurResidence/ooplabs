# Makefile для виртуальной клавиатуры
# Компилятор
CXX = g++

# Флаги компиляции
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Имя исполняемого файла
TARGET = virtual_keyboard

# Исходные файлы
SOURCES = main.cpp Keyboard.cpp Commands.cpp CommandFactory.cpp KeyboardStateSaver.cpp

# Объектные файлы
OBJECTS = $(SOURCES:.cpp=.o)

# Заголовочные файлы
HEADERS = Command.h Keyboard.h Commands.h CommandFactory.h KeyboardStateSaver.h

# Основная цель
all: $(TARGET)

# Создание исполняемого файла
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Компиляция объектных файлов
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка
clean:
	rm -f $(OBJECTS) $(TARGET) keyboard_state.json output.txt

# Запуск программы
run: $(TARGET)
	./$(TARGET)

# Отладочная версия
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Установка зависимостей (если нужно)
install:
	@echo "No external dependencies required"

# Помощь
help:
	@echo "Доступные команды:"
	@echo "  make          - Скомпилировать программу"
	@echo "  make run      - Скомпилировать и запустить"
	@echo "  make debug    - Скомпилировать отладочную версию"
	@echo "  make clean    - Удалить скомпилированные файлы"
	@echo "  make help     - Показать эту справку"

.PHONY: all clean run debug install help

