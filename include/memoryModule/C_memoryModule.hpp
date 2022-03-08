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

#ifndef C_MEMORYMODULE_HPP_INCLUDED
#define C_MEMORYMODULE_HPP_INCLUDED

#include <cstdint>

namespace codeg
{

using AddressBusSize = uint8_t;
using MemorySize = std::size_t;

class MemoryModule
{
public:
    explicit MemoryModule(MemorySize memorySize) :
            _g_memorySize(memorySize)
    {}
    virtual ~MemoryModule() = default;

    [[nodiscard]] MemorySize getMemorySize() const
    {
        return this->_g_memorySize;
    }

    [[nodiscard]] MemorySize getMaxMemorySize(AddressBusSize size) const
    {
        MemorySize wanted = static_cast<MemorySize>(1) << size;
        return this->_g_memorySize > wanted ? wanted : this->_g_memorySize;
    }
    [[nodiscard]] MemorySize getAddressBusSize() const
    {
        return this->_g_memorySize;
    }

protected:
    MemorySize _g_memorySize;
};

}//end codeg

#endif // C_MEMORYMODULE_HPP_INCLUDED