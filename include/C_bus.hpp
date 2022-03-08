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

#ifndef C_BUS_HPP_INCLUDED
#define C_BUS_HPP_INCLUDED

#include <cstdint>
#include <limits>

namespace codeg
{

using BitSize = uint8_t;

template<codeg::BitSize TBits>
class Bus
{
    static_assert(TBits != 0, "TBits can't be 0 !");
    static_assert(TBits <= sizeof(uint64_t)*8, "TBits can't be > 64 !");
public:
    Bus() = default;
    explicit Bus(uint64_t value) :
            g_bus(value & ~(std::numeric_limits<uint64_t>::max()<<TBits))
    {}
    ~Bus() = default;

    template<codeg::BitSize TObjBits>
    codeg::Bus<TBits>& operator =(const codeg::Bus<TObjBits>& r)
    {
        this->g_bus = r.get() & ~(std::numeric_limits<uint64_t>::max()<<TBits);
        return *this;
    }

    [[nodiscard]] codeg::BitSize getBitSize() const
    {
        return TBits;
    }

    void set(uint64_t value)
    {
        this->g_bus = value & ~(std::numeric_limits<uint64_t>::max()<<TBits);
    }
    [[nodiscard]] uint64_t get() const
    {
        return this->g_bus;
    }

private:
    uint64_t g_bus{0};
};

}//end codeg

#endif // C_BUS_HPP_INCLUDED
