#include "CommandFactory.h"
#include "Keyboard.h"
#include "Commands.h"
#include <stdexcept>
#include <sstream>

using namespace std;

void CommandFactory::register_command(const string& command_type, 
                                     function<shared_ptr<Command>(Keyboard*, const map<string, string>&)> creator,
                                     function<map<string, string>(Command*)> serializer) {
    creators[command_type] = creator;
    serializers[command_type] = serializer;
}

shared_ptr<Command> CommandFactory::create_command(const string& command_type, 
                                                       Keyboard* receiver, 
                                                       const map<string, string>& params) {
    auto it = creators.find(command_type);
    if (it == creators.end()) {
        throw runtime_error("Unknown command type: " + command_type);
    }
    return it->second(receiver, params);
}

map<string, string> CommandFactory::serialize_command(Command* command) {
    string command_type = get_command_type(command);
    auto it = serializers.find(command_type);
    if (it == serializers.end()) {
        throw runtime_error("Unregistered command class: " + command_type);
    }
    return it->second(command);
}

string CommandFactory::get_command_type(Command* command) {
    // Используем dynamic_cast для определения типа
    if (dynamic_cast<PrintCommand*>(command)) {
        return "PrintCommand";
    } else if (dynamic_cast<VolumeUpCommand*>(command)) {
        return "VolumeUpCommand";
    } else if (dynamic_cast<VolumeDownCommand*>(command)) {
        return "VolumeDownCommand";
    } else if (dynamic_cast<MediaPlayerCommand*>(command)) {
        return "MediaPlayerCommand";
    }
    return "Unknown";
}
