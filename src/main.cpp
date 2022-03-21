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
#include <vector>
#include <string>
#include <limits>
#include <filesystem>

#include "C_console.hpp"
#include "C_error.hpp"
#include "C_string.hpp"
#include "memoryModule/C_MM1.hpp"
#include "motherboard/C_GCM_5_1.hpp"
#include "processor/C_ALUminium_1_1.hpp"
#include "peripheral/C_uart.hpp"

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
        fileLogOutPath = fileInPath;
        fileLogOutPath += ".log";
    }

    ///Opening files
    std::ifstream fileIn(fileInPath, std::ios::binary );
    if ( !fileIn )
    {
        std::cout << "Can't read the file " << fileInPath << std::endl;
        return -1;
    }

    codeg::varConsole = new codeg::Console();
    if (writeLogFile)
    {
        if ( !codeg::varConsole->logOpen(fileLogOutPath) )
        {
            std::cout << "Can't write the file " << fileLogOutPath << std::endl;
            return -1;
        }
    }

    std::cout << std::endl;

    std::string stringLine;

    try
    {
        ConsoleInfo << "Reading the file ..." << std::endl;

        auto* buffer = new uint8_t[0xFFFF];
        auto finalSize = fileIn.readsome(reinterpret_cast<char*>(buffer), 0xFFFF);

        ConsoleInfo << "Data size : " << finalSize << " bytes" << std::endl;

        ConsoleInfo << "Creating memory module size for the source ..." << std::endl;
        std::shared_ptr<codeg::MemoryModule> memory = std::make_shared<codeg::MM1_64k>();
        memory->set(0, buffer, finalSize);
        delete[] buffer;

        ConsoleInfo << "Creating the motherboard and plug the memory module ..." << std::endl;
        codeg::GCM_5_1_SPS1 motherboard;
        motherboard.memoryPlug(motherboard.getMemorySourceIndex(), memory);

        motherboard._processor._alu = std::make_shared<codeg::Aluminium_1_1>();
        motherboard._processor.memoryPlug(0, std::make_shared<codeg::MM1_16k>());
        motherboard.memoryPlug(1, std::make_shared<codeg::MM1_16k>());

        std::shared_ptr<codeg::UART_peripheral_card_A_1_1> uartCard = std::make_shared<codeg::UART_peripheral_card_A_1_1>();
        uartCard->setInputBuffer("test_hello\n");
        motherboard.peripheralPlug(0, uartCard);

        ConsoleInfo << "ok !" << std::endl;

        ConsoleInfo << "Waiting user input" << std::endl;

        std::string userCommand;
        do
        {
            std::cout << ">";
            std::getline(std::cin, userCommand);

            userCommand = codeg::RemoveExtraSpace(userCommand);
            std::vector<std::string> splitedUserCommand;
            codeg::Split(userCommand, splitedUserCommand, ' ');

            ConsoleInfo << "user command : \"" << userCommand << "\"" << std::endl;

            if (userCommand == "help")
            {
                std::cout << "\texit -> exit the application\n"
                          << "\tread pc -> read the program counter\n"
                          << "\tread mem [\"m\"/\"p\"] [slot] [address] -> read in a motherboard/processor memory slot at address\n"
                          << "\tread bus [name] -> read a specific bus value\n"
                          << "\tread bus -> read all bus value\n"
                          << "\texecute [cycle] -> execute a number of clock cycle (clock until sync)\n"
                          << "\tgoto [address] -> execute a number of clock cycle (clock until sync) until the address is reached (or max iterations)\n"
                          << "\treset -> do a hard reset\n"
                          << "\tflushUart -> clear the output buffer of the uart card and print the result\n";
            }
            else if (userCommand == "reset")
            {
                motherboard.hardReset();
                ConsoleInfo << "pc: "<< motherboard.getProgramCounter() <<" ("
                                     << codeg::ValueToHex(motherboard.getProgramCounter(), 8, true) << ")";
            }
            else if (userCommand == "flushUart")
            {
                if (uartCard->getOutputBuffer().empty())
                {
                    ConsoleWarning << "buffer is empty" << std::endl;
                }
                else
                {
                    ConsoleInfo << "buffer: \"" << uartCard->getOutputBuffer() << "\"" << std::endl;
                    uartCard->clearOutputBuffer();
                }
            }
            else if (splitedUserCommand.size() > 1)
            {
                if (splitedUserCommand[0] == "read")
                {
                    if (splitedUserCommand[1] == "pc")
                    {
                        if (splitedUserCommand.size() == 2)
                        {
                            ConsoleInfo << motherboard.getProgramCounter()
                                        << " ("<< codeg::ValueToHex(motherboard.getProgramCounter(), 8, true) <<")"
                                        << std::endl;
                        }
                        else
                        {
                            ConsoleError << "usage: read pc" << std::endl;
                        }
                    }
                    else if (splitedUserCommand[1] == "mem")
                    {
                        if (splitedUserCommand.size() == 5)
                        {
                            if (splitedUserCommand[2] == "m")
                            {
                                std::size_t slotValue = std::strtoul(splitedUserCommand[3].c_str(), nullptr, 0);

                                const codeg::MemoryModuleSlot* slot = motherboard.getMemorySlot(slotValue);
                                if (slot)
                                {
                                    if (slot->_mem)
                                    {
                                        codeg::MemoryAddress addressValue = std::strtoul(splitedUserCommand[4].c_str(), nullptr, 0);

                                        uint8_t data;
                                        if ( slot->_mem->get(addressValue, data) )
                                        {
                                            ConsoleInfo << codeg::ValueToHex(data, 2) << std::endl;
                                        }
                                        else
                                        {
                                            ConsoleError << "address out of range (max: "<< slot->_mem->getMemorySize() <<")" << std::endl;
                                        }
                                    }
                                    else
                                    {
                                        ConsoleError << "no memory plugged in this slot" << std::endl;
                                    }
                                }
                                else
                                {
                                    ConsoleError << "unknown slot " << splitedUserCommand[3] << std::endl;
                                }
                            }
                            else if (splitedUserCommand[2] == "p")
                            {
                                std::size_t slotValue = std::strtoul(splitedUserCommand[3].c_str(), nullptr, 0);

                                const codeg::MemoryModuleSlot* slot = motherboard._processor.getMemorySlot(slotValue);
                                if (slot)
                                {
                                    if (slot->_mem)
                                    {
                                        codeg::MemoryAddress addressValue = std::strtoul(splitedUserCommand[4].c_str(), nullptr, 0);

                                        uint8_t data;
                                        if ( slot->_mem->get(addressValue, data) )
                                        {
                                            ConsoleInfo << codeg::ValueToHex(data, 2) << std::endl;
                                        }
                                        else
                                        {
                                            ConsoleError << "address out of range (max: "<< slot->_mem->getMemorySize() <<")" << std::endl;
                                        }
                                    }
                                    else
                                    {
                                        ConsoleError << "no memory plugged in this slot" << std::endl;
                                    }
                                }
                                else
                                {
                                    ConsoleError << "unknown slot " << splitedUserCommand[3] << std::endl;
                                }
                            }
                            else
                            {
                                ConsoleError << "please put \"m\" (motherboard) or \"p\" (processor)" << std::endl;
                            }
                        }
                        else
                        {
                            ConsoleError << "usage: read mem [\"m\"/\"p\"] [slot] [address]" << std::endl;
                        }
                    }
                    else if (splitedUserCommand[1] == "bus")
                    {
                        if (splitedUserCommand.size() == 3)
                        {
                            if ( motherboard._processor._busses.exist(splitedUserCommand[2]) )
                            {
                                const codeg::Bus& bus = motherboard._processor._busses.get(splitedUserCommand[2]);
                                ConsoleInfo << "["<< splitedUserCommand[2] <<"] = "<< bus.get()
                                            <<" ("<< codeg::ValueToHex(bus.get(), 8, true) <<")" << std::endl;
                            }
                            else
                            {
                                ConsoleError << "bus ["<< splitedUserCommand[2] <<"] doesn't exist" << std::endl;
                            }
                        }
                        else if (splitedUserCommand.size() == 2)
                        {
                            for (const auto& bus : motherboard._processor._busses)
                            {
                                ConsoleInfo << "["<< bus.first <<"] = "<< bus.second.get()
                                            <<" ("<< codeg::ValueToHex(bus.second.get(), 8, true) <<")" << std::endl;
                            }
                        }
                        else
                        {
                            ConsoleError << "usage: read bus ([name])" << std::endl;
                        }
                    }
                    else
                    {
                        ConsoleError << "usage: read [\"pc\"/\"mem\"/\"bus\"] ..." << std::endl;
                    }
                }
                else if (splitedUserCommand[0] == "execute")
                {
                    if (splitedUserCommand.size() == 2)
                    {
                        std::size_t clockCycle = std::strtoul(splitedUserCommand[1].c_str(), nullptr, 0);

                        for (std::size_t i=0; i<clockCycle; ++i)
                        {
                            if ( !motherboard._processor.clockUntilSync(20) )
                            {
                                ConsoleError << "max iteration reached !" << std::endl;
                                break;
                            }
                        }
                        ConsoleInfo << "pc: "<< motherboard.getProgramCounter()
                                             <<" ("<< codeg::ValueToHex(motherboard.getProgramCounter(), 8, true) <<")"
                                             << std::endl;
                    }
                    else
                    {
                        ConsoleError << "usage: execute [clock cycle]" << std::endl;
                    }
                }
                else if (splitedUserCommand[0] == "goto")
                {
                    if (splitedUserCommand.size() == 2)
                    {
                        codeg::MemoryAddress memoryAddress = std::strtoul(splitedUserCommand[1].c_str(), nullptr, 0);
                        bool reached = false;

                        for (std::size_t i=0; i<5000; ++i)
                        {
                            if (motherboard.getProgramCounter() == memoryAddress)
                            {
                                ConsoleInfo << "memory reached !" << std::endl;
                                reached = true;
                                break;
                            }

                            if ( !motherboard._processor.clockUntilSync(20) )
                            {
                                ConsoleError << "clock cycle: max iteration reached !" << std::endl;
                                break;
                            }
                        }
                        if (!reached)
                        {
                            ConsoleError << "max iteration reached !" << std::endl;
                            ConsoleInfo << "pc: "<< motherboard.getProgramCounter()
                                        <<" ("<< codeg::ValueToHex(motherboard.getProgramCounter(), 8, true) <<")"
                                        << std::endl;
                        }
                    }
                    else
                    {
                        ConsoleError << "usage: goto [address]" << std::endl;
                    }
                }
            }
            else
            {
                ConsoleWarning << "unknown command or exit!" << std::endl;
            }
        }
        while (userCommand != "exit");
    }
    catch (const codeg::Error& e)
    {
        ConsoleError << "error : " <<  e.what() << std::endl;
        return -1;
    }
    catch (const std::exception& e)
    {
        ConsoleFatal << "unknown exception : " << e.what() << std::endl;
        return -1;
    }

    codeg::varConsole->logClose();
    delete codeg::varConsole;

    return 0;
}
