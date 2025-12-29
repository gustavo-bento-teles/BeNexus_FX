#include "CommandService.h"

String CommandService::execCmd(const String& line) {
    ParsedCommand cmd = parse(line);
    return dispatch(cmd);
}

ParsedCommand CommandService::parse(const String& line) {
    String cmdStr, paramStr, valueStr;

    String parts[3];
    int index = 0;
    int last = 0;

    for (int i = 0; i <= line.length(); i++) {
        if (line[i] == ' ' || i == line.length()) {
            parts[index++] = line.substring(last, i);
            last = i + 1;
            if (index == 3) break;
        }
    }

    cmdStr = parts[0];
    paramStr = parts[1];
    valueStr = parts[2];

    ParsedCommand result;

    if (cmdStr == "set") result.cmd = Command::SET;
    else if (cmdStr == "get") result.cmd = Command::GET;
    else result.cmd = Command::UNKNOWN;

    if (paramStr == "lant") result.param = Param::LANTERN;
    else result.param = Param::UNKNOWN;

    result.value = valueStr;

    return result;
}

String CommandService::dispatch(const ParsedCommand& cmd) {
    if (cmd.cmd == Command::UNKNOWN)
        return commandError(CommandError::INVALID_COMMAND);

    if (cmd.param == Param::UNKNOWN)
        return commandError(CommandError::INVALID_PARAM);

    if (cmd.value.length() == 0 && cmd.cmd != Command::GET)
        return commandError(CommandError::MISSING_VALUE);


    switch (cmd.cmd) {
        case Command::SET:
            switch (cmd.param) {
                case Param::LANTERN:
                    if (isNumeric(cmd.value)) {
                        int v = cmd.value.toInt();
                        if (v == 0) {
                            outputManager->setLantern(false);
                            return "Lanterna OFF";
                        } else if (v == 1) {
                            outputManager->setLantern(true);
                            return "Lanterna ON";
                        } else {
                            return commandError(CommandError::INVALID_VALUE);
                        }
                    } else {
                        return commandError(CommandError::INVALID_VALUE);
                    }
                    break;
            }
            break;

        case Command::GET:
            switch (cmd.param) {
                case Param::LANTERN:
                    if (outputManager->isLanternOn())
                        return "Lanterna: true";
                    else
                        return "Lanterna: false";
                    break;
            }
            break;
    }

    return commandError(CommandError::INVALID_COMMAND);
}

bool CommandService::isNumeric(const String& s) {
    if (s.length() == 0) return false;

    for (unsigned int i = 0; i < s.length(); i++) {
        if (!isDigit(s[i])) return false;
    }
    return true;
}

String CommandService::commandError(CommandError err) {
    switch (err) {
        case CommandError::INVALID_COMMAND:
            return "Cmd invalido!";
        
        case CommandError::INVALID_PARAM:
            return "Param invalido!";
        
        case CommandError::INVALID_VALUE:
            return "Value invalido!";
        
        case CommandError::MISSING_VALUE:
            return "Nenhum valor passado";
        
        default:
            return "Erro desconhecido";
    }
}