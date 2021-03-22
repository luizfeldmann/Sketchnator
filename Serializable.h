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

#ifndef _SERIALIZABLE_H_
#define _SERIALIZABLE_H_

// ==================================================================================================
// OUTPUT
// ==================================================================================================
#include <ostream>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>
#include <functional>

class SerializationOut : public rapidjson::PrettyWriter<rapidjson::OStreamWrapper>
{
public:
    using WriterType = rapidjson::PrettyWriter<rapidjson::OStreamWrapper>;
    using WriterType::WriterType; // import constructors

    // value writing
    inline bool operator<<(bool data) {
        return Bool(data);
    }

    inline bool operator<<(signed int data) {
        return Int(data);
    }

    inline bool operator<<(unsigned int data) {
        return Uint(data);
    }

    inline bool operator<<(int64_t data) {
        return Int64(data);
    }

    inline bool operator<<(uint64_t data) {
        return Uint64(data);
    }

    inline bool operator<<(double data) {
        return Double(data);
    }

    inline bool operator<<(float data) {
        return Double(data);
    }

    inline bool operator<<(const char* data) {
        return String(data, strlen(data));
    }

    inline bool operator<<(const std::string& data) {
        return String(data.data(), data.size());
    }

    template<typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
    inline bool operator<<(T data)
    {
        return Int64((uint64_t)data);
    }

    // key writing
    class ValueAssignContext {
    private:
        inline ValueAssignContext(SerializationOut& so, const char* k) : out(so), key(k) {}
        ValueAssignContext(const ValueAssignContext&) = delete;         // non copyable
        ValueAssignContext(ValueAssignContext&&) = delete;              // non movable
        ValueAssignContext& operator=(const ValueAssignContext&);       // non copy-assignable
        ValueAssignContext& operator=(ValueAssignContext&&) = delete;   // non move-assignable

        friend class SerializationOut;

    protected:
        SerializationOut& out;
        const std::string key;

    public:
        template <class T>
        bool operator=(const T& data) {
            if (!out.Key(key.c_str()))
                return false;

            return (out << data);
        }
    };

    inline ValueAssignContext operator[](const char* key) {
        return ValueAssignContext(*this, key);
    };
};

template<class StreamType, typename = typename std::enable_if<std::is_base_of<std::ostream, StreamType>::value, StreamType>::type>
struct AutoSerializationOutBase
{
    StreamType os;
    rapidjson::OStreamWrapper osw;

    inline AutoSerializationOutBase(StreamType&& src) :
        os(std::move(src)),
        osw(os)
    {
    }
};

template <class StreamType>
class AutoSerializationOut : protected AutoSerializationOutBase<StreamType>, public SerializationOut {
public:
    AutoSerializationOut(StreamType&& src) :
        AutoSerializationOutBase<StreamType>(std::move(src)),
        SerializationOut(AutoSerializationOutBase<StreamType>::osw)
    {
    }
};

// ==================================================================================================
// INPUT
// ==================================================================================================
#include <rapidjson/document.h>
#include <istream>
#include <rapidjson/istreamwrapper.h>

class SerializationInValue : public rapidjson::Value
{
public:
    // import constructors
    using rapidjson::Value::Value;

    SerializationInValue(rapidjson::Value&& src) :
        rapidjson::Value(std::move(src))
    {
    }

    // read operators
    inline bool operator>>(bool& data) const {
        if (!IsBool())
            return false;

        data = GetBool();
        return true;
    }

    inline bool operator>>(unsigned char& data) const {
        if (!IsInt())
            return false;

        data = (unsigned char)GetInt();
        return true;
    }

    inline bool operator>>(signed short& data) const {
        if (!IsInt())
            return false;

        data = (signed short)GetInt();
        return true;
    }

    inline bool operator>>(unsigned short& data) const {
        if (!IsUint())
            return false;

        data = (unsigned short)GetUint();
        return true;
    }

    inline bool operator>>(signed int& data) const {
        if (!IsInt())
            return false;

        data = GetInt();
        return true;
    }

    inline bool operator>>(unsigned int& data) const {
        if (!IsUint())
            return false;

        data = GetUint();
        return true;
    }

