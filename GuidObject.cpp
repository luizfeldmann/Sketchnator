/// Sketchnator
/// Copyright (C) 2021 Luiz Gustavo Pfitscher e Feldmann
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "GuidObject.h"
#include <random>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <iostream>

static uint64_t next_random()
{
    static std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    static std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<uint32_t>::max());

    return dis(gen);
}

Guid::Guid() :
    hi(next_random() + (next_random() << 32) ),
    lo(next_random() + (next_random() << 32) )
{
}

static const char delimiter = '-';

std::ostream& Guid::operator<<(std::ostream& os) const {
    os << std::setfill('0') << std::setw(16) << std::hex << hi;
    os << delimiter;
    os << std::setfill('0') << std::setw(16) << std::hex << lo;

    return os;
}

std::string Guid::to_string() const
{
    std::stringstream stream;
    operator<<(stream);

    return stream.str();
}

bool operator<<(SerializationOut& out, const Guid& g)
{
    return out << g.to_string();
}

bool operator>>(const SerializationInValue& in, Guid& data)
{
    std::string temp;
    if (!(in >> temp))
        return false;

    auto pos = temp.find(delimiter);
    if (pos == std::string::npos)
        return false;

    try {

        data.hi = std::stoull(temp.substr(0, pos),  NULL, 16);
        data.lo = std::stoull(temp.substr(pos + 1), NULL, 16);
        return true;
    }
    catch (std::invalid_argument& ex) {
        std::cerr << "Reading GUID OBJECT [std::invalid_argument]: " << ex.what() << std::endl;
        return false;
    }
    catch (std::out_of_range& ex) {
        std::cerr << "Reading GUID OBJECT [std::out_of_range]: " << ex.what() << std::endl;
        return false;
    }
}

BEGIN_SERIALIZATION_SCHEME(GuidObject)
    SERIALIZATION_FIELD(guid)
END_SERIALIZATION_SCHEME()
