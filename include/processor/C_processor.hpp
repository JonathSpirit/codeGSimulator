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

#ifndef C_PROCESSOR_HPP_INCLUDED
#define C_PROCESSOR_HPP_INCLUDED

#include <cstdint>
#include "memoryModule/C_MM1.hpp"
#include "C_bus.hpp"
#include "C_signal.hpp"

namespace codeg
{

class ProcessorSPS1 : public codeg::MemoryModuleSlotCapable
{
public:
    ProcessorSPS1() = default;
    ~ProcessorSPS1() override = default;

    virtual void clock() = 0;

    virtual void softReset() = 0;
    virtual void hardReset() = 0;

    //24 bits bus for address jump control.
    codeg::Bus<24> _BJMPSRC{0};

    //8 bits bus for general output pins.
    codeg::Bus<8> _BWRITE1{0};
    codeg::Bus<8> _BWRITE2{0};
    //8 bits bus for general input pins.
    codeg::Bus<8> _BREAD1{0};
    codeg::Bus<8> _BREAD2{0};

    //8 bits bus for number control.
    codeg::Bus<8> _NUMBER{0};

    //8 bits bus for source memory data.
    codeg::Bus<8> _BDATASRC{0};

    //6 bits bus for choosing a peripheral.
    codeg::Bus<6> _BPCS{0};

    //next address clock for source memory.
    codeg::Signal _ADDSRC_CLK{nullptr};
    //jumping to address clock for source memory.
    codeg::Signal _JMPSRC_CLK{nullptr};
    //pulse for selected peripheral.
    codeg::Signal _PERIPHERAL_CLK{nullptr};

    //read bus extern 1 selection (active low).
    codeg::Signal _SELECTING_RBEXT1{nullptr};
    //read bus extern 2 selection (active low).
    codeg::Signal _SELECTING_RBEXT2{nullptr};
};

}//end codeg

#endif //C_PROCESSOR_HPP_INCLUDED