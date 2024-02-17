#include "DebugCpp.h"
#include<stdio.h>
#include <string>
#include <stdio.h>
#include <sstream>

Debug* Debug::debugInst = nullptr;

void Debug::LogNoNewLine(const char* message) {
    debugInst->debugLock.lock();
    debugInst->ss << message;
    debugInst->debugLock.unlock();
}

void Debug::Log(const char* message) {
    debugInst->debugLock.lock();
    debugInst->ss << message;
    debugInst->ss << std::endl;
    debugInst->debugLock.unlock();
}

void Debug::Log(const std::string message) {
    debugInst->debugLock.lock();
    debugInst->ss << message;
    debugInst->ss << std::endl;
    debugInst->debugLock.unlock();
}

void Debug::Log(const int message) {
    debugInst->debugLock.lock();
    debugInst->ss << message;
    debugInst->ss << std::endl;
    debugInst->debugLock.unlock();
}

void Debug::Log(const unsigned int message) {
    debugInst->debugLock.lock();
    debugInst->ss << message;
    debugInst->ss << std::endl;
    debugInst->debugLock.unlock();
}

void Debug::Log(const char message) {
    debugInst->debugLock.lock();
    debugInst->ss << message;
    debugInst->ss << std::endl;
    debugInst->debugLock.unlock();
}

void Debug::Log(const float message) {
    debugInst->debugLock.lock();
    debugInst->ss << message;
    debugInst->ss << std::endl;
    debugInst->debugLock.unlock();
}

void Debug::Log(const double message) {
    debugInst->debugLock.lock();
    debugInst->ss << message;
    debugInst->ss << std::endl;
    debugInst->debugLock.unlock();
}

void Debug::Log(const bool message) {
    debugInst->debugLock.lock();
    if (message)
        debugInst->ss << "true";
    else
        debugInst->ss << "false";
    debugInst->ss << std::endl;
    debugInst->debugLock.unlock();
}

void Debug::InitiateDebug() {
    debugInst = new Debug();
    debugInst->ss = std::stringstream();
}

void Debug::ReleaseDebug() {
    delete debugInst;
}

int Debug::GetDebugLog(char* message, int maxLength) {
    debugInst->debugLock.lock();
    const std::string& temp = debugInst->ss.str();
    if (temp.length() < maxLength)
    {
        maxLength = temp.length();
    }
    if (maxLength > 0) {
        memcpy(message, temp.c_str(), maxLength);
    }
    debugInst->debugLock.unlock();
    return maxLength;
}

void Debug::ClearDebugLog() {
    debugInst->debugLock.lock();
    debugInst->ss.str("");
    debugInst->ss.clear();
    debugInst->debugLock.unlock();
}