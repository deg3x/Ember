﻿#pragma once

#include <vector>

#include "Definitions.h"
#include "LoggerTypes.h"

class ENGINE_API Logger
{
public:
    // Add a timestamp to the log entry, to print in the console
    static std::vector<LogEntry> consoleLog;
    
public:
    static void Log(const LogEntry& entry, LogOutput output = (LogOutput)(LogOutput::STDOUT | LogOutput::CONSOLE));
    static void Log(LogCategory category, const std::string& message, const std::string& context, LogOutput output = (LogOutput)(LogOutput::STDOUT | LogOutput::CONSOLE));

    static void ClearConsoleLog();
    static std::vector<LogEntry> GetConsoleLog();
};
