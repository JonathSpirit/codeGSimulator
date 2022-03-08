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

#ifndef C_MEMORYMODULE_HPP_INCLUDED
#define C_MEMORYMODULE_HPP_INCLUDED

#include <cstdint>
#include <memory>
#include <vector>

namespace codeg
{

enum MemoryModuleType
{
    TYPE_MM1
};

using AddressBusSize = uint8_t;
using MemorySize = std::size_t;
using MemoryAddress = std::size_t;

class MemoryModule
{
public:
    explicit MemoryModule(codeg::MemorySize memorySize) :
            _g_memorySize(memorySize)
    {}
    virtual ~MemoryModule() = default;

    [[nodiscard]] codeg::MemorySize getMemorySize() const
    {
        return this->_g_memorySize;
    }

    [[nodiscard]] codeg::MemorySize getMaxMemorySize(AddressBusSize size) const
    {
        codeg::MemorySize wanted = static_cast<codeg::MemorySize>(1) << size;
        return this->_g_memorySize > wanted ? wanted : this->_g_memorySize;
    }
    [[nodiscard]] codeg::MemorySize getAddressBusSize() const
    {
        codeg::MemorySize n = 0;
        codeg::MemorySize x = this->_g_memorySize;

        while (x != 0)
        {
            x >>= 8;
            ++n;
        }
        return n;
    }

    virtual bool set(codeg::MemoryAddress address, uint8_t data) = 0;
    virtual bool set(codeg::MemoryAddress address, uint8_t* data, codeg::MemorySize dataSize) = 0;
    virtual bool get(codeg::MemoryAddress address, uint8_t& data) const = 0;
    virtual bool get(codeg::MemoryAddress startAddress, codeg::MemorySize addressCount, uint8_t* data, codeg::MemorySize dataSize) const = 0;

    [[nodiscard]] virtual codeg::MemoryModuleType getType() const = 0;

protected:
    codeg::MemorySize _g_memorySize;
};

struct MemoryModuleSlot
{
    std::shared_ptr<codeg::MemoryModule> _mem;
    codeg::MemoryModuleType _slotType;

    codeg::AddressBusSize _slotBusSizeCapacity;
    bool _isSourceCapable;
    bool _isPluggable;
};

class MemoryModuleSlotCapable
{
public:
    MemoryModuleSlotCapable() = default;
    virtual ~MemoryModuleSlotCapable() = default;

    bool memoryPlug(std::size_t index, const std::shared_ptr<codeg::MemoryModule>& memoryModule)
    {
        if (index < this->_g_memorySlots.size())
        {
            if (this->_g_memorySlots[index]._isPluggable && (this->_g_memorySlots[index]._slotType == memoryModule->getType()))
            {
                if (this->_g_memorySlots[index]._mem == nullptr)
                {
                    this->_g_memorySlots[index]._mem = memoryModule;
                    return true;
                }
            }
        }
        return false;
    }
    std::shared_ptr<codeg::MemoryModule> memoryUnplug(std::size_t index)
    {
        if (index < this->_g_memorySlots.size())
        {
            if ((this->_g_memorySlots[index]._mem != nullptr) && (this->_g_memorySlots[index]._isPluggable))
            {
                std::shared_ptr<codeg::MemoryModule> tmpMemory = this->_g_memorySlots[index]._mem;
                this->_g_memorySlots[index]._mem.reset();
                return tmpMemory;
            }
        }
        return nullptr;
    }
    void memoryUnplugAll()
    {
        for (std::size_t i=0; i<this->_g_memorySlots.size(); ++i)
        {
            this->memoryUnplug(i);
        }
    }

    [[nodiscard]] std::size_t getMemorySlotSize() const
    {
        return this->_g_memorySlots.size();
    }
    [[nodiscard]] const codeg::MemoryModuleSlot* getMemorySlot(std::size_t index) const
    {
        if (index < this->_g_memorySlots.size())
        {
            return &this->_g_memorySlots[index];
        }
        return nullptr;
    }
    [[nodiscard]] const codeg::MemoryModuleSlot* getMemorySourceSlot() const
    {
        if (this->_g_memorySource < this->_g_memorySlots.size())
        {
            return &this->_g_memorySlots[this->_g_memorySource];
        }
        return nullptr;
    }

    bool setMemorySource(std::size_t index)
    {
        if (index < this->_g_memorySlots.size())
        {
            if (this->_g_memorySlots[index]._isSourceCapable)
            {
                this->_g_memorySource = index;
                return true;
            }
        }
        return false;
    }
    [[nodiscard]] std::size_t getMemorySourceIndex() const
    {
        return this->_g_memorySource;
    }

protected:
    std::vector<codeg::MemoryModuleSlot> _g_memorySlots;
    std::size_t _g_memorySource{0};
};

}//end codeg

#endif // C_MEMORYMODULE_HPP_INCLUDED