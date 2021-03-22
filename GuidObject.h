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

#ifndef _GUID_OBJECT_
#define _GUID_OBJECT_

#include <ostream>
#include <cinttypes>
#include "Serializable.h"

struct Guid
{
private:
    uint64_t hi;
    uint64_t lo;
    friend bool operator>>(const SerializationInValue&, Guid&); // serializer is allowed to set the guid

    // never allow for copying
    Guid(const Guid&) = delete;             // non copy-constructible
    Guid& operator=(const Guid&) = delete;  // non copy-assignable

public:
    // random GUID construction
    Guid();

    // allows moving around
    Guid(Guid&&) = default;             // move-constructible
    Guid& operator=(Guid&&) = default;  // move-assignable

    // comparisons
    inline bool operator==(const Guid& other) const {
        return (hi == other.hi) && (lo == other.lo);
    }

    inline bool operator!=(const Guid& other) const {
        return (hi != other.hi) || (lo != other.lo);
    }

    inline bool operator<(const Guid& other) const {
        return (hi == other.hi) ? (lo < other.lo) : (hi < other.hi);
    }

    inline bool operator>(const Guid& other) const {
        return (hi == other.hi) ? (lo > other.lo) : (hi > other.hi);
    }

    // gets representation in format high_part-low_part where both parts are uint64 in hex
    std::ostream& operator<<(std::ostream& os) const;
    std::string to_string() const;
};

inline std::ostream& operator<<(std::ostream& os, const Guid& g) {
    return g.operator<<(os);
}

// allows for guid to be serialized
bool operator<<(SerializationOut&, const Guid&);
bool operator>>(const SerializationInValue&, Guid&);

// parent to all classes containing a GUID
struct GuidObject {
private:
    // let's make very explicit that this and derived classes may never be copied (unique identifiers must be *unique*)
    GuidObject(const GuidObject&) = delete;             // non copy-constructible
    GuidObject& operator=(const GuidObject&) = delete;  // non copy-assignable
public:
    // make moving semantics explicit
    GuidObject() = default;
    GuidObject(GuidObject&&) = default;             // move-constructible
    GuidObject& operator=(GuidObject&&) = default;  // move-assignable (required by the serializer)

    // GUID is:
    // PUBLIC - to allow comparing by anyone
    // non-CONST - to allow for moving the object
    Guid guid;
};

// OTHER FEATURES
DECLARE_SERIALIZATION_SCHEME(GuidObject) // allows for serializing a guid object and it's derived classes
template<typename T> using is_guid_object = std::is_base_of<GuidObject, T>;

#endif // _GUID_OBJECT_
