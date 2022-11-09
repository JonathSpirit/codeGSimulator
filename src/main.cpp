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

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

namespace fs = std::filesystem;

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

    codeg::RegisterNewMemoryModuleType(std::make_unique<codeg::MemoryModuleClassType<codeg::MM1> >());

    fs::path fileInPath;
    fs::path fileLogOutPath;
    bool writeLogFile = true;

    CLI::App app{"A simulator specifically built for the homemade language codeG", "codeGSimulator"};

    app.add_flag_callback("--version", [](){
        printVersion();
        throw CLI::Success{};
    }, "Print the version (and do nothing else)");

    app.add_flag("!--noLog", writeLogFile, "Don't write a log file (default a log file is written)");

    app.add_option("--in", fileInPath, "Set the input file to be read and simulated")->required(true);
    app.add_option("--outLog", fileLogOutPath, "Set the output log file (default is the input path+.log)");

    try
    {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e)
    {
        return app.exit(e);
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
    std::ifstream fileIn(fileInPath, std::ios::binary);
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

    bool running = true;

    struct Command
    {
        std::string _name;
        std::string _usage;
        std::string _description;
        std::size_t _minArguments;
        std::size_t _maxArguments;
        std::function<bool(const std::vector<std::string>&)> _func;
    };

    std::vector<std::shared_ptr<codeg::MemoryModule> > unpluggedMemories;

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

        std::vector<Command> commands = {
            {"exit", "exit", "exit the application", 0,0, [&]([[maybe_unused]] const std::vector<std::string>& args){
                running = false;
                return true;
            }},
            {"reset", "reset", "do a hard reset on motherboard", 0,0, [&]([[maybe_unused]] const std::vector<std::string>& args){
                motherboard.hardReset();
                ConsoleInfo << "pc: "<< motherboard.getProgramCounter() <<" ("
                            << codeg::ValueToHex(motherboard.getProgramCounter(), 8, true) << ")" << std::endl;
                return true;
            }},
            {"flushUart", "flushUart", "clear the output buffer of the uart card and print the result", 0,0, [&]([[maybe_unused]] const std::vector<std::string>& args){
                if (uartCard->getOutputBuffer().empty())
                {
                    ConsoleWarning << "buffer is empty" << std::endl;
                }
                else
                {
                    ConsoleInfo << "buffer: \"" << uartCard->getOutputBuffer() << "\"" << std::endl;
                    uartCard->clearOutputBuffer();
                }
                return true;
            }},
            {"read_pc", "read_pc", "read the program counter", 0,0, [&]([[maybe_unused]] const std::vector<std::string>& args){
                ConsoleInfo << motherboard.getProgramCounter()
                            << " ("<< codeg::ValueToHex(motherboard.getProgramCounter(), 8, true) <<")"
                            << std::endl;
                return true;
            }},
            {"read_mem", R"(read_mem ["m"/"p"] [slot] [address])", "read in a motherboard/processor memory slot", 3,3, [&]([[maybe_unused]] const std::vector<std::string>& args){
                if (args[0] == "m")
                {
                    std::size_t slotValue = std::strtoul(args[1].c_str(), nullptr, 0);

                    const codeg::MemoryModuleSlot* slot = motherboard.getMemorySlot(slotValue);
                    if (slot)
                    {
                        if (slot->_mem)
                        {
                            codeg::MemoryAddress addressValue = std::strtoul(args[2].c_str(), nullptr, 0);

                            uint8_t data;
                            if ( slot->_mem->get(addressValue, data) )
                            {
                                ConsoleInfo << codeg::ValueToHex(data, 2) << std::endl;
                            }
                            else
                            {
                                ConsoleError << "address out of range (max: "<< slot->_mem->getMemorySize() <<")" << std::endl;
                                return false;
                            }
                        }
                        else
                        {
                            ConsoleError << "no memory plugged in this slot" << std::endl;
                            return false;
                        }
                    }
                    else
                    {
                        ConsoleError << "unknown slot " << args[1] << std::endl;
                        return false;
                    }
                }
                else if (args[0] == "p")
                {
                    std::size_t slotValue = std::strtoul(args[1].c_str(), nullptr, 0);

                    const codeg::MemoryModuleSlot* slot = motherboard._processor.getMemorySlot(slotValue);
                    if (slot)
                    {
                        if (slot->_mem)
                        {
                            codeg::MemoryAddress addressValue = std::strtoul(args[2].c_str(), nullptr, 0);

                            uint8_t data;
                            if ( slot->_mem->get(addressValue, data) )
                            {
                                ConsoleInfo << codeg::ValueToHex(data, 2) << std::endl;
                            }
                            else
                            {
                                ConsoleError << "address out of range (max: "<< slot->_mem->getMemorySize() <<")" << std::endl;
                                return false;
                            }
                        }
                        else
                        {
                            ConsoleError << "no memory plugged in this slot" << std::endl;
                            return false;
                        }
                    }
                    else
                    {
                        ConsoleError << "unknown slot " << args[1] << std::endl;
                        return false;
                    }
                }
                else
                {
                    ConsoleError << R"(please put "m" (motherboard) or "p" (processor))" << std::endl;
                    return false;
                }
                return true;
            }},
            {"read_bus", "read_bus ([name])", "read a specific bus value, or all of them", 0,1, [&]([[maybe_unused]] const std::vector<std::string>& args){
                if (args.size() == 1)
                {
                    if ( motherboard._processor._busses.exist(args[0]) )
                    {
                        const codeg::Bus& bus = motherboard._processor._busses.get(args[0]);
                        ConsoleInfo << "["<< args[0] <<"] = "<< bus.get()
                                    <<" ("<< codeg::ValueToHex(bus.get(), 8, true) <<")" << std::endl;
                    }
                    else
                    {
                        ConsoleError << "bus ["<< args[0] <<"] doesn't exist" << std::endl;
                        return false;
                    }
                }
                else
                {
                    for (const auto& bus : motherboard._processor._busses)
                    {
                        ConsoleInfo << "["<< bus.first <<"] = "<< bus.second.get()
                                    <<" ("<< codeg::ValueToHex(bus.second.get(), 8, true) <<")" << std::endl;
                    }
                }
                return true;
            }},
            {"unplug", R"(unplug ["m"/"p"] [slot])", "unplug a memory module on the motherboard/processor", 2,2, [&]([[maybe_unused]] const std::vector<std::string>& args){
                if (args[0] == "m")
                {
                    std::size_t slotValue = std::strtoul(args[1].c_str(), nullptr, 0);

                    auto memory = motherboard.memoryUnplug(slotValue);

                    if (memory)
                    {
                        unpluggedMemories.push_back(std::move(memory));
                        ConsoleInfo << "correctly unplugged the memory module from the slot: " << slotValue << std::endl;
                    }
                    else
                    {
                        ConsoleError << "can't unplug the memory module from the slot: " << slotValue << std::endl;
                        return false;
                    }
                }
                else if (args[0] == "p")
                {
                    std::size_t slotValue = std::strtoul(args[1].c_str(), nullptr, 0);

                    auto memory = motherboard._processor.memoryUnplug(slotValue);

                    if (memory)
                    {
                        unpluggedMemories.push_back(std::move(memory));
                        ConsoleInfo << "correctly unplugged the memory module from the slot: " << slotValue << std::endl;
                    }
                    else
                    {
                        ConsoleError << "can't unplug the memory module from the slot: " << slotValue << std::endl;
                        return false;
                    }
                }
                else
                {
                    ConsoleError << R"(please put "m" (motherboard) or "p" (processor))" << std::endl;
                    return false;
                }
                return true;
            }},
            {"plug", R"(plug ["m"/"p"] [slot] [index])", "plug a memory module on the motherboard/processor for the unplugged list", 3,3, [&]([[maybe_unused]] const std::vector<std::string>& args){
                if (args[0] == "m")
                {
                    std::size_t slotValue = std::strtoul(args[1].c_str(), nullptr, 0);
                    std::size_t unpluggedIndex = std::strtoul(args[2].c_str(), nullptr, 0);

                    if (unpluggedIndex < unpluggedMemories.size())
                    {
                        if ( motherboard.memoryPlug(slotValue, unpluggedMemories[unpluggedIndex]) )
                        {
                            unpluggedMemories.erase(unpluggedMemories.begin()+unpluggedIndex);
                            ConsoleInfo << "correctly plugged the memory in the slot: " << slotValue << std::endl;
                        }
                        else
                        {
                            ConsoleError << "can't plug the memory module in the slot: " << slotValue << std::endl;
                            return false;
                        }
                    }
                    else
                    {
                        ConsoleError << "unplugged memory "<< unpluggedIndex <<" doesn't exist" << std::endl;
                        return false;
                    }
                }
                else if (args[0] == "p")
                {
                    std::size_t slotValue = std::strtoul(args[1].c_str(), nullptr, 0);
                    std::size_t unpluggedIndex = std::strtoul(args[2].c_str(), nullptr, 0);

                    if (unpluggedIndex < unpluggedMemories.size())
                    {
                        if ( motherboard._processor.memoryPlug(slotValue, unpluggedMemories[unpluggedIndex]) )
                        {
                            unpluggedMemories.erase(unpluggedMemories.begin()+unpluggedIndex);
                            ConsoleInfo << "correctly plugged the memory in the slot: " << slotValue << std::endl;
                        }
                        else
                        {
                            ConsoleError << "can't plug the memory module in the slot: " << slotValue << std::endl;
                            return false;
                        }
                    }
                    else
                    {
                        ConsoleError << "unplugged memory "<< unpluggedIndex <<" doesn't exist" << std::endl;
                        return false;
                    }
                }
                else
                {
                    ConsoleError << R"(please put "m" (motherboard) or "p" (processor))" << std::endl;
                    return false;
                }
                return true;
            }},
            {"info", R"(info ["motherboard"/"memUnplugged"])", "print information about peripherals", 1,1, [&]([[maybe_unused]] const std::vector<std::string>& args){
                if (args[0] == "motherboard")
                {
                    ConsoleInfo << "Name: --NAME--\n";
                    ConsoleInfo << "Peripheral slot size: " << motherboard.getPeripheralSlotSize() << '\n';
                    ConsoleInfo << "Memory slot size: " << motherboard.getMemorySlotSize() << " with " << 0 << " sources slot\n";
                    ConsoleInfo << "Program counter: " << motherboard.getProgramCounter() << std::endl;
                }
                else if (args[0] == "memUnplugged")
                {
                    for (std::size_t i=0; i<unpluggedMemories.size(); ++i)
                    {
                        ConsoleInfo << "["<<i<<"] type: " << unpluggedMemories[i]->getType()
                                    << " address bus size: " <<  unpluggedMemories[i]->getAddressBusSize()
                                    << " size: " << unpluggedMemories[i]->getMemorySize() << std::endl;
                    }
                }
                else
                {
                    return false;
                }
                return true;
            }},
            {"execute", "execute [cycle]", "execute a number of clock cycle (clock until sync)", 1,1, [&]([[maybe_unused]] const std::vector<std::string>& args){
                std::size_t clockCycle = std::strtoul(args[0].c_str(), nullptr, 0);

                for (std::size_t i=0; i<clockCycle; ++i)
                {
                    if ( !motherboard._processor.clockUntilSync(20) )
                    {
                        ConsoleWarning << "max iteration reached !" << std::endl;
                        break;
                    }
                }
                ConsoleInfo << "pc: "<< motherboard.getProgramCounter()
                            <<" ("<< codeg::ValueToHex(motherboard.getProgramCounter(), 8, true) <<")"
                            << std::endl;
                return true;
            }},
            {"goto", "goto [address]", "execute a number of clock cycle (clock until sync) until the address is reached (or max iterations)", 1,1, [&]([[maybe_unused]] const std::vector<std::string>& args){
                codeg::MemoryAddress memoryAddress = std::strtoul(args[0].c_str(), nullptr, 0);
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
                        ConsoleWarning << "clock cycle: max iteration reached !" << std::endl;
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
                return true;
            }}
        };

        ConsoleInfo << "Waiting user input" << std::endl;

        std::string commandLine;
        std::string commandName;
        std::vector<std::string> commandArgs;
        do
        {
            std::cout << ">";
            std::getline(std::cin, commandLine);

            commandLine = codeg::RemoveExtraSpace(commandLine);

            codeg::Split(commandLine, commandArgs, ' ');

            if (commandArgs.empty())
            {
                continue;
            }

            commandName = std::move(commandArgs.front());
            commandArgs.erase(commandArgs.begin());

            ConsoleInfo << "user command: \"" << commandName << "\"" << std::endl;

            bool knownCommand = false;

            if (commandName == "help")
            {
                knownCommand = true;

                for (auto& command : commands)
                {
                    ConsoleInfo << '\t' << command._name << ", " << command._usage << " <- " << command._description << std::endl;
                }
            }
            else
            {
                for (auto& command: commands)
                {
                    if (command._name == commandName)
                    {
                        knownCommand = true;

                        if (commandArgs.size() < command._minArguments ||
                            commandArgs.size() > command._maxArguments)
                        {
                            ConsoleError << "bad arguments size: " << command._minArguments << " >= "
                                         << commandArgs.size() << " <= " << command._maxArguments << std::endl;
                            ConsoleError << "usage: " << command._usage << std::endl;
                        }
                        else if (!command._func(commandArgs))
                        {
                            ConsoleError << "usage: " << command._usage << std::endl;
                        }
                        break;
                    }
                }
            }
            if (!knownCommand)
            {
                ConsoleWarning << "unknown command" << std::endl;
            }
        } while (running);
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
