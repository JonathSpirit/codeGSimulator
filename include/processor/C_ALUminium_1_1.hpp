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

#ifndef C_ALUMINIUM_1_1_HPP_INCLUDED
#define C_ALUMINIUM_1_1_HPP_INCLUDED

#include <cstdint>
#include "processor/C_alu.hpp"

namespace codeg
{

enum AluminiumOperations_1_1 : uint8_t
{
    ALU_1_1_OP_ADDITION = 0x00,
    ALU_1_1_OP_SUBTRACTION,

    ALU_1_1_OP_AND_BITWISE,
    ALU_1_1_OP_OR_BITWISE,
    ALU_1_1_OP_XOR_BITWISE,
    ALU_1_1_OP_INV_BITWISE,

    ALU_1_1_OP_AND_LOGICAL,
    ALU_1_1_OP_OR_LOGICAL,
    ALU_1_1_OP_XOR_LOGICAL,
    ALU_1_1_OP_INV_LOGICAL,

    ALU_1_1_OP_SHIFT_LEFT,
    ALU_1_1_OP_SHIFT_RIGHT,

    ALU_1_1_OP_STRICT_BIGGER,
    ALU_1_1_OP_STRICT_SMALLER,
    ALU_1_1_OP_BIGGER,
    ALU_1_1_OP_SMALLER,
    ALU_1_1_OP_EQUAL,

    ALU_1_1_OP_MULTIPLICATION,

    ALU_1_1_OP_2COMPLEMENT,
    ALU_1_1_OP_ROTATE,
    ALU_1_1_OP_ROTATE_LEFT,
    ALU_1_1_OP_ROTATE_RIGHT,

    ALU_1_1_OP_AOPL, //Write to the accumulator left/right and return operation left
    ALU_1_1_OP_AOPR, //Write to the accumulator left/right and return operation right
    ALU_1_1_OP_OPAL, //Write to the operation left/right and return accumulator left
    ALU_1_1_OP_OPAR  //Write to the operation left/right and return accumulator right
};

class Aluminium_1_1 : public codeg::Alu
{
public:
    Aluminium_1_1() = default;
    ~Aluminium_1_1() override = default;

    void setOperationLeft(uint8_t val) override;
    void setOperation(uint8_t val) override;
    void setOperationRight(uint8_t val) override;

private:
    void updateResult();

    uint8_t g_accumulatorLeft{0};
    uint8_t g_accumulatorRight{0};
    uint8_t g_operationLeft{0};
    uint8_t g_operationRight{0};
    uint8_t g_operation{0};
};

}//end codeg

#endif //C_ALUMINIUM_1_1_HPP_INCLUDED