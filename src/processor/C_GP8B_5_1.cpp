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

#include "processor/C_GP8B_5_1.hpp"
#include "C_codeg.hpp"

namespace codeg
{

GP8B_5_1::GP8B_5_1()
{
    this->_g_memorySlots.push_back( {nullptr, codeg::MemoryModuleType::TYPE_MM1, 2, false, true} );
}

void GP8B_5_1::clock()
{
    switch (this->g_stat)
    {
    case STAT_SYNC_BIT:
        this->g_stat = STAT_INSTRUCTION_SET;
        break;
    case STAT_INSTRUCTION_SET:
        this->g_instruction = this->_busses.get(CG_PROC_SPS1_BUS_BDATASRC).get();
        this->_signals.get(CG_PROC_SPS1_SIGNAL_ADDSRC_CLK).call(true);
        this->_signals.get(CG_PROC_SPS1_SIGNAL_ADDSRC_CLK).call(false);
        this->computeArgument();

        this->g_stat = STAT_EXECUTION;
        break;
    case STAT_EXECUTION:
        this->executeInstruction();

        if ( static_cast<codeg::CodegBinaryRev1Busses>(this->g_instruction&CG_CODEGBINARYREV1_BUSSES_MASK) == codeg::CodegBinaryRev1Busses::READABLE_SOURCE)
        {//There is no argument if the selected bus is not source !
            this->_signals.get(CG_PROC_SPS1_SIGNAL_ADDSRC_CLK).call(true);
            this->_signals.get(CG_PROC_SPS1_SIGNAL_ADDSRC_CLK).call(false);
        }

        this->g_stat = STAT_SYNC_BIT;
        break;
    }
}

void GP8B_5_1::softReset()
{
    this->g_stat = STAT_SYNC_BIT;
}
void GP8B_5_1::hardReset()
{
    this->g_stat = STAT_SYNC_BIT;
}

void GP8B_5_1::executeInstruction()
{
    switch( static_cast<codeg::CodegBinaryRev1>(this->g_instruction&CG_CODEGBINARYREV1_OPCODE_MASK) )
    {
    case CodegBinaryRev1::OPCODE_BWRITE1_CLK:
        this->_busses.get(CG_PROC_SPS1_BUS_BWRITE1).set(this->g_arguments);
        break;
    case CodegBinaryRev1::OPCODE_BWRITE2_CLK:
        this->_busses.get(CG_PROC_SPS1_BUS_BWRITE2).set(this->g_arguments);
        break;
    case CodegBinaryRev1::OPCODE_BPCS_CLK:
        this->_busses.get(CG_PROC_SPS1_BUS_BPCS).set(this->g_arguments);
        break;
    case CodegBinaryRev1::OPCODE_OPLEFT_CLK:
        this->_alu->setOperationLeft(this->g_arguments);
        break;
    case CodegBinaryRev1::OPCODE_OPRIGHT_CLK:
        this->_alu->setOperationRight(this->g_arguments);
        break;
    case CodegBinaryRev1::OPCODE_OPCHOOSE_CLK:
        this->_alu->setOperation(this->g_arguments);
        break;
    case CodegBinaryRev1::OPCODE_PERIPHERAL_CLK:
        this->_signals.get(CG_PROC_SPS1_SIGNAL_PERIPHERAL_CLK).call(true);
        this->_signals.get(CG_PROC_SPS1_SIGNAL_PERIPHERAL_CLK).call(false);
        break;
    case CodegBinaryRev1::OPCODE_BJMPSRC1_CLK:
    {
        uint32_t x = this->_busses.get(CG_PROC_SPS1_BUS_BJMPSRC).get();
        x &=~ 0x000000FF;
        x |= static_cast<uint32_t>(this->g_arguments);
        this->_busses.get(CG_PROC_SPS1_BUS_BJMPSRC).set(x);
    }
        break;
    case CodegBinaryRev1::OPCODE_BJMPSRC2_CLK:
    {
        uint32_t x = this->_busses.get(CG_PROC_SPS1_BUS_BJMPSRC).get();
        x &=~ 0x0000FF00;
        x |= static_cast<uint32_t>(this->g_arguments)<<8;
        this->_busses.get(CG_PROC_SPS1_BUS_BJMPSRC).set(x);
    }
        break;
    case CodegBinaryRev1::OPCODE_BJMPSRC3_CLK:
    {
        uint32_t x = this->_busses.get(CG_PROC_SPS1_BUS_BJMPSRC).get();
        x &=~ 0x00FF0000;
        x |= static_cast<uint32_t>(this->g_arguments)<<16;
        this->_busses.get(CG_PROC_SPS1_BUS_BJMPSRC).set(x);
    }
        break;
    case CodegBinaryRev1::OPCODE_JMPSRC_CLK:
        this->_signals.get(CG_PROC_SPS1_SIGNAL_JMPSRC_CLK).call(true);
        this->_signals.get(CG_PROC_SPS1_SIGNAL_JMPSRC_CLK).call(false);
        break;
    case CodegBinaryRev1::OPCODE_BRAMADD1_CLK:
        this->g_ramAddress &=~ 0x00FF;
        this->g_ramAddress |= static_cast<uint16_t>(this->g_arguments);
        break;
    case CodegBinaryRev1::OPCODE_BRAMADD2_CLK:
        this->g_ramAddress &=~ 0xFF00;
        this->g_ramAddress |= static_cast<uint16_t>(this->g_arguments)<<8;
        break;
    case CodegBinaryRev1::OPCODE_IF:
        if (this->g_arguments)
        {
            this->_signals.get(CG_PROC_SPS1_SIGNAL_ADDSRC_CLK).call(true);
            this->_signals.get(CG_PROC_SPS1_SIGNAL_ADDSRC_CLK).call(false);
        }
        break;
    case CodegBinaryRev1::OPCODE_IFNOT:
        if (!this->g_arguments)
        {
            this->_signals.get(CG_PROC_SPS1_SIGNAL_ADDSRC_CLK).call(true);
            this->_signals.get(CG_PROC_SPS1_SIGNAL_ADDSRC_CLK).call(false);
        }
        break;
    case CodegBinaryRev1::OPCODE_RAMW:
        if ( this->getMemorySlot(0)->_mem )
        {
            this->getMemorySlot(0)->_mem->set(this->g_ramAddress, this->g_arguments);
        }
        break;
    case CodegBinaryRev1::OPCODE_SPI_CLK:
    case CodegBinaryRev1::OPCODE_BCFG_SPI_CLK:
    case CodegBinaryRev1::OPCODE_STICK:
    case CodegBinaryRev1::OPCODE_LTICK:
        break;
    }
}

void GP8B_5_1::computeArgument()
{
    switch( static_cast<codeg::CodegBinaryRev1Busses>(this->g_instruction&CG_CODEGBINARYREV1_BUSSES_MASK) )
    {
    case CodegBinaryRev1Busses::READABLE_SOURCE:
        this->g_arguments = this->_busses.get(CG_PROC_SPS1_BUS_BDATASRC).get();
        break;
    case CodegBinaryRev1Busses::READABLE_BREAD1:
        this->g_arguments = this->_busses.get(CG_PROC_SPS1_BUS_BREAD1).get();
        break;
    case CodegBinaryRev1Busses::READABLE_BREAD2:
        this->g_arguments = this->_busses.get(CG_PROC_SPS1_BUS_BREAD2).get();
        break;
    case CodegBinaryRev1Busses::READABLE_RESULT:
        this->g_arguments = this->_alu->getResult();
        break;
    case CodegBinaryRev1Busses::READABLE_RAM:
        if (this->getMemorySlot(0)->_mem)
        {
            if ( !this->getMemorySlot(0)->_mem->get(this->g_ramAddress, this->g_arguments) )
            {
                this->g_arguments = 0;
            }
        }
        else
        {
            this->g_arguments = 0;
        }
        break;
    case CodegBinaryRev1Busses::READABLE_SPI:
        this->g_arguments = 0; ///TODO: implement an spi emulation
        break;
    case CodegBinaryRev1Busses::READABLE_EXT1:
        this->_signals.get(CG_PROC_SPS1_SIGNAL_SELECTING_RBEXT1).call(true);
        this->_signals.get(CG_PROC_SPS1_SIGNAL_SELECTING_RBEXT1).call(false);
        this->g_arguments = this->_busses.get(CG_PROC_SPS1_BUS_NUMBER).get();
        break;
    case CodegBinaryRev1Busses::READABLE_EXT2:
        this->_signals.get(CG_PROC_SPS1_SIGNAL_SELECTING_RBEXT2).call(true);
        this->_signals.get(CG_PROC_SPS1_SIGNAL_SELECTING_RBEXT2).call(false);
        this->g_arguments = this->_busses.get(CG_PROC_SPS1_BUS_NUMBER).get();
        break;
    default:
        this->g_arguments = 0;
        break;
    }

    this->_busses.get(CG_PROC_SPS1_BUS_NUMBER).set(this->g_arguments);
}

}//end codeg
