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

#include "motherboard/motherboards.hpp"
#include "processor/C_GP8B_5_1.hpp"
#include "peripheral/C_peripheral.hpp"

#define CG_PERIPHERAL_MEMORY_CONTROLLER_CE_MASK 0x01
#define CG_PERIPHERAL_MEMORY_CONTROLLER_WE_MASK 0x02
#define CG_PERIPHERAL_MEMORY_CONTROLLER_OE_MASK 0x04
#define CG_PERIPHERAL_MEMORY_CONTROLLER_ADDRESS0_MASK 0x08
#define CG_PERIPHERAL_MEMORY_CONTROLLER_ADDRESS1_MASK 0x10
#define CG_PERIPHERAL_MEMORY_CONTROLLER_ADDRESS2_MASK 0x20

#define CG_PERIPHERAL_MEMORY_SOURCESWITCH_MASK 0x01

namespace codeg
{

class GCM_5_1_SPS1 : public codeg::Motherboard
{
public:
    GCM_5_1_SPS1();
    ~GCM_5_1_SPS1() override = default;

    void softReset() override;
    void hardReset() override;

    uint8_t updateDataSource() override;

    [[nodiscard]] std::string getType() override;

    void signal_ADDSRC_CLK(bool val);
    void signal_JMPSRC_CLK(bool val);
    void signal_PERIPHERAL_CLK(bool val);
    void signal_SELECTING_RBEXT1(bool val);
    void signal_SELECTING_RBEXT2(bool val);

    codeg::GP8B_5_1 _processor;
};

class MemoryController : public codeg::Peripheral
{
public:
    MemoryController() = default;
    ~MemoryController() override = default;

    void update(codeg::Motherboard& motherboard, codeg::BusMap& busses, codeg::SignalMap& signals) override;

    [[nodiscard]] codeg::PeripheralType getType() const override;

private:
    bool g_writeFlag{false};
    bool g_addressClock0Flag{false};
    bool g_addressClock1Flag{false};
    bool g_addressClock2Flag{false};
    uint32_t g_address{0};
};

class MemorySourceSwitch : public codeg::Peripheral
{
public:
    MemorySourceSwitch() = default;
    ~MemorySourceSwitch() override = default;

    void update(codeg::Motherboard& motherboard, codeg::BusMap& busses, codeg::SignalMap& signals) override;

    [[nodiscard]] codeg::PeripheralType getType() const override;
};

}//end codeg

#endif // C_GCM_5_1_HPP_INCLUDED