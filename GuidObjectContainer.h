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

#ifndef _GUID_OBJECT_CONTAINER_H_
#define _GUID_OBJECT_CONTAINER_H_

#include "GuidObject.h"
#include "ContainerSerialization.h"
#include "SerializationContext.h"

// test if container iterator points to a guid object
template<typename Iter> using is_iterator_to_guid_object = is_guid_object<typename std::remove_cv<typename std::iterator_traits<Iter>::value_type>::type>;

// GuidObjectContainer
// DERIVES FROM A GENERIC CONTAINER AND IMPLEMENTS A SPECIAL DE-SERIALIZATION SCHEME TO ALLOW FOR ITERATOR RECOVERY
// ===================================================================================================================
template<typename Container>
class GuidObjectContainer : public std::enable_if<is_iterator_to_guid_object<typename Container::const_iterator>::value, Container>::type {
public:
    using Container::Container;
};

// GuidObjectContainerLookupTable
// A table that associates, for each type of iterator, all the de-serialized containers that take such iterator
// This allows for iterators serialized as Guids to find its parent containers
// ===================================================================================================================
class GuidObjectContainerLookupTableBase : public SerializationContextObject {
public:
    virtual ~GuidObjectContainerLookupTableBase() = default; // class must be polymorphic
};

template <typename iterator_type>
class GuidObjectContainerLookupTable : public GuidObjectContainerLookupTableBase
{
private:
    struct container_info {
        std::function<iterator_type()> const begin;
        std::function<iterator_type()> const end;
    };

    std::list<container_info> registered_containers;

public:
    GuidObjectContainerLookupTable() = default;
    ~GuidObjectContainerLookupTable() = default;

    template<typename Container>
    typename std::enable_if<std::is_same<typename Container::iterator, iterator_type>::value, void>::type Register(Container& c) {
        registered_containers.emplace_back( container_info{
            .begin = [&c]()->iterator_type{ return c.begin(); },
            .end = [&c]()->iterator_type{ return c.end(); },
        });
    }

    bool find(const Guid& guid, iterator_type& found)
    {
        for (const container_info& info : registered_containers)
        {
            if (!info.begin || !info.end)
                continue;

            found = std::find_if(info.begin(), info.end(), [&guid](const auto& testIter)->bool
            {
                return (testIter.guid == guid);
            });

            if (found != info.end())
                return true;
        }

        return false;
    }

    void clear() { registered_containers.clear(); }
};

template <typename T>
bool operator>> (const SerializationInValue& in, GuidObjectContainer<T>& container)
{
    SerializationContext::Get().Get<GuidObjectContainerLookupTable<typename T::iterator>>().Register(container);
    return operator>>(in, (T&)container); // call the default serializer
}


// ITERATOR OUTPUT
// SERIALIZING AN ITERATOR TO GUID OBJECT
// ===================================================================================================================
// when serializing an iterator to a guid object, simply write the text value of the guid
template<typename Iter> typename std::enable_if<is_iterator_to_guid_object<Iter>::value, bool>::type
operator<<(SerializationOut& out, const Iter& iter)
{
    return (out << iter->guid);
}

// ITERATOR INPUT
// DESERIALIZING AN ITERATOR TO GUID OBJECT
// ===================================================================================================================
template<typename IterType> typename std::enable_if<is_iterator_to_guid_object<IterType>::value, bool>::type
operator>>(const SerializationInValue& in, IterType& iter)
{
    Guid guid;
    if (!(in >> guid))
        return false;

    return SerializationContext::Get().Get<GuidObjectContainerLookupTable<IterType>>().find(guid, iter);
}

#endif // _GUID_OBJECT_CONTAINER_H_
