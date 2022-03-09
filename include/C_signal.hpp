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

#ifndef C_SIGNAL_HPP_INCLUDED
#define C_SIGNAL_HPP_INCLUDED

#include <cstdint>
#include <map>
#include "C_error.hpp"

namespace codeg
{

class SignalCapable
{
public:
    SignalCapable() = default;
};

using SignalFunctionPtr = void (codeg::SignalCapable::*)(bool);

struct Signal
{
    void call(bool val)
    {
        this->_value = val;
        if (this->_func && this->_obj)
        {
            (this->_obj->*this->_func)(val);
        }
    }

    template<class TFunc>
    void attach(codeg::SignalCapable* obj, TFunc func)
    {
        this->_obj = obj;
        this->_func = static_cast<codeg::SignalFunctionPtr>(func);
    }

    void detach()
    {
        this->_obj = nullptr;
        this->_func = nullptr;
    }

    codeg::SignalFunctionPtr _func{nullptr};
    codeg::SignalCapable* _obj{nullptr};
    bool _value{false};
};

class SignalMap
{
public:
    SignalMap() = default;
    ~SignalMap() = default;

    [[nodiscard]] std::size_t getSize() const
    {
        return this->g_data.size();
    }

    template<class... Types>
    bool add(const std::string& key, Types... args)
    {
        return this->g_data.emplace( std::make_pair(key, args...) ).second;
    }
    bool add(const std::string& key)
    {
        return this->g_data.emplace(std::piecewise_construct, std::make_tuple(key), std::make_tuple()).second;
    }

    [[nodiscard]] bool exist(const std::string& key) const
    {
        return this->g_data.find(key) != this->g_data.cend();
    }

    [[nodiscard]] const codeg::Signal& get(const std::string& key) const
    {
        auto it = this->g_data.find(key);

        if (it != this->g_data.cend())
        {
            return it->second;
        }
        throw codeg::Error("Unknown signal : "+key);
    }
    [[nodiscard]] codeg::Signal& get(const std::string& key)
    {
        auto it = this->g_data.find(key);

        if (it != this->g_data.cend())
        {
            return it->second;
        }
        throw codeg::Error("Unknown signal : "+key);
    }

private:
    std::map<std::string, codeg::Signal> g_data;
};

}//end codeg

#endif // C_SIGNAL_HPP_INCLUDED
