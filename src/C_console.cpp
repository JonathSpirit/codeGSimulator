/////////////////////////////////////////////////////////////////////////////////
// Copyright 2022 Guillaume Guillet                                            //
//                                                                             //
// Licensed under the Apache License, Version 2.0 (the "License");             //
// you may not use this file except in compliance with the License.            //
// You may obtain a copy of the License at                                     //
//                                                                             //
//     http://www.apache.org/licenses/LICENSE-2.0                              //
//                                                                             //
// Unless required by applicable law or agreed to in writing, software         //
// distributed under the License is distributed on an "AS IS" BASIS,           //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    //
// See the License for the specific language governing permissions and         //
// limitations under the License.                                              //
/////////////////////////////////////////////////////////////////////////////////

#include "C_console.hpp"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <fstream>

#ifdef _WIN32
    #include <windows.h>

    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#endif

namespace codeg
{

std::ofstream _fileLog;

int ConsoleInit()
{
    #ifdef _WIN32
    ///WINDOWS

    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return GetLastError();
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return GetLastError();
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return GetLastError();
    }
    #endif

    return 0;
}

bool LogOpen(const std::filesystem::path& path)
{
    if ( _fileLog.is_open() )
    {
        return false;
    }

    _fileLog.open(path, std::ofstream::ate);
    if ( _fileLog )
    {
        return true;
    }

    _fileLog.close();
    return false;
}
void LogClose()
{
    _fileLog.close();
}

void ConsoleWrite(codeg::ConsoleOutputType type, const char* str)
{
    std::time_t t = std::time(nullptr);
    auto tData = std::put_time(std::localtime(&t), "%d.%m.%Y - %H:%M:%S");

    switch (type)
    {
    case CONSOLE_TYPE_INFO:
        std::cout << "[info](" << tData << ") " << str << std::endl;
        if (_fileLog)
        {
            _fileLog << "[info](" << tData << ") " << str << std::endl;
        }
        break;
    case CONSOLE_TYPE_ERROR:
        std::cout << "\x1b[31m";
        std::cout << "[error](" << tData << ") " << str << std::endl;
        std::cout << "\x1b[0m";
        if (_fileLog)
        {
            _fileLog << "[error](" << tData << ") " << str << std::endl;
        }
        break;
    case CONSOLE_TYPE_FATAL:
        std::cout << "\x1b[31m";
        std::cout << "[fatal](" << tData << ") " << str << std::endl;
        std::cout << "\x1b[0m";
        if (_fileLog)
        {
            _fileLog << "[error](" << tData << ") " << str << std::endl;
        }
        break;
    case CONSOLE_TYPE_WARNING:
        std::cout << "\x1b[36m";
        std::cout << "[warning](" << tData << ") " << str << std::endl;
        std::cout << "\x1b[0m";
        if (_fileLog)
        {
            _fileLog << "[warning](" << tData << ") " << str << std::endl;
        }
        break;
    case CONSOLE_TYPE_SYNTAX:
        std::cout << "\x1b[33m";
        std::cout << "[syntax error](" << tData << ") " << str << std::endl;
        std::cout << "\x1b[0m";
        if (_fileLog)
        {
            _fileLog << "[syntax error](" << tData << ") " << str << std::endl;
        }
        break;
    default:
        std::cout << str << '\n' << "\x1b[0m" << std::flush;
        if (_fileLog)
        {
            _fileLog << str << std::endl;
        }
        break;
    }
}

}//end codeg