    inline bool operator>>(int64_t& data) const {
        if (!IsInt64())
            return false;

        data = GetInt64();
        return true;
    }

    inline bool operator>>(uint64_t& data) const {
        if (!IsUint64())
            return false;

        data = GetUint64();
        return true;
    }

    inline bool operator>>(float& data) const {
        if (IsFloat())
            data = GetFloat();
        else if (IsDouble())
            data = (float)GetDouble();
        else
            return false;

        return true;
    }

    inline bool operator>>(double& data) const {
        if (IsDouble())
            data = GetDouble();
        else if (IsFloat())
            data = (double)GetFloat();
        else
            return false;

        return true;
    }

    inline bool operator>>(std::string& data) const {
        if (!IsString())
            return false;

        data = GetString();
        return true;
    }

    inline bool operator>>(char* data) const {
        if (!IsString())
            return false;

        strcpy(data, GetString());
        return true;
    }

    template<typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
    inline bool operator>>(T& data) const {
        if (!IsUint64())
            return false;

        data = (T)GetUint64();
        return true;
    }
};

struct AutoSerializationInBase
{
    rapidjson::Document doc;

    template<class StreamType, typename = typename std::enable_if<std::is_base_of<std::istream, StreamType>::value, StreamType>::type>
    inline AutoSerializationInBase(StreamType&& src)
    {
        rapidjson::IStreamWrapper isw(src);
        doc.ParseStream(isw);
    }
};

struct AutoSerializationIn : protected AutoSerializationInBase, public SerializationInValue
{
public:
    template<class StreamType, typename = typename std::enable_if<std::is_base_of<std::istream, StreamType>::value, StreamType>::type>
    AutoSerializationIn(StreamType&& src) :
        AutoSerializationInBase(std::move(src)),
        SerializationInValue( std::forward<rapidjson::Value>(doc) )
    {
    }
};

// ==================================================================================================
// SCHEME
// ==================================================================================================
#include <vector>
#include <memory>

template <typename ObjectType>
struct SerializableMemberBase
{
    const std::string name;

    inline SerializableMemberBase(const std::string& key) : name(key) {};
    inline virtual ~SerializableMemberBase() = default;

    virtual bool write_value(const ObjectType& instance, SerializationOut& out) const = 0;
    virtual bool read_value(ObjectType& instance, const SerializationInValue& in) const = 0;
};

template <typename ObjectType, typename MemberType>
struct SerializableMember : public SerializableMemberBase<ObjectType>
{
protected:
    MemberType ObjectType::* const PointerToMemberType;

public:
    inline SerializableMember(const std::string& key, MemberType ObjectType::*ptr) : SerializableMemberBase<ObjectType>(key), PointerToMemberType(ptr) {};

    virtual bool write_value(const ObjectType& instance, SerializationOut& out) const {
        return out[SerializableMemberBase<ObjectType>::name.c_str()] = instance.*PointerToMemberType;
    }

    virtual bool read_value(ObjectType& instance, const SerializationInValue& in) const {
        if (!in.IsObject())
            return false;

        rapidjson::Document temp;
        const SerializationInValue siv( in[SerializableMemberBase<ObjectType>::name.c_str()], temp.GetAllocator() );

        if (!(siv >> instance.*PointerToMemberType))
            return false;

        return true;
    }
};

template <typename ObjectType, typename ReturnType>
struct SerializableMethodMember : public SerializableMemberBase<ObjectType>
{
protected:
    ReturnType(ObjectType::*PointerToMethod)() const;

public:
    inline SerializableMethodMember(const std::string& key, ReturnType(ObjectType::*met)() const) : SerializableMemberBase<ObjectType>(key), PointerToMethod(met) {};

    virtual bool write_value(const ObjectType& instance, SerializationOut& out) const {
        return out[SerializableMemberBase<ObjectType>::name.c_str()] = (instance.*PointerToMethod)();
    }

    virtual bool read_value(ObjectType& instance, const SerializationInValue& in) const {
        return true; // there is no place to save it, so just ignore it
    }
};


class SerializationSchemeBase {};

