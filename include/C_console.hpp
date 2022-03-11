/////////////////////////////////////////////////////////////////////////////////
// Copyright 2021 Guillaume Guillet                                            //
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

#ifndef C_CONSOLE_H_INCLUDED
#define C_CONSOLE_H_INCLUDED

#include <string>
#include <ostream>
#include <iostream>
#include <filesystem>

namespace codeg
{

int ConsoleInit();

bool LogOpen(const std::filesystem::path& path, std::ofstream& file);

enum ConsoleOutputType
{
    CONSOLE_TYPE_NONE,

    CONSOLE_TYPE_INFO,
    CONSOLE_TYPE_ERROR,
    CONSOLE_TYPE_FATAL,
    CONSOLE_TYPE_WARNING,
    CONSOLE_TYPE_SYNTAX
};

void ConsoleWrite(std::ostream& log, codeg::ConsoleOutputType type, const char* str);
template<typename T, typename... TArgs>
void ConsoleWrite(std::ostream& log, codeg::ConsoleOutputType type, const char* str, T value, TArgs... args)
{
    std::time_t t = std::time(nullptr);
    auto tData = std::put_time(std::localtime(&t), "%d.%m.%Y - %H:%M:%S");

    switch (type)
    {
    case CONSOLE_TYPE_INFO:
        std::cout << "[info](" << tData << ") ";
        if (log)
        {
            log << "[info](" << tData << ") ";
        }
        break;
    case CONSOLE_TYPE_ERROR:
        std::cout << "\x1b[31m";
        std::cout << "[error](" << tData << ") ";
        if (log)
        {
            log << "[error](" << tData << ") ";
        }
        break;
    case CONSOLE_TYPE_FATAL:
        std::cout << "\x1b[31m";
        std::cout << "[fatal](" << tData << ") ";
        if (log)
        {
            log << "[error](" << tData << ") ";
        }
        break;
    case CONSOLE_TYPE_WARNING:
        std::cout << "\x1b[36m";
        std::cout << "[warning](" << tData << ") ";
        if (log)
        {
            log << "[warning](" << tData << ") ";
        }
        break;
    case CONSOLE_TYPE_SYNTAX:
        std::cout << "\x1b[33m";
        std::cout << "[syntax error](" << tData << ") ";
        if (log)
        {
            log << "[syntax error](" << tData << ") ";
        }
        break;
    default:
        break;
    }

    while (*str != '\0')
    {
        if (*str == '%')
        {
            std::cout << value;
            if (log)
            {
                log << value;
            }

            codeg::ConsoleWrite(log, codeg::CONSOLE_TYPE_NONE, str + 1, args...);
            return;
        }
        std::cout << *str;
        if (log)
        {
            log << *str;
        }

        ++str;
    }

    std::cout << "\x1b[0m" << std::endl;

    std::cout.flush();
    log.flush();
}

inline void ConsoleFatalWrite(std::ostream& log, const char* str)
{
    codeg::ConsoleWrite(log, codeg::CONSOLE_TYPE_FATAL, str);
}
template<typename T, typename... TArgs>
inline void ConsoleFatalWrite(std::ostream& log, const char* str, T value, TArgs... args)
{
    codeg::ConsoleWrite(log, codeg::CONSOLE_TYPE_FATAL, str, value, args...);
}

inline void ConsoleErrorWrite(std::ostream& log, const char* str)
{
    codeg::ConsoleWrite(log, codeg::CONSOLE_TYPE_ERROR, str);
}
template<typename T, typename... TArgs>
inline void ConsoleErrorWrite(std::ostream& log, const char* str, T value, TArgs... args)
{
    codeg::ConsoleWrite(log, codeg::CONSOLE_TYPE_ERROR, str, value, args...);
}

inline void ConsoleWarningWrite(std::ostream& log, const char* str)
{
    codeg::ConsoleWrite(log, codeg::CONSOLE_TYPE_WARNING, str);
}
template<typename T, typename... TArgs>
inline void ConsoleWarningWrite(std::ostream& log, const char* str, T value, TArgs... args)
{
    codeg::ConsoleWrite(log, codeg::CONSOLE_TYPE_WARNING, str, value, args...);
}

inline void ConsoleInfoWrite(std::ostream& log, const char* str)
{
    codeg::ConsoleWrite(log, codeg::CONSOLE_TYPE_INFO, str);
}
template<typename T, typename... TArgs>
inline void ConsoleInfoWrite(std::ostream& log, const char* str, T value, TArgs... args)
{
    codeg::ConsoleWrite(log, codeg::CONSOLE_TYPE_INFO, str, value, args...);
}

inline void ConsoleSyntaxWrite(std::ostream& log, const char* str)
{
    codeg::ConsoleWrite(log, codeg::CONSOLE_TYPE_SYNTAX, str);
}
template<typename T, typename... TArgs>
inline void ConsoleSyntaxWrite(std::ostream& log, const char* str, T value, TArgs... args)
{
    codeg::ConsoleWrite(log, codeg::CONSOLE_TYPE_SYNTAX, str, value, args...);
}

}//end codeg

#endif // C_CONSOLE_H_INCLUDED
