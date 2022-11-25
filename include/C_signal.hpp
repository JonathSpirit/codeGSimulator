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

#include <map>
#include <functional>
#include <string>

namespace codeg
{

class Signal
{
public:
    using FunctionType = std::function<void(bool)>;

    Signal() = default;
    ~Signal() = default;

    void call(bool val);

    void attach(Signal::FunctionType func);
    void detach();

    [[nodiscard]] bool getValue() const;

private:
    Signal::FunctionType g_function{};
    bool g_value{false};
};

class SignalMap
{
public:
    SignalMap() = default;
    ~SignalMap() = default;

    [[nodiscard]] std::size_t getSize() const;

    template<class... Types>
    bool add(const std::string& key, Types... args)
    {
        return this->g_data.emplace( std::make_pair(key, args...) ).second;
    }
    inline bool add(const std::string& key)
    {
        return this->g_data.emplace(std::piecewise_construct, std::make_tuple(key), std::make_tuple()).second;
    }

    [[nodiscard]] bool exist(const std::string& key) const;

    [[nodiscard]] const codeg::Signal& get(const std::string& key) const;
    [[nodiscard]] codeg::Signal& get(const std::string& key);

private:
    std::map<std::string, codeg::Signal> g_data;
};

}//end codeg

#endif // C_SIGNAL_HPP_INCLUDED
