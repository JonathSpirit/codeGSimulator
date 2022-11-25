/////////////////////////////////////////////////////////////////////////////////
// Copyright 2021 Guillaume Guillet                                            //
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

#include "C_signal.hpp"
#include "C_error.hpp"

namespace codeg
{

//Signal

void Signal::call(bool val)
{
    this->g_value = val;
    if (this->g_function)
    {
        this->g_function(val);
    }
}

void Signal::attach(Signal::FunctionType func)
{
    this->g_function = std::move(func);
}
void Signal::detach()
{
    this->g_function = nullptr;
}

bool Signal::getValue() const
{
    return this->g_value;
}

//SignalMap

std::size_t SignalMap::getSize() const
{
    return this->g_data.size();
}

bool SignalMap::exist(const std::string& key) const
{
    return this->g_data.find(key) != this->g_data.end();
}

const codeg::Signal& SignalMap::get(const std::string& key) const
{
    auto it = this->g_data.find(key);

    if (it != this->g_data.end())
    {
        return it->second;
    }
    throw codeg::Error("unknown signal : "+key);
}
codeg::Signal& SignalMap::get(const std::string& key)
{
    auto it = this->g_data.find(key);

    if (it != this->g_data.end())
    {
        return it->second;
    }
    throw codeg::Error("unknown signal : "+key);
}


}//end codeg
