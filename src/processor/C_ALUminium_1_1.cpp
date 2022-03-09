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

#include "processor/C_ALUminium_1_1.hpp"

namespace codeg
{

void Aluminium_1_1::setOperationLeft(uint8_t val)
{
    switch (static_cast<codeg::AluminiumOperations_1_1>(this->g_operation))
    {
    case ALU_1_1_OP_AOPL:
    case ALU_1_1_OP_AOPR:
        this->g_accumulatorLeft = val;
        break;
    case ALU_1_1_OP_OPAL:
    case ALU_1_1_OP_OPAR:
    default:
        this->g_operationLeft = val;
        break;
    }
    this->updateResult();
}
void Aluminium_1_1::setOperation(uint8_t val)
{
    this->g_operation = val;
    this->updateResult();
}
void Aluminium_1_1::setOperationRight(uint8_t val)
{
    switch (static_cast<codeg::AluminiumOperations_1_1>(this->g_operation))
    {
    case ALU_1_1_OP_AOPL:
    case ALU_1_1_OP_AOPR:
        this->g_accumulatorRight = val;
        break;
    case ALU_1_1_OP_OPAL:
    case ALU_1_1_OP_OPAR:
    default:
        this->g_operationRight = val;
        break;
    }
    this->updateResult();
}

void Aluminium_1_1::updateResult()
{
    switch (static_cast<codeg::AluminiumOperations_1_1>(this->g_operation))
    {
    case ALU_1_1_OP_ADDITION:
        this->_g_result = this->g_operationLeft + this->g_operationRight;
        break;
    case ALU_1_1_OP_SUBTRACTION:
        this->_g_result = this->g_operationLeft - this->g_operationRight;
        break;
    case ALU_1_1_OP_AND_BITWISE:
        this->_g_result = this->g_operationLeft & this->g_operationRight;
        break;
    case ALU_1_1_OP_OR_BITWISE:
        this->_g_result = this->g_operationLeft | this->g_operationRight;
        break;
    case ALU_1_1_OP_XOR_BITWISE:
        this->_g_result = this->g_operationLeft ^ this->g_operationRight;
        break;
    case ALU_1_1_OP_INV_BITWISE:
        this->_g_result = ~this->g_operationLeft;
        break;
    case ALU_1_1_OP_AND_LOGICAL:
        this->_g_result = this->g_operationLeft && this->g_operationRight ? 1 : 0;
        break;
    case ALU_1_1_OP_OR_LOGICAL:
        this->_g_result = this->g_operationLeft || this->g_operationRight ? 1 : 0;
        break;
    case ALU_1_1_OP_XOR_LOGICAL:
        this->_g_result = (this->g_operationLeft>0 ? 1 : 0) ^ (this->g_operationRight>0 ? 1 : 0);
        break;
    case ALU_1_1_OP_INV_LOGICAL:
        this->_g_result = this->g_operationLeft ? 0 : 1;
        break;
    case ALU_1_1_OP_SHIFT_LEFT:
        this->_g_result = this->g_operationLeft << this->g_operationRight;
        break;
    case ALU_1_1_OP_SHIFT_RIGHT:
        this->_g_result = this->g_operationLeft >> this->g_operationRight;
        break;
    case ALU_1_1_OP_STRICT_BIGGER:
        this->_g_result = this->g_operationLeft > this->g_operationRight ? 1 : 0;
        break;
    case ALU_1_1_OP_STRICT_SMALLER:
        this->_g_result = this->g_operationLeft < this->g_operationRight ? 1 : 0;
        break;
    case ALU_1_1_OP_BIGGER:
        this->_g_result = this->g_operationLeft >= this->g_operationRight ? 1 : 0;
        break;
    case ALU_1_1_OP_SMALLER:
        this->_g_result = this->g_operationLeft <= this->g_operationRight ? 1 : 0;
        break;
    case ALU_1_1_OP_EQUAL:
        this->_g_result = this->g_operationLeft == this->g_operationRight ? 1 : 0;
        break;
    case ALU_1_1_OP_MULTIPLICATION:
        this->_g_result = this->g_operationLeft * this->g_operationRight;
        break;
    case ALU_1_1_OP_2COMPLEMENT:
        this->_g_result = (~this->g_operationLeft)+1;
        break;
    case ALU_1_1_OP_ROTATE:
    {
        uint32_t x = this->g_operationLeft;
        x = ((x & 0x55555555) << 1) | ((x & 0xAAAAAAAA) >> 1);
        x = ((x & 0x33333333) << 2) | ((x & 0xCCCCCCCC) >> 2);
        x = ((x & 0x0F0F0F0F) << 4) | ((x & 0xF0F0F0F0) >> 4);
        x = ((x & 0x00FF00FF) << 8) | ((x & 0xFF00FF00) >> 8);
        x = ((x & 0x0000FFFF) << 16) | ((x & 0xFFFF0000) >> 16);
        this->_g_result = x >> (32 - 8);
    }
        break;
    case ALU_1_1_OP_ROTATE_LEFT:
        this->_g_result = this->g_operationLeft;

        for (uint8_t i=0; i<this->g_operationRight; ++i)
        {
            this->_g_result = ((this->_g_result & 0x80) ? 0x01 : 0x00) | (this->_g_result << 1);
        }
        break;
    case ALU_1_1_OP_ROTATE_RIGHT:
        this->_g_result = this->g_operationLeft;

        for (uint8_t i=0; i<this->g_operationRight; ++i)
        {
            this->_g_result = ((this->_g_result & 0x01) ? 0x80 : 0x00) | (this->_g_result >> 1);
        }
        break;
    case ALU_1_1_OP_AOPL:
        this->_g_result = this->g_operationLeft;
        break;
    case ALU_1_1_OP_AOPR:
        this->_g_result = this->g_operationRight;
        break;
    case ALU_1_1_OP_OPAL:
        this->_g_result = this->g_accumulatorLeft;
        break;
    case ALU_1_1_OP_OPAR:
        this->_g_result = this->g_accumulatorRight;
        break;
    default:
        this->_g_result = 0x00;
        break;
    }
}

}//end codeg
