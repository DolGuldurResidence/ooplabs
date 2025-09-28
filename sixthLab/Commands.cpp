#include "Commands.h"
#include "Keyboard.h"
#include <sstream>

using namespace std;

// PrintCommand implementation
PrintCommand::PrintCommand(Keyboard* receiver, char character) 
    : receiver(receiver), character(character) {
}

string PrintCommand::execute() {
    receiver->print_char(character);
    return receiver->get_text_buffer();
}

string PrintCommand::undo() {
    receiver->remove_last_char();
    return receiver->get_text_buffer();
}

string PrintCommand::redo() {
    return execute();
}

// VolumeUpCommand implementation
VolumeUpCommand::VolumeUpCommand(Keyboard* receiver, int step) 
    : receiver(receiver), step(step) {
}

string VolumeUpCommand::execute() {
    receiver->increase_volume(step);
    ostringstream oss;
    oss << "Volume increased +" << step << "%";
    return oss.str();
}

string VolumeUpCommand::undo() {
    receiver->decrease_volume(step);
    ostringstream oss;
    oss << "Volume decreased +" << step << "%";
    return oss.str();
}

string VolumeUpCommand::redo() {
    return execute();
}

// VolumeDownCommand implementation
VolumeDownCommand::VolumeDownCommand(Keyboard* receiver, int step) 
    : receiver(receiver), step(step) {
}

string VolumeDownCommand::execute() {
    receiver->decrease_volume(step);
    ostringstream oss;
    oss << "Volume decreased +" << step << "%";
    return oss.str();
}

string VolumeDownCommand::undo() {
    receiver->increase_volume(step);
    ostringstream oss;
    oss << "Volume increased +" << step << "%";
    return oss.str();
}

string VolumeDownCommand::redo() {
    return execute();
}

// MediaPlayerCommand implementation
MediaPlayerCommand::MediaPlayerCommand(Keyboard* receiver) 
    : receiver(receiver) {
}

string MediaPlayerCommand::execute() {
    receiver->launch_media_player();
    return "Media player launched";
}

string MediaPlayerCommand::undo() {
    receiver->close_media_player();
    return "Media player closed";
}

string MediaPlayerCommand::redo() {
    return execute();
}
