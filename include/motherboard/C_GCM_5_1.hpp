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

#ifndef C_GCM_5_1_HPP_INCLUDED
#define C_GCM_5_1_HPP_INCLUDED

#include "motherboard/C_motherboard.hpp"
#include "processor/C_GP8B_5_1.hpp"

namespace codeg
{

class GCM_5_1_SPS1 : public codeg::Motherboard, public codeg::SignalCapable
{
public:
    GCM_5_1_SPS1();
    ~GCM_5_1_SPS1() override = default;

    void softReset() override;
    void hardReset() override;

    void signal_ADDSRC_CLK(bool val);
    void signal_JMPSRC_CLK(bool val);
    void signal_PERIPHERAL_CLK(bool val);
    void signal_SELECTING_RBEXT1(bool val);
    void signal_SELECTING_RBEXT2(bool val);

    codeg::GP8B_5_1 _processor;
};

}//end codeg

#endif // C_GCM_5_1_HPP_INCLUDED