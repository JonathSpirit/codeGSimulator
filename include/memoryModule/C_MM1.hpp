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

#ifndef C_MM1_HPP_INCLUDED
#define C_MM1_HPP_INCLUDED

#include "memoryModule/C_memoryModule.hpp"
#include <vector>

namespace codeg
{

class MM1 : public codeg::MemoryModule
{
protected:
    explicit MM1(codeg::MemorySize memorySize);
    ~MM1() override = default;

public:
    bool set(codeg::MemoryAddress address, uint8_t data) override;
    bool set(codeg::MemoryAddress address, uint8_t* data, codeg::MemorySize dataSize) override;
    bool get(codeg::MemoryAddress address, uint8_t& data) const override;
    bool get(codeg::MemoryAddress startAddress, codeg::MemorySize addressCount, uint8_t* data, codeg::MemorySize dataSize) const override;

    [[nodiscard]] codeg::MemoryModuleType getType() const override;

private:
    std::vector<uint8_t> g_data;
};

class MM1_64k : public codeg::MM1
{
public:
    MM1_64k();
    ~MM1_64k() override = default;
};

class MM1_32k : public codeg::MM1
{
public:
    MM1_32k();
    ~MM1_32k() override = default;
};

class MM1_16k : public codeg::MM1
{
public:
    MM1_16k();
    ~MM1_16k() override = default;
};

}//end codeg

#endif // C_MM1_HPP_INCLUDED