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

#ifndef C_ALU_HPP_INCLUDED
#define C_ALU_HPP_INCLUDED

#include <cstdint>

namespace codeg
{

class Alu
{
public:
    Alu() = default;
    virtual ~Alu() = default;

    virtual void setOperationLeft(uint8_t val) = 0;
    virtual void setOperation(uint8_t val) = 0;
    virtual void setOperationRight(uint8_t val) = 0;

    [[nodiscard]] uint8_t getResult() const
    {
        return this->_g_result;
    }

protected:
    uint8_t _g_result{0};
};

}//end codeg

#endif //C_ALU_HPP_INCLUDED