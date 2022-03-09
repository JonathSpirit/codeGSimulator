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

namespace codeg
{

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

        this->g_stat = STAT_EXECUTION;
        break;
    case STAT_EXECUTION:
        this->executeInstruction();

        this->g_stat = STAT_SYNC_BIT;
        break;
    }
}

void GP8B_5_1::softReset()
{
}
void GP8B_5_1::hardReset()
{
}

void GP8B_5_1::executeInstruction()
{

}

}//end codeg
