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

#include "memoryModule/C_MM1.hpp"

namespace codeg
{

///MM1
MM1::MM1(codeg::MemorySize memorySize) :
        codeg::MemoryModule(memorySize)
{
}

bool MM1::set(codeg::MemoryAddress address, uint8_t data)
{
    if (address < this->g_data.size())
    {
        this->g_data[address] = data;
        return true;
    }
    return false;
}
bool MM1::set(codeg::MemoryAddress address, uint8_t* data, codeg::MemorySize dataSize)
{
    if (dataSize == 0)
    {
        return false;
    }

    if ( (address < this->g_data.size()) && (address+dataSize < this->g_data.size()) )
    {
        for (codeg::MemorySize i=0; i<dataSize; ++i)
        {
            this->g_data[address+i] = data[i];
        }
        return true;
    }
    return false;
}
bool MM1::get(codeg::MemoryAddress address, uint8_t& data) const
{
    if (address < this->g_data.size())
    {
        data = this->g_data[address];
        return true;
    }
    return false;
}
bool MM1::get(codeg::MemoryAddress startAddress, codeg::MemorySize addressCount, uint8_t* data, codeg::MemorySize dataSize) const
{
    if ((dataSize == 0) || (addressCount == 0) || (dataSize<addressCount))
    {
        return false;
    }

    if ( (startAddress < this->g_data.size()) && (startAddress+addressCount < this->g_data.size()) )
    {
        for (codeg::MemorySize i=0; i<addressCount; ++i)
        {
            data[i] = this->g_data[startAddress+i];
        }
        return true;
    }
    return false;
}

codeg::MemoryModuleType MM1::getType() const
{
    return codeg::MemoryModuleType::TYPE_MM1;
}

///MM1_xk
MM1_64k::MM1_64k() :
        codeg::MM1(1 << 16)
{}
MM1_32k::MM1_32k() :
        codeg::MM1(1 << 15)
{}
MM1_16k::MM1_16k() :
        codeg::MM1(1 << 14)
{}

}//end codeg
