## Command

Command — это поведенческий паттерн проектирования, который инкапсулирует запрос в виде объекта, позволяя параметризовать клиенты с различными запросами, организовывать очередь запросов или поддерживать отмену операций

**Паттерн Command** — это когда каждая операция (действие) становится отдельным объектом.

## Простая аналогия:
Представь, что команда — это кнопка на пульте:
- **Кнопка** — это объект команды
- **Телевизор** — это объект, который выполняет действие
- **Ты** — это тот, кто нажимает кнопку



## В этой программе:

**Команды** — это разные действия:
- `Напечатать букву 'а'`
- `Увеличить громкость` 
- `Включить/выключить плеер`

**Клавиатура** — это пульт, где:
- Клавишам назначены команды
- Есть кнопки **Отмена** и **Повтор**

**Редактор** — это телевизор, который реально выполняет действия

## Главная фишка:
Клавиатура не знает, КАК выполнять команды, она только знает, КОГДА их выполнять. А команды сами знают, что делать.


# Что делают классы
1. Editor (Редактор) - Реальный исполнитель всех действий
2. Command (Команда) - абстрактный класс.Определяет интерфейс для всех команд
3. Конкретные команды:
    * KeyCommand - Печатает символ в редакторе
    * VolumeUpCommand - Увеличивает громкость
    * VolumeDownCommand - Уменьшает громкость
    * MediaPlayerCommand - Включает/выключает медиаплеер
4. Keyboard (Клавиатура) - Управляет командами и горячими клавишами
5. CommandFactory (Фабрика команд) - Создает команды по типу и параметрам
6. KeyboardStateSaver (Сохранение состояния) - Сохраняет и загружает привязки клавиш

---

Основные связи:

Command - абстрактный базовый класс для всех команд

Конкретные команды наследуются от Command и работают с Editor

Keyboard хранит команды в map и управляет стеками undo/redo

CommandFactory создает команды по типу и параметрам

KeyboardStateSaver сохраняет/загружает состояние привязок клавиш

---

```mermaid
classDiagram
    class Command {
        <<abstract>>
        +execute() void
        +undo() void
        +get_type() string
        +get_params() json
    }

    class KeyCommand {
        -editor: Editor&
        -character: char
        +execute()
        +undo()
        +get_type()
        +get_params()
    }

    class VolumeUpCommand {
        -editor: Editor&
        -step: int
        +execute()
        +undo()
        +get_type()
        +get_params()
    }

    class VolumeDownCommand {
        -editor: Editor&
        -step: int
        +execute()
        +undo()
        +get_type()
        +get_params()
    }

    class MediaPlayerCommand {
        -editor: Editor&
        +execute()
        +undo()
        +get_type()
        +get_params()
    }

    class Editor {
        -text: string
        -volume: int
        -media_playing: bool
        -log_file: ofstream
        +print_char(char)
        +delete_last_char()
        +increase_volume(int)
        +decrease_volume(int)
        +toggle_media_player()
    }

    class Keyboard {
        -key_commands: map~string,Command*~
        -undo_stack: stack~Command*~
        -redo_stack: stack~Command*~
        -editor: Editor&
        +bind_key(string, Command*)
        +press_key(string)
        +undo()
        +redo()
        +get_state() json
        +set_state(json)
    }

    class CommandFactory {
        +create_command(string, json, Editor&) Command*
    }

    class KeyboardStateSaver {
        +save(Keyboard&, string)
        +load(Keyboard&, string)
    }

    Command <|-- KeyCommand
    Command <|-- VolumeUpCommand
    Command <|-- VolumeDownCommand
    Command <|-- MediaPlayerCommand

    Keyboard *-- Command : агрегация
    Keyboard --> Editor : использует
    KeyCommand --> Editor : использует
    VolumeUpCommand --> Editor : использует
    VolumeDownCommand --> Editor : использует
    MediaPlayerCommand --> Editor : использует
    CommandFactory ..> Command : создает
    KeyboardStateSaver --> Keyboard : работает с
```


