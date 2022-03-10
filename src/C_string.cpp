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

#include "C_string.hpp"
#include <sstream>

namespace codeg
{

std::string CleanString(const std::string& str)
{
    std::string result;
    bool extraSpace = true;

    result.reserve(str.size());

    for (std::size_t i=0; i<str.size(); ++i)
    {
        if ( str[i] == ' ' )
        {
            if (!extraSpace)
            {
                result += ' ';
                extraSpace = true;
            }
        }
        else
        {
            result += str[i];
            extraSpace = false;
        }
    }

    return result;
}
size_t Split(const std::string& str, std::vector<std::string>& buff, char delimiter)
{
    std::string buffStr;
    std::istringstream strStream(str);
    while (std::getline(strStream, buffStr, delimiter))
    {
        buff.push_back(buffStr);
    }
    return buff.size();
}

std::string ValueToHex(uint32_t val, unsigned int hexSize, bool removeExtraZero, bool removePrefix)
{
    if (hexSize==0)
    {
        return "";
    }
    else if (hexSize>8)
    {
        hexSize = 8;
    }

    char buff[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    std::string out = removePrefix ? "" : "0x";

    bool extraZeroFlag = removeExtraZero;

    uint32_t mask = 0x0000000F << (4*(hexSize-1));
    for (unsigned int i=0; i<hexSize; ++i)
    {
        char buffChar = buff[(val&mask) >> 4*((hexSize-1)-i)];
        if (extraZeroFlag)
        {
            if (buffChar != '0')
            {
                extraZeroFlag = false;
                out += buffChar;
            }
        }
        else
        {
            out += buffChar;
        }
        mask >>= 4;
    }
    if (extraZeroFlag)
    {//The result is only 0
        out += '0';
    }

    return out;
}

}//end codeg
