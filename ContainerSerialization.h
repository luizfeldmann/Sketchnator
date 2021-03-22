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

#ifndef _CONTAINER_SERIALIZATION_H_
#define _CONTAINER_SERIALIZATION_H_

#include "Serializable.h"

/// =====================================================================================
/// IMPLEMENTATION OF INPUT/OUTPUT OPERATORS FOR (DE)SERIALIZING GENERIC CONTAINERS
/// =====================================================================================
template<typename, typename = void> struct has_iterator : std::false_type {};
template <typename Container> struct has_iterator<Container, std::enable_if_t<std::is_same<typename Container::value_type, typename Container::iterator::value_type>::value>> : std::true_type {};

template<typename, typename = void> struct has_const_iterator : std::false_type {};
template <typename Container> struct has_const_iterator<Container, std::enable_if_t<std::is_same<typename Container::value_type, typename std::remove_const<typename Container::const_iterator::value_type>::type>::value>> : std::true_type {};

// ======================================================================================
// OUTPUT
// ======================================================================================
template <typename T> typename std::enable_if<has_const_iterator<T>::value, bool>::type
operator<<(SerializationOut& out, const T& container) {
    if (!out.StartArray())
        return false;

    bool b = true;
    for (auto it = std::cbegin(container); it != std::cend(container); ++it)
        b &= out << *it;

    if (!out.EndArray())
        return false;

    return b;
}

// ======================================================================================
// INPUT
// ======================================================================================
template <template <typename, typename...> class Container, typename Content, typename Params> typename std::enable_if<std::is_same<Content, typename Container<Content, Params>::iterator::value_type>::value, bool>::type
operator>>(const SerializationInValue& in, Container<Content, Params>& container)
{
    static_assert(std::is_default_constructible<Content>::value, "Serialized container's elements must be default-constructible");

    if (!in.IsArray())
        return false;

    bool status = true;

    for (const rapidjson::Value& i_read : in.GetArray())
    {
        rapidjson::Document temp;
        const SerializationInValue siv(i_read, temp.GetAllocator());

        container.emplace_back(); // construct with default arguments
        if (    !(status &= (siv >> container.back()))        ) // write to value in the back
            break;

    }

    return status;
}

#endif // _CONTAINER_SERIALIZATION_H_
