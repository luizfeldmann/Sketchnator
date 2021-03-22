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

#ifndef _POLYMORPHIC_SERIALIZABLE_OBJECT_H_
#define _POLYMORPHIC_SERIALIZABLE_OBJECT_H_

#include <memory>
#include <map>
#include <iostream>
#include "Serializable.h"

/// PolymorphicSerializable
/// Base for polymorphic types that wish to be serializable
/// Implements functions to self-serialize or self-deserialize and a "name" function
/// ====================================================================================================
class PolymorphicSerializable {
public:
    virtual ~PolymorphicSerializable() = default;
    virtual bool operator>>(SerializationOut&) const = 0;     // serializes itself to the output
    virtual bool operator<<(const SerializationInValue&) = 0; // (de)serializes itself from the input
    virtual const char* class_name() const = 0;               // returns the name of this type
};

DECLARE_SERIALIZATION_SCHEME(PolymorphicSerializable)

// Declaration of of the required members - must be placed in a 'public' section
#define DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS() \
    virtual bool operator>>(SerializationOut&) const; \
    virtual bool operator<<(const SerializationInValue&); \
    virtual const char* class_name() const;

// default implementation of polymorphic serialization - it just calls the default operators
#define IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(T) \
bool T::operator>>(SerializationOut& out) const { return ::operator<<(out, *this); } \
bool T::operator<<(const SerializationInValue& in) { return ::operator>>(in, *this); } \
const char* T::class_name() const { return #T; } \
const static bool is_registered_##T = PolymorphicObjectFactory::Register<T>(#T);

/// ====================================================================================================
/// Factory
/// ====================================================================================================
// Given the name of the class, produces an instance of that type

class PolymorphicObjectFactory
{
private:
    static std::map<std::string, std::function<PolymorphicSerializable*(void)>>& factory_functions();

public:
    template<class ObjectType>
    static typename std::enable_if<std::is_base_of<PolymorphicSerializable, ObjectType>::value, bool>::type Register(const char* name)
    {
        factory_functions().operator[](name) = []()->PolymorphicSerializable*{ return new ObjectType();};
        return true;
    }

    static PolymorphicSerializable* Make(const std::string& class_name);
};

/// ====================================================================================================
/// Operators of unique_ptr
/// ====================================================================================================

// OUTPUT
// ====================================================================================================
template<typename ObjectType> typename std::enable_if<std::is_base_of<PolymorphicSerializable, ObjectType>::value, bool>::type
operator<<(SerializationOut& out, const std::unique_ptr<ObjectType>& uptr)
{
    if (uptr)
        return uptr->operator>>(out); // calls the virtual serialization method

    // object is NULL
    if (!out.StartObject())
        return false;

    out["class_name"] = ""; // EMPTY CLASS NAME (could be "NULL" but we save space this way)

    if (!out.EndObject())
        return false;

    return true;
}

// INPUT
// ====================================================================================================
template<typename ObjectType> typename std::enable_if<std::is_base_of<PolymorphicSerializable, ObjectType>::value, bool>::type
operator>>(const SerializationInValue& in, std::unique_ptr<ObjectType>& uptr)
{
    // read the name of the class
    if (!in.IsObject())
        return false;

    const std::string class_name = in["class_name"].GetString();

    // pointer is NULL ?
    if (class_name.empty())
    {
        uptr.reset();
        return true;
    }

    // create instance from class name
    ObjectType* newobj = dynamic_cast<ObjectType*>( PolymorphicObjectFactory::Make(class_name) );
    if (!newobj)
        return false; // failed to construct

    // store instance in unique_ptr
    uptr.reset( newobj );

    return uptr->operator<<(in); // read additional data using virtual reader;
}

#endif // _POLYMORPHIC_SERIALIZABLE_OBJECT_H_
