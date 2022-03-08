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

#include "main.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <vector>
#include <string>
#include <limits>
#include <filesystem>

#include "C_target.hpp"
#include "C_console.hpp"
#include "C_error.hpp"
#include "C_string.hpp"
#include "memoryModule/C_MM1.hpp"

#include "CMakeConfig.hpp"

namespace fs = std::filesystem;

void printHelp()
{
    std::cout << "codeGSimulator usage :" << std::endl << std::endl;

    std::cout << "Set the input file to be compiled" << std::endl;
    std::cout << "\tcodeGSimulator --in=<path>" << std::endl << std::endl;

    std::cout << "Set the output log file (default is the input path+.log)" << std::endl;
    std::cout << "\tcodeGSimulator --outLog=<path>" << std::endl << std::endl;

    std::cout << "Don't write a log file (default a log file is writed)" << std::endl;
    std::cout << "\tcodeGSimulator --noLog" << std::endl << std::endl;

    std::cout << "Print the version (and do nothing else)" << std::endl;
    std::cout << "\tcodeGSimulator --version" << std::endl << std::endl;

    std::cout << "Print the help page (and do nothing else)" << std::endl;
    std::cout << "\tcodeGSimulator --help" << std::endl << std::endl;
}
void printVersion()
{
    std::cout << "codeGSimulator created by Guillaume Guillet, version " << CGS_VERSION_MAJOR << "." << CGS_VERSION_MINOR << std::endl;
}

int main(int argc, char **argv)
{
    if ( int err = codeg::ConsoleInit() )
    {
        std::cout << "Warning, bad console init, the console can be ugly now ! (error: "<<err<<")" << std::endl;
    }

    fs::path fileInPath;
    fs::path fileLogOutPath;

    std::vector<std::string> commands(argv, argv + argc);

    if (commands.size() <= 1)
    {
        printHelp();
        return -1;
    }

    bool writeLogFile = true;

    for (std::size_t i=1; i<commands.size(); ++i)
    {
        //Commands
        if ( commands[i] == "--help")
        {
            printHelp();
            return 0;
        }
        if ( commands[i] == "--version")
        {
            printVersion();
            return 0;
        }
        if ( commands[i] == "--noLog")
        {
            writeLogFile = false;
            continue;
        }

        //Commands with an argument
        std::vector<std::string> splitedCommand;
        codeg::Split(commands[i], splitedCommand, '=');

        if (splitedCommand.size() == 2)
        {
            if ( splitedCommand[0] == "--in")
            {
                fileInPath = splitedCommand[1];
                continue;
            }
            if ( splitedCommand[0] == "--outLog")
            {
                fileLogOutPath = splitedCommand[1];
                continue;
            }
        }

        //Unknown command
        std::cout << "Unknown command : \""<< commands[i] <<"\" !" << std::endl;
        return -1;
    }

    if ( fileInPath.empty() )
    {
        std::cout << "No input file !" << std::endl;
        return -1;
    }
    if (fileLogOutPath.empty() && writeLogFile )
    {
        fileLogOutPath = fileInPath += ".log";
    }

    ///Opening files
    std::ifstream fileIn(fileInPath, std::ios::binary );
    if ( !fileIn )
    {
        std::cout << "Can't read the file \"" << fileInPath << "\"" << std::endl;
        return -1;
    }

    if (writeLogFile)
    {
        if ( !codeg::LogOpen(fileLogOutPath) )
        {
            std::cout << "Can't write the file \"" << fileLogOutPath << "\"" << std::endl;
            return -1;
        }
    }

    std::cout << std::endl;

    std::string readedLine;

    try
    {
        ///First step reading and compiling
        codeg::ConsoleInfoWrite("Reading the file ...");

        /*while( data._reader.getline(readedLine) )
        {

        }*/
    }
    catch (const std::exception& e)
    {
        codeg::ConsoleFatalWrite("unknown exception : "+std::string(e.what()));
        return -1;
    }

    codeg::LogClose();

    return 0;
}
