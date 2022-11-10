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

#include "motherboard/motherboards.hpp"
#include <unordered_map>

namespace codeg
{

namespace
{

std::unordered_map<std::string, std::unique_ptr<MotherboardClassTypeBase> > gData;

}//end

void RegisterNewMotherboardType(std::unique_ptr<MotherboardClassTypeBase>&& classType)
{
    auto it = gData.find(classType->getType());
    if (it == gData.end())
    {
        std::string name = classType->getType();
        gData[std::move(name)] = std::move(classType);
    }
}
Motherboard* GetNewMotherboard(const std::string& type)
{
    auto it = gData.find(type);
    if (it != gData.end())
    {
        return it->second->create();;
    }
    return nullptr;
}
std::size_t GetMotherboardTypeSize()
{
    return gData.size();
}

}//end codeg
