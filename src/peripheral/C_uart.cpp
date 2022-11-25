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

#include "peripheral/C_uart.hpp"
#include "processor/C_GP8B_5_1.hpp"
#include "C_console.hpp"
#include "C_string.hpp"

namespace codeg
{

void UART_peripheral_card_A_1_1::update([[maybe_unused]] codeg::Motherboard& motherboard, codeg::BusMap& busses, codeg::SignalMap& signals)
{
    if ( this->isSelected() )
    {
        uint8_t bwrite1 = busses.get(CG_PROC_SPS1_BUS_BWRITE1).get();
        uint8_t bwrite2 = busses.get(CG_PROC_SPS1_BUS_BWRITE2).get();

        if ( signals.get(CG_PROC_SPS1_SIGNAL_PERIPHERAL_CLK).getValue() )
        {
            if (bwrite2 & CG_PERIPHERAL_UART_RST_RX_FLAG_MASK)
            {
                if (this->g_inputBuffer.empty())
                {
                    this->g_rxFlag = false;
                }
                else
                {
                    this->g_inputBuffer.erase(0, 1);
                    this->g_rxFlag = !this->g_inputBuffer.empty();
                }
            }
            if (bwrite2 & CG_PERIPHERAL_UART_RST_TX_FLAG_MASK)
            {
                this->g_txFlag = false;
            }
            if (bwrite2 & CG_PERIPHERAL_UART_APPLY_TX_DATA_MASK)
            {
                this->g_txData = bwrite1;
            }
            if (bwrite2 & CG_PERIPHERAL_UART_TRANSMIT_MASK)
            {
                if ( static_cast<char>(this->g_txData) == '\n' )
                {
                    ConsoleInfo << "uart: receiving \""<< codeg::ReplaceNonPrintableAsciiChar(this->g_outputBuffer) <<"\"" << std::endl;
                    this->g_outputBuffer.clear();
                }
                else
                {
                    this->g_outputBuffer.push_back( static_cast<char>(this->g_txData) );
                    if (this->g_outputBuffer.size() >= 20)
                    {
                        ConsoleInfo << "uart: (overflow) receiving \""<< codeg::ReplaceNonPrintableAsciiChar(this->g_outputBuffer) <<"\"" << std::endl;
                        this->g_outputBuffer.clear();
                    }
                }

                this->g_txFlag = true;
            }
        }

        if (this->g_inputBuffer.empty())
        {
            busses.get(CG_PROC_SPS1_BUS_BREAD1).set(0);
        }
        else
        {
            busses.get(CG_PROC_SPS1_BUS_BREAD1).set( this->g_inputBuffer.front() );
        }

        busses.get(CG_PROC_SPS1_BUS_BREAD2).set((this->g_rxFlag ? 0x01 : 0x00) | (this->g_txFlag ? 0x02 : 0x00));
    }
}

codeg::PeripheralType UART_peripheral_card_A_1_1::getType() const
{
    return codeg::PeripheralType::TYPE_PP1;
}

const std::string& UART_peripheral_card_A_1_1::getInputBuffer() const
{
    return this->g_inputBuffer;
}
void UART_peripheral_card_A_1_1::setInputBuffer(std::string input)
{
    this->g_inputBuffer = std::move(input);
    this->g_rxFlag = !this->g_inputBuffer.empty();
}

void UART_peripheral_card_A_1_1::clearOutputBuffer()
{
    this->g_outputBuffer.clear();
}
const std::string& UART_peripheral_card_A_1_1::getOutputBuffer() const
{
    return this->g_outputBuffer;
}

}//end codeg
