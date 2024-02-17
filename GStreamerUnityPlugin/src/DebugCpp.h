#pragma once

#include<stdio.h>
#include <string>
#include <stdio.h>
#include <sstream>
#include <mutex>

// An utility to feed back logs to unity.
class Debug
{
public:
    static void LogNoNewLine(const char* message);
    static void Log(const char* message);
    static void Log(const std::string message);
    static void Log(const int message);
    static void Log(const unsigned int message);
    static void Log(const char message);
    static void Log(const float message);
    static void Log(const double message);
    static void Log(const bool message);

    static void InitiateDebug();
    static void ReleaseDebug();
    static int GetDebugLog(char* message, int maxLength);
    static void ClearDebugLog();



private:
    static Debug* debugInst;
    std::stringstream ss;
    std::mutex debugLock;
};