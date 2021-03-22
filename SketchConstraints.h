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

#ifndef _SKETCH_CONSTRAINTS_H_
#define _SKETCH_CONSTRAINTS_H_

#include "SketchEquations.h"
#include "SketchAnnotation.h"
#include <vector>
#include <memory>

// GENERIC CONSTRAINT
// Associates entities (points, lines, circles) in a specific way
// It implements IsAssociatedTo function that enables identifying relationships
// This is important because then an object is deleted, no dangling constraints may be left and should be deleted too
// ========================================================================================
struct SketchConstraintGuiData;
class SketchConstraint : public SketchAnnotation
{
public:
    virtual ~SketchConstraint() = default;

    // Tests if constraint is somehow related to object (when deleting object, must not leave dangling constraints behind)
    virtual bool IsAssociatedTo(const SketchPointList::iterator& point) const;
    virtual bool IsAssociatedTo(const SketchLineList::iterator& line) const;
    virtual bool IsAssociatedTo(const SketchCircleList::iterator& circle) const;

    // Gets the equation used to solve this constraint numerically
    // The returned object is "independent" in the sense you don't need to get a new one on each iteration
    // The equation contains references to the concerned parameters, so changing the the sketch will automatically update the equations
    virtual ConstraintEquation* GetEquation();
    virtual std::unique_ptr<SketchConstraintGuiData> GetGuiData();

    // Automatically implemented functions to serialize the constraints
    DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(SketchConstraint)

// CONSTRAINT LIST
// This is a container for SketchConstraint
// Constraints are stored as pointers to allow polymorphism
// A list of unique_ptr is used to avoid leaks
// =========================================================================================
class SketchConstraintList : public std::list<std::unique_ptr<SketchConstraint>>
{
public:
    iterator FindAssociated(const SketchPointList::iterator& point, const iterator& first);
    iterator FindAssociated(const SketchLineList::iterator& line, const iterator& first);
    iterator FindAssociated(const SketchCircleList::iterator& circle, const iterator& first);

    void erase_associated(const SketchPointList::iterator& point);
    void erase_associated(const SketchLineList::iterator& line);
    void erase_associated(const SketchCircleList::iterator& circle);

    iterator add(SketchConstraint* sc);
};

inline bool operator>>(const SerializationInValue& in, SketchConstraintList& data) { return operator>>(in,  (std::list<std::unique_ptr<SketchConstraint>>&)data); }
inline bool operator<<(SerializationOut& out, const SketchConstraintList& data)    { return operator<<(out, (const std::list<std::unique_ptr<SketchConstraint>>&)data); }

// COMMON BASES FOR CONSTRAINTS
// ============================================================================

// TwoPointConstraint
class TwoPointConstraint : public SketchConstraint
{
protected:
    TwoPointConstraint() = default; // default constructor required for serialization

public:
    SketchPointList::iterator first;
    SketchPointList::iterator second;

    inline TwoPointConstraint(SketchPointList::iterator& a, SketchPointList::iterator& b) : first(a),  second(b) { }

    bool IsAssociatedTo(const SketchPointList::iterator& point) const;
    bool IsAssociatedTo(const SketchLineList::iterator& line) const;
    bool IsAssociatedTo(const SketchCircleList::iterator& circle) const;

    //DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(TwoPointConstraint)

// OneLineConstraint
class OneLineConstraint : public SketchConstraint
{
protected:
    OneLineConstraint() = default; // default constructor required for serialization

public:
    SketchLineList::iterator line;

    inline OneLineConstraint(SketchLineList::iterator& a) : line(a) { }

    bool IsAssociatedTo(const SketchPointList::iterator& point) const;
    bool IsAssociatedTo(const SketchLineList::iterator& line) const;
    bool IsAssociatedTo(const SketchCircleList::iterator& circle) const;

    //DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(OneLineConstraint)

// TwoLineConstraint
class TwoLineConstraint : public SketchConstraint
{
protected:
    TwoLineConstraint() = default;  // default constructor required for serialization

public:
    SketchLineList::iterator first;
    SketchLineList::iterator second;

    inline TwoLineConstraint(SketchLineList::iterator& a, SketchLineList::iterator& b) : first(a), second(b) { }

    bool IsAssociatedTo(const SketchPointList::iterator& point) const;
    bool IsAssociatedTo(const SketchLineList::iterator& line) const;
    bool IsAssociatedTo(const SketchCircleList::iterator& circle) const;

    //DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(TwoLineConstraint)

// OneCircleConstraint
class OneCircleConstraint : public SketchConstraint
{
protected:
    OneCircleConstraint() = default;  // default constructor required for serialization

public:
    SketchCircleList::iterator circle;

    inline OneCircleConstraint(SketchCircleList::iterator& c) : circle(c) { }

    bool IsAssociatedTo(const SketchPointList::iterator& point) const;
    bool IsAssociatedTo(const SketchLineList::iterator& line) const;
    bool IsAssociatedTo(const SketchCircleList::iterator& c) const;

    //DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(OneCircleConstraint)

// TwoCircleConstraint
class TwoCircleConstraint : public SketchConstraint
{
protected:
    TwoCircleConstraint() = default;  // default constructor required for serialization

public:
    SketchCircleList::iterator first;
    SketchCircleList::iterator second;

    inline TwoCircleConstraint(SketchCircleList::iterator& a, SketchCircleList::iterator& b) : first(a), second(b) { }

    bool IsAssociatedTo(const SketchPointList::iterator& point) const;
    bool IsAssociatedTo(const SketchLineList::iterator& line) const;
    bool IsAssociatedTo(const SketchCircleList::iterator& c) const;

    //DECLARE_POLYMORPHIC_SERIALIZATION_MEMBERS()
};

DECLARE_SERIALIZATION_SCHEME(TwoCircleConstraint)

#endif
