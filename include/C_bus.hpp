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
#include <map>
#include "C_error.hpp"

namespace codeg
{

using BitSize = uint8_t;

class Bus
{
public:
    explicit Bus(codeg::BitSize bitSize) :
            g_bus(0),
            g_bitSize(bitSize)
    {
        if ( (bitSize == 0) || (bitSize > sizeof(uint64_t)*8) )
        {
            throw codeg::Error("bitSize can't be 0 or > 64");
        }
    }
    explicit Bus(codeg::BitSize bitSize, uint64_t value) :
            g_bus(value & ~(std::numeric_limits<uint64_t>::max()<<bitSize)),
            g_bitSize(bitSize)
    {
        if ( (bitSize == 0) || (bitSize > sizeof(uint64_t)*8) )
        {
            throw codeg::Error("bitSize can't be 0 or > 64");
        }
    }
    ~Bus() = default;

    codeg::Bus& operator =(const codeg::Bus& r)
    {
        this->g_bus = r.get() & ~(std::numeric_limits<uint64_t>::max()<<this->g_bitSize);
        return *this;
    }

    [[nodiscard]] codeg::BitSize getBitSize() const
    {
        return this->g_bitSize;
    }

    void set(uint64_t value)
    {
        this->g_bus = value & ~(std::numeric_limits<uint64_t>::max()<<this->g_bitSize);
    }
    [[nodiscard]] uint64_t get() const
    {
        return this->g_bus;
    }

private:
    uint64_t g_bus;
    codeg::BitSize g_bitSize;
};

class BusMap
{
public:
    BusMap() = default;
    ~BusMap() = default;

    [[nodiscard]] std::size_t getSize() const
    {
        return this->g_data.size();
    }

    template<class... Types>
    bool add(const std::string& key, Types... args)
    {
        return this->g_data.emplace( std::make_pair(key, args...) ).second;
    }

    [[nodiscard]] bool exist(const std::string& key) const
    {
        return this->g_data.find(key) != this->g_data.cend();
    }

    [[nodiscard]] const codeg::Bus& get(const std::string& key) const
    {
        auto it = this->g_data.find(key);

        if (it != this->g_data.cend())
        {
            return it->second;
        }
        throw codeg::Error("Unknown bus : "+key);
    }
    [[nodiscard]] codeg::Bus& get(const std::string& key)
    {
        auto it = this->g_data.find(key);

        if (it != this->g_data.cend())
        {
            return it->second;
        }
        throw codeg::Error("Unknown bus : "+key);
    }

    [[nodiscard]] std::map<std::string, codeg::Bus>::const_iterator begin() const
    {
        return this->g_data.cbegin();
    }
    [[nodiscard]] std::map<std::string, codeg::Bus>::const_iterator end() const
    {
        return this->g_data.cend();
    }

private:
    std::map<std::string, codeg::Bus> g_data;
};

}//end codeg

#endif // C_BUS_HPP_INCLUDED
