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

#ifndef _SERIALIZATION_CONTEXT_H_
#define _SERIALIZATION_CONTEXT_H_
#include <memory>
#include <map>
#include <typeindex>

// Serialization Context
// An object that may be created when a (de)serializer is used
// It stores certain internal information that is shared between serializer methods
// It may be used to persist information between multiple invocations of the serializer
// There may be only one context at each time
// ====================================================================================

class SerializationContextObject {
public:
    virtual ~SerializationContextObject() = default;
};

class SerializationContext {
private:
    SerializationContext(const SerializationContext&) = delete; // non copy-constructible
    SerializationContext(SerializationContext&&) = delete;      // non move-constructible
    SerializationContext& operator=(const SerializationContext&) = delete;  // non copy-assignable
    SerializationContext& operator=(SerializationContext&&) = delete;       // non move-assignable

    std::map<std::type_index, std::unique_ptr<SerializationContextObject>> stored_data;

public:
    SerializationContext();
    ~SerializationContext();
    static SerializationContext& Get();

    template <class T>
    typename std::enable_if<std::is_base_of<SerializationContextObject, T>::value, T&>::type Get()
    {
        const auto& type = typeid(T);
        decltype(stored_data)::iterator found = stored_data.find(type);
        if (found != stored_data.end())
            return dynamic_cast<T&>( *(found->second.get()) );

        T* newobj = new T();
        if (stored_data.insert( { type, std::unique_ptr<SerializationContextObject>( newobj ) } ).second)
            return *newobj;

        delete newobj;
        throw std::runtime_error("SerializationContext::Get could not instantiate object");
    }
};

#endif // _SERIALIZATION_CONTEXT_H_
