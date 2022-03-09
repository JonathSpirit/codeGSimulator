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

#ifndef C_GP8B_5_1_HPP_INCLUDED
#define C_GP8B_5_1_HPP_INCLUDED

#include <cstdint>
#include "processor/C_processor.hpp"

namespace codeg
{

class GP8B_5_1 : public codeg::ProcessorSPS1
{
public:
    enum Stat
    {
        STAT_SYNC_BIT,
        STAT_INSTRUCTION_SET,
        STAT_EXECUTION

        //STAT_WAITING
        //STAT_RAMWRITE_OR_END
    };

    GP8B_5_1() = default;
    ~GP8B_5_1() override = default;

    void clock() override;

    void softReset() override;
    void hardReset() override;

private:
    void executeInstruction();

    Stat g_stat{STAT_SYNC_BIT};
    uint8_t g_instruction{0};
};

}//end codeg

#endif // C_GP8B_5_1_HPP_INCLUDED