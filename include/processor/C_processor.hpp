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
#include "processor/C_alu.hpp"
#include "C_bus.hpp"
#include "C_signal.hpp"

#define CG_PROC_SPS1_BUS_BJMPSRC "BJMPSRC"
#define CG_PROC_SPS1_BUS_BWRITE1 "BWRITE1"
#define CG_PROC_SPS1_BUS_BWRITE2 "BWRITE2"
#define CG_PROC_SPS1_BUS_BREAD1 "BREAD1"
#define CG_PROC_SPS1_BUS_BREAD2 "BREAD2"
#define CG_PROC_SPS1_BUS_NUMBER "NUMBER"
#define CG_PROC_SPS1_BUS_BDATASRC "BDATASRC"
#define CG_PROC_SPS1_BUS_BPCS "BPCS"

#define CG_PROC_SPS1_SIGNAL_ADDSRC_CLK "ADDSRC_CLK"
#define CG_PROC_SPS1_SIGNAL_JMPSRC_CLK "JMPSRC_CLK"
#define CG_PROC_SPS1_SIGNAL_PERIPHERAL_CLK "PERIPHERAL_CLK"
#define CG_PROC_SPS1_SIGNAL_SELECTING_RBEXT1 "SELECTING_RBEXT1"
#define CG_PROC_SPS1_SIGNAL_SELECTING_RBEXT2 "SELECTING_RBEXT2"

namespace codeg
{

class ProcessorSPS1 : public codeg::MemoryModuleSlotCapable
{
public:
    ProcessorSPS1()
    {
        this->_busses.add(CG_PROC_SPS1_BUS_BJMPSRC, 24);
        this->_busses.add(CG_PROC_SPS1_BUS_BWRITE1, 8);
        this->_busses.add(CG_PROC_SPS1_BUS_BWRITE2, 8);
        this->_busses.add(CG_PROC_SPS1_BUS_BREAD1, 8);
        this->_busses.add(CG_PROC_SPS1_BUS_BREAD2, 8);
        this->_busses.add(CG_PROC_SPS1_BUS_NUMBER, 8);
        this->_busses.add(CG_PROC_SPS1_BUS_BDATASRC, 8);
        this->_busses.add(CG_PROC_SPS1_BUS_BPCS, 6);

        this->_signals.add(CG_PROC_SPS1_SIGNAL_ADDSRC_CLK);
        this->_signals.add(CG_PROC_SPS1_SIGNAL_JMPSRC_CLK);
        this->_signals.add(CG_PROC_SPS1_SIGNAL_PERIPHERAL_CLK);
        this->_signals.add(CG_PROC_SPS1_SIGNAL_SELECTING_RBEXT1);
        this->_signals.add(CG_PROC_SPS1_SIGNAL_SELECTING_RBEXT2);
    }
    ~ProcessorSPS1() override = default;

    virtual void clock() = 0;
    bool clockUntilSync(std::size_t maxIteration)
    {
        do
        {
            this->clock();
        }
        while ((maxIteration--) && !this->isSync());

        return maxIteration > 0;
    }

    virtual void softReset() = 0;
    virtual void hardReset() = 0;

    [[nodiscard]] virtual bool isSync() const = 0;

    codeg::BusMap _busses;
    codeg::SignalMap _signals;
    std::shared_ptr<codeg::Alu> _alu;
};

}//end codeg

#endif //C_PROCESSOR_HPP_INCLUDED