template <typename ObjectType>
class SerializationScheme : public SerializationSchemeBase, public std::vector< std::unique_ptr<SerializableMemberBase<ObjectType>> >
{
protected:
    struct SchemeDelegate {
        std::function<bool(const ObjectType&, SerializationOut&)> const writer;
        std::function<bool(ObjectType&, const SerializationInValue&)> const reader;
    };
    std::vector<SchemeDelegate> inheritedSchemes;

public:
    SerializationScheme() = default;

    // writes from memory to the stream (SerializationOut)
    bool write(const ObjectType& instance, SerializationOut& out, bool IsDelegateCall = false) const
    {
        // create object only if necessary (this function may be a delegate, so the caller already created the object)
        if (!IsDelegateCall)
            if (!out.StartObject())
                return false;

        for (const SchemeDelegate& delegate : inheritedSchemes)
            if (delegate.writer)
                if (!delegate.writer(instance, out))
                    return false;

        bool status = true;
        for (const std::unique_ptr<SerializableMemberBase<ObjectType>>& member : *this)
            status &= member->write_value(instance, out);

        if (!IsDelegateCall)
            if (!out.EndObject())
                return false;

        return status;
    }

    // reads from the stream (SerializationIn) to memory
    bool read(ObjectType& instance, const SerializationInValue& in) const
    {
        for (const SchemeDelegate& delegate : inheritedSchemes)
            if (delegate.reader)
                if (!delegate.reader(instance, in))
                    return false;

        if (!in.IsObject())
            return false;

        bool status = true;
        for (const std::unique_ptr<SerializableMemberBase<ObjectType>>& member : *this)
            status &= member->read_value(instance, in);

        return status;
    }

    // list a property in this scheme
    SerializationScheme& operator<<(SerializableMemberBase<ObjectType>*&& insert)
    {
        this->emplace_back( std::unique_ptr<SerializableMemberBase<ObjectType>>(insert) );
        return *this;
    }

    // list properties from base class here
    template<typename ParentClass, typename = typename std::enable_if<std::is_base_of<ParentClass, ObjectType>::value, void>::type>
    SerializationScheme& operator<<(const SerializationScheme<ParentClass>& insert)
    {
        inheritedSchemes.emplace_back(SchemeDelegate{
            .writer = [&insert](const ObjectType& instance, SerializationOut& out)->bool{
                return insert.write(dynamic_cast<const ParentClass&>(instance), out, true);
            },
            .reader = [&insert](ObjectType& instance, const SerializationInValue& in)->bool{
                return insert.read(dynamic_cast<ParentClass&>(instance), in);
            }
        });

        return *this;
    }
};

template<class ObjectType> const SerializationScheme<ObjectType>& GetSerializationScheme();
template<class SchemeType> struct is_serialization_scheme : std::conditional< std::is_base_of<SerializationSchemeBase, SchemeType>::value , std::true_type, std::false_type>::type {};

// ==================================================================================================
// MACROS
// ==================================================================================================
#define DECLARE_SERIALIZATION_SCHEME(T) \
template<> const SerializationScheme<T>& GetSerializationScheme<T>(); \
bool operator>>(const SerializationInValue& in, T& data); \
bool operator<<(SerializationOut& out, const T& data);

#define BEGIN_SERIALIZATION_SCHEME(T) \
bool operator>>(const SerializationInValue& in, T& data) { return GetSerializationScheme<T>().read(data, in);   } \
bool operator<<(SerializationOut& out, const T& data)    { return GetSerializationScheme<T>().write(data, out); } \
template<> const SerializationScheme<T>& GetSerializationScheme<T>() { \
using ObjectType = T; \
    static const SerializationScheme<ObjectType> scheme(std::move( \
        SerializationScheme<ObjectType>()

#define END_SERIALIZATION_SCHEME() \
    )); \
    return scheme; \
}

#define SERIALIZATION_METHOD(name) << new SerializableMethodMember<ObjectType, decltype(std::declval<ObjectType>().name())>(#name, &ObjectType::name)
#define SERIALIZATION_FIELD(name) << new SerializableMember<ObjectType, decltype(ObjectType::name)>(#name, &ObjectType::name)
#define SERIALIZATION_INHERIT(name) << GetSerializationScheme<name>()

#endif // _SERIALIZABLE_H_
