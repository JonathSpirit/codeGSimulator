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

#ifndef C_CODEG_HPP_INCLUDED
#define C_CODEG_HPP_INCLUDED

#include <cstdint>

#define CG_CODEGBINARYREV1_OPCODE_MASK 0x1F
#define CG_CODEGBINARYREV1_BUSSES_MASK 0xE0

namespace codeg
{

enum class CodegBinaryRev1 : uint8_t
{
    OPCODE_BWRITE1_CLK    = 0x00,
    OPCODE_BWRITE2_CLK    = 0x01,

    OPCODE_BPCS_CLK       = 0x02,

    OPCODE_OPLEFT_CLK     = 0x03,
    OPCODE_OPRIGHT_CLK    = 0x04,
    OPCODE_OPCHOOSE_CLK   = 0x05,

    OPCODE_PERIPHERAL_CLK = 0x06,

    OPCODE_BJMPSRC1_CLK   = 0x07,
    OPCODE_BJMPSRC2_CLK   = 0x08,
    OPCODE_BJMPSRC3_CLK   = 0x09,
    OPCODE_JMPSRC_CLK     = 0x0A,

    OPCODE_BRAMADD1_CLK   = 0x0B,
    OPCODE_BRAMADD2_CLK   = 0x0C,

    OPCODE_SPI_CLK        = 0x0D,
    OPCODE_BCFG_SPI_CLK   = 0x0E,

    OPCODE_STICK          = 0x0F,

    OPCODE_IF             = 0x10,
    OPCODE_IFNOT          = 0x11,

    OPCODE_RAMW           = 0x12,
    /*
    OPCODE_UOP            = 0x13,
    OPCODE_UOP            = 0x14,
    OPCODE_UOP            = 0x15,
    OPCODE_UOP            = 0x16,
    */
    OPCODE_LTICK          = 0x17
};
enum class CodegBinaryRev1Busses : uint8_t
{
    READABLE_SOURCE = 0x00,

    READABLE_BREAD1 = 0x20,
    READABLE_BREAD2 = 0x40,

    READABLE_RESULT = 0x60,
    READABLE_RAM = 0x80,
    READABLE_SPI = 0xA0,

    READABLE_EXT1 = 0xC0,
    READABLE_EXT2 = 0xE0
};

}//end codeg

#endif // C_CODEG_HPP_INCLUDED
