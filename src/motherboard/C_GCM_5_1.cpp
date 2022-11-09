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
    this->_g_peripheralSlots.push_back( {nullptr, codeg::PeripheralType::TYPE_PP1, true} );
    this->_g_peripheralSlots.push_back( {nullptr, codeg::PeripheralType::TYPE_PP1, true} );
    this->_g_peripheralSlots.push_back( {nullptr, codeg::PeripheralType::TYPE_PP1, true} );
    this->_g_peripheralSlots.push_back( {nullptr, codeg::PeripheralType::TYPE_PP1, true} );
    this->_g_peripheralSlots.push_back( {std::make_shared<codeg::MemoryController>(), codeg::PeripheralType::TYPE_HARDWARE, false} );

    this->_g_memorySlots.push_back( {nullptr, "MM1", 3, true, true} );
    this->_g_memorySlots.push_back( {nullptr, "MM1", 3, true, true} );

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
        this->updateDataSource();
    }
}
void GCM_5_1_SPS1::signal_PERIPHERAL_CLK([[maybe_unused]] bool val)
{
    if (val)
    {
        this->peripheralUpdateAll(this->_processor._busses.get(CG_PROC_SPS1_BUS_BPCS).get(), *this, this->_processor._busses, this->_processor._signals);
    }
}
void GCM_5_1_SPS1::signal_SELECTING_RBEXT1([[maybe_unused]] bool val)
{
    if (val)
    {
        this->_processor._busses.get(CG_PROC_SPS1_BUS_NUMBER) = this->_processor._busses.get(CG_PROC_SPS1_BUS_BWRITE1);
    }
}
void GCM_5_1_SPS1::signal_SELECTING_RBEXT2([[maybe_unused]] bool val)
{
    if (val)
    {
        this->_processor._busses.get(CG_PROC_SPS1_BUS_NUMBER) = this->_processor._busses.get(CG_PROC_SPS1_BUS_BWRITE2);
    }
}

///MemoryController

void MemoryController::update(codeg::Motherboard& motherboard, codeg::BusMap& busses, codeg::SignalMap& signals)
{
    if ( this->isSelected() )
    {
        uint8_t bwrite1 = busses.get(CG_PROC_SPS1_BUS_BWRITE1).get();
        uint8_t bwrite2 = busses.get(CG_PROC_SPS1_BUS_BWRITE2).get();

        if (signals.get(CG_PROC_SPS1_SIGNAL_PERIPHERAL_CLK)._value)
        {
            if (bwrite1 & CG_PERIPHERAL_MEMORY_CONTROLLER_ADDRESS0_MASK)
            {
                if (!this->g_addressClock0Flag)
                {
                    this->g_addressClock0Flag = true;
                    this->g_address &=~ 0x000000FF;
                    this->g_address |= static_cast<uint32_t>(bwrite2);
                }
            }
            else
            {
                this->g_addressClock0Flag = false;
            }
            if (bwrite1 & CG_PERIPHERAL_MEMORY_CONTROLLER_ADDRESS1_MASK)
            {
                if (!this->g_addressClock1Flag)
                {
                    this->g_addressClock1Flag = true;
                    this->g_address &=~ 0x0000FF00;
                    this->g_address |= static_cast<uint32_t>(bwrite2)<<8;
                }
            }
            else
            {
                this->g_addressClock1Flag = false;
            }
            if (bwrite1 & CG_PERIPHERAL_MEMORY_CONTROLLER_ADDRESS2_MASK)
            {
                if (!this->g_addressClock2Flag)
                {
                    this->g_addressClock2Flag = true;
                    this->g_address &=~ 0x00FF0000;
                    this->g_address |= static_cast<uint32_t>(bwrite2)<<16;
                }
            }
            else
            {
                this->g_addressClock2Flag = false;
            }
            if (!(bwrite1 & CG_PERIPHERAL_MEMORY_CONTROLLER_WE_MASK))
            {
                if (!this->g_writeFlag)
                {
                    this->g_writeFlag = true;
                    if ((bwrite1&CG_PERIPHERAL_MEMORY_CONTROLLER_CE_MASK) &&
                        (bwrite1&CG_PERIPHERAL_MEMORY_CONTROLLER_OE_MASK) )
                    {
                        std::size_t index = 1-motherboard.getMemorySourceIndex();
                        std::shared_ptr<codeg::MemoryModule> mem = motherboard.getMemorySlot(index)->_mem;
                        if (mem)
                        {
                            mem->set(this->g_address, bwrite2);
                        }
                    }
                }
            }
            else
            {
                this->g_writeFlag = false;
            }
        }

        if ((bwrite1&CG_PERIPHERAL_MEMORY_CONTROLLER_CE_MASK) &&
            !(bwrite1&CG_PERIPHERAL_MEMORY_CONTROLLER_OE_MASK) )
        {
            std::size_t index = 1-motherboard.getMemorySourceIndex();
            std::shared_ptr<codeg::MemoryModule> mem = motherboard.getMemorySlot(index)->_mem;
            if (mem)
            {
                uint8_t data = 0;
                mem->get(this->g_address, data);

                busses.get(CG_PROC_SPS1_BUS_BREAD1).set(data);
            }
            else
            {
                busses.get(CG_PROC_SPS1_BUS_BREAD1).set(0);
            }
        }
        else
        {
            busses.get(CG_PROC_SPS1_BUS_BREAD1).set(0);
        }
    }
}

codeg::PeripheralType MemoryController::getType() const
{
    return codeg::PeripheralType::TYPE_HARDWARE;
}

}//end codeg
