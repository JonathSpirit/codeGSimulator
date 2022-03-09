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

#include "motherboard/C_GCM_5_1.hpp"

namespace codeg
{

GCM_5_1_SPS1::GCM_5_1_SPS1()
{
    this->_g_memorySlots.push_back( {nullptr, codeg::MemoryModuleType::TYPE_MM1, 3, true, true} );
    this->_g_memorySlots.push_back( {nullptr, codeg::MemoryModuleType::TYPE_MM1, 3, true, true} );

    this->_processor._signals.get(CG_PROC_SPS1_SIGNAL_ADDSRC_CLK).attach(this, &codeg::GCM_5_1_SPS1::signal_ADDSRC_CLK);
    this->_processor._signals.get(CG_PROC_SPS1_SIGNAL_JMPSRC_CLK).attach(this, &codeg::GCM_5_1_SPS1::signal_JMPSRC_CLK);
    this->_processor._signals.get(CG_PROC_SPS1_SIGNAL_PERIPHERAL_CLK).attach(this, &codeg::GCM_5_1_SPS1::signal_PERIPHERAL_CLK);
    this->_processor._signals.get(CG_PROC_SPS1_SIGNAL_SELECTING_RBEXT1).attach(this, &codeg::GCM_5_1_SPS1::signal_SELECTING_RBEXT1);
    this->_processor._signals.get(CG_PROC_SPS1_SIGNAL_SELECTING_RBEXT2).attach(this, &codeg::GCM_5_1_SPS1::signal_SELECTING_RBEXT2);
}

void GCM_5_1_SPS1::softReset()
{
    this->setProgramCounter(0);
    this->updateDataSource();

    this->_processor.softReset();
}
void GCM_5_1_SPS1::hardReset()
{
    this->setProgramCounter(0);
    this->setMemorySource(0);
    this->updateDataSource();

    this->_processor.hardReset();
}

uint8_t GCM_5_1_SPS1::updateDataSource()
{
    uint8_t memData = 0;
    this->getMemorySourceSlot()->_mem->get(this->getProgramCounter(), memData);
    this->_processor._busses.get(CG_PROC_SPS1_BUS_BDATASRC).set(memData);
    return memData;
}

void GCM_5_1_SPS1::signal_ADDSRC_CLK(bool val)
{
    if (val)
    {
        this->setProgramCounter( this->_g_programCounter+1 );
        this->updateDataSource();
    }
}
void GCM_5_1_SPS1::signal_JMPSRC_CLK(bool val)
{
    if (val)
    {
        this->setProgramCounter(this->_processor._busses.get(CG_PROC_SPS1_BUS_BJMPSRC).get());
    }
}
void GCM_5_1_SPS1::signal_PERIPHERAL_CLK([[maybe_unused]] bool val)
{

}
void GCM_5_1_SPS1::signal_SELECTING_RBEXT1([[maybe_unused]] bool val)
{

}
void GCM_5_1_SPS1::signal_SELECTING_RBEXT2([[maybe_unused]] bool val)
{

}

}//end codeg
