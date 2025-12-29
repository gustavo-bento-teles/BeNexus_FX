#pragma once
#include <Arduino.h>
#include "output/OutputManager.h"

enum class Command {
    SET,
    GET,
    UNKNOWN
};

enum class Param {
    LANTERN,
    UNKNOWN
};

struct ParsedCommand {
    Command cmd;
    Param param;
    String value;
};

enum class CommandError {
    INVALID_COMMAND,
    INVALID_PARAM,
    INVALID_VALUE,
    MISSING_VALUE
};

class CommandService {
public:
    CommandService(OutputManager* outputMg) : outputManager(outputMg) {}

    String execCmd(const String& line);

private:
    bool isNumeric(const String& s);

    String commandError(CommandError err);

    ParsedCommand parse(const String& line);
    String dispatch(const ParsedCommand& cmd);

    OutputManager* outputManager;
};