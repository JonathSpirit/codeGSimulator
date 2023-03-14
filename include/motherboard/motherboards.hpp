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

#ifndef C_MOTHERBOARD_HPP_INCLUDED
#define C_MOTHERBOARD_HPP_INCLUDED

#include <cstdint>
#include "memoryModule/memoryModules.hpp"
#include "peripheral/C_peripheral.hpp"

namespace codeg
{

class Motherboard : public codeg::MemoryModuleSlotCapable, public codeg::PeripheralSlotCapable
{
protected:
    Motherboard() = default;
    ~Motherboard() override = default;

public:
    bool setProgramCounter(codeg::MemoryAddress address)
    {
        this->_g_programCounter = address;
        this->updateDataSource();
        return true;
    }
    [[nodiscard]] codeg::MemoryAddress getProgramCounter() const
    {
        return this->_g_programCounter;
    }

    virtual void softReset() = 0;
    virtual void hardReset() = 0;

    virtual uint8_t updateDataSource() = 0;

    [[nodiscard]] virtual std::string getType() = 0;

protected:
    codeg::MemoryAddress _g_programCounter{0};
};

class MotherboardClassTypeBase
{
public:
    MotherboardClassTypeBase(std::string type) :
            _g_type(std::move(type))
    {}
    virtual ~MotherboardClassTypeBase() = default;

    [[nodiscard]] virtual Motherboard* create() const = 0;

    const std::string& getType()
    {
        return this->_g_type;
    }

protected:
    std::string _g_type;
};
template<class T>
class MotherboardClassType : public MotherboardClassTypeBase
{
public:
    MotherboardClassType() :
            MotherboardClassTypeBase(T{}.getType())
    {}
    ~MotherboardClassType() override = default;

    [[nodiscard]] Motherboard* create() const override
    {
        return new T{};
    }
};

void RegisterNewMotherboardType(std::unique_ptr<MotherboardClassTypeBase>&& classType);
Motherboard* GetNewMotherboard(const std::string& type);
std::size_t GetMotherboardTypeSize();

}//end codeg

#endif //C_MOTHERBOARD_HPP_INCLUDED