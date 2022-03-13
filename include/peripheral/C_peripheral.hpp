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

#ifndef C_PERIPHERAL_HPP_INCLUDED
#define C_PERIPHERAL_HPP_INCLUDED

#include <cstdint>
#include <memory>
#include <vector>
#include "C_bus.hpp"
#include "C_signal.hpp"

namespace codeg
{

class Motherboard;

enum PeripheralType
{
    TYPE_PP1,
    TYPE_HARDWARE
};

class Peripheral
{
public:
    Peripheral() = default;
    virtual ~Peripheral() = default;

    [[nodiscard]] bool isSelected() const
    {
        return this->g_isSelected;
    }
    void select(bool flag)
    {
        this->g_isSelected = flag;
    }

    [[nodiscard]] virtual codeg::PeripheralType getType() const = 0;

    virtual void update(codeg::Motherboard& motherboard, codeg::BusMap& busses, codeg::SignalMap& signals) = 0;

private:
    bool g_isSelected{false};
};

struct PeripheralSlot
{
    std::shared_ptr<codeg::Peripheral> _peripheral;
    codeg::PeripheralType _slotType;

    bool _isPluggable;
};

class PeripheralSlotCapable
{
public:
    PeripheralSlotCapable() = default;
    virtual ~PeripheralSlotCapable() = default;

    void peripheralUpdateAll(std::size_t selected, codeg::Motherboard& motherboard, codeg::BusMap& busses, codeg::SignalMap& signals)
    {
        for (std::size_t i=0; i<this->_g_peripheralSlots.size(); ++i)
        {
            if (this->_g_peripheralSlots[i]._peripheral)
            {
                this->_g_peripheralSlots[i]._peripheral->select(selected == i);
                this->_g_peripheralSlots[i]._peripheral->update(motherboard, busses, signals);
            }
        }
    }

    bool peripheralPlug(std::size_t index, const std::shared_ptr<codeg::Peripheral>& peripheral)
    {
        if (index < this->_g_peripheralSlots.size())
        {
            if (this->_g_peripheralSlots[index]._isPluggable && (this->_g_peripheralSlots[index]._slotType == peripheral->getType()))
            {
                if (this->_g_peripheralSlots[index]._peripheral == nullptr)
                {
                    this->_g_peripheralSlots[index]._peripheral = peripheral;
                    return true;
                }
            }
        }
        return false;
    }
    std::shared_ptr<codeg::Peripheral> peripheralUnplug(std::size_t index)
    {
        if (index < this->_g_peripheralSlots.size())
        {
            if ((this->_g_peripheralSlots[index]._peripheral != nullptr) && (this->_g_peripheralSlots[index]._isPluggable))
            {
                std::shared_ptr<codeg::Peripheral> tmpMemory = this->_g_peripheralSlots[index]._peripheral;
                this->_g_peripheralSlots[index]._peripheral.reset();
                return tmpMemory;
            }
        }
        return nullptr;
    }
    void peripheralUnplugAll()
    {
        for (std::size_t i=0; i<this->_g_peripheralSlots.size(); ++i)
        {
            this->peripheralUnplug(i);
        }
    }

    [[nodiscard]] std::size_t getPeripheralSlotSize() const
    {
        return this->_g_peripheralSlots.size();
    }
    [[nodiscard]] const codeg::PeripheralSlot* getPeripheralSlot(std::size_t index) const
    {
        if (index < this->_g_peripheralSlots.size())
        {
            return &this->_g_peripheralSlots[index];
        }
        return nullptr;
    }

protected:
    std::vector<codeg::PeripheralSlot> _g_peripheralSlots;
};

}//end codeg

#endif // C_PERIPHERAL_HPP_INCLUDED