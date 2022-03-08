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
#include "memoryModule/C_memoryModule.hpp"

namespace codeg
{

class Motherboard : public codeg::MemoryModuleSlotCapable
{
protected:
    Motherboard() = default;
    ~Motherboard() override = default;

public:
    bool setProgramCounter(codeg::MemoryAddress address)
    {
        this->_g_programCounter = address;
        return true;
    }
    [[nodiscard]] codeg::MemoryAddress getProgramCounter() const
    {
        return this->_g_programCounter;
    }

    virtual void softReset() = 0;
    virtual void hardReset() = 0;

protected:
    std::vector<codeg::MemoryModuleSlot> _g_memorySlots;
    codeg::MemoryAddress _g_programCounter{0};
};

}//end codeg

#endif //C_MOTHERBOARD_HPP_INCLUDED