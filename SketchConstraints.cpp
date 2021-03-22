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

#include "SketchConstraints.h"
#include <glm/gtx/norm.hpp>
#include <iostream>
#include <algorithm>
#include <glm/gtx/vector_angle.hpp>

/// GENERIC CONSTRAINT
/// ========================================================================================
bool SketchConstraint::IsAssociatedTo(const SketchPointList::iterator& point) const      { return false; }
bool SketchConstraint::IsAssociatedTo(const SketchLineList::iterator& line) const        { return false; }
bool SketchConstraint::IsAssociatedTo(const SketchCircleList::iterator& circle) const    { return false; }
ConstraintEquation* SketchConstraint::GetEquation() { return NULL; }

BEGIN_SERIALIZATION_SCHEME(SketchConstraint)
    SERIALIZATION_INHERIT(SketchAnnotation)
END_SERIALIZATION_SCHEME()

IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(SketchConstraint)

/// CONSTRAINT LIST
/// =========================================================================================
SketchConstraintList::iterator SketchConstraintList::add(SketchConstraint* sc)
{
    emplace_back( std::unique_ptr<SketchConstraint>(sc) );
    return std::prev(end());
}

SketchConstraintList::iterator SketchConstraintList::FindAssociated(const SketchPointList::iterator& point, const SketchConstraintList::iterator& first)
{
    return std::find_if(first, end(), [&point](const std::unique_ptr<SketchConstraint>& constraint) -> bool {
        return constraint->IsAssociatedTo(point);
    });
}

SketchConstraintList::iterator SketchConstraintList::FindAssociated(const SketchLineList::iterator& line, const SketchConstraintList::iterator& first)
{
    return std::find_if(first, end(), [&line](const std::unique_ptr<SketchConstraint>& constraint) -> bool {
        return constraint->IsAssociatedTo(line);
    });
}

SketchConstraintList::iterator SketchConstraintList::FindAssociated(const SketchCircleList::iterator& circle, const SketchConstraintList::iterator& first)
{
    return std::find_if(first, end(), [&circle](const std::unique_ptr<SketchConstraint>& constraint) -> bool {
        return constraint->IsAssociatedTo(circle);
    });
}

void SketchConstraintList::erase_associated(const SketchPointList::iterator& point)
{
    erase(std::remove_if(begin(), end(), [&point](const std::unique_ptr<SketchConstraint>& constraint) -> bool {
        return constraint->IsAssociatedTo(point);
    }), end());
}

void SketchConstraintList::erase_associated(const SketchLineList::iterator& line)
{
    erase(std::remove_if(begin(), end(),  [&line](const std::unique_ptr<SketchConstraint>& constraint) -> bool {
        return constraint->IsAssociatedTo(line);
    }), end());
}

void SketchConstraintList::erase_associated(const SketchCircleList::iterator& circle)
{
    erase(std::remove_if(begin(), end(), [&circle](const std::unique_ptr<SketchConstraint>& constraint) -> bool {
        return constraint->IsAssociatedTo(circle);
    }), end());
}

/// COMMON BASES FOR CONSTRAINTS
/// ============================================================================

// POINT BASED CONSTRAINTs
// =================================================================================================

// TwoPointConstraint
bool TwoPointConstraint::IsAssociatedTo(const SketchPointList::iterator& point) const
{
    return (first == point) || (second == point);
}

bool TwoPointConstraint::IsAssociatedTo(const SketchLineList::iterator& line) const
{
    return line->IsEndpoint(first) || line->IsEndpoint(second);
}

bool TwoPointConstraint::IsAssociatedTo(const SketchCircleList::iterator& circle) const
{
    return circle->IsCenterOrRadius(first) || circle->IsCenterOrRadius(second);
}

BEGIN_SERIALIZATION_SCHEME(TwoPointConstraint)
    SERIALIZATION_FIELD(first)
    SERIALIZATION_FIELD(second)
    SERIALIZATION_INHERIT(SketchConstraint)
END_SERIALIZATION_SCHEME()
//IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(TwoPointConstraint)

// LINE BASED CONSTRAINTS
// =================================================================================================

// OneLineConstraint
bool OneLineConstraint::IsAssociatedTo(const SketchPointList::iterator& point) const
{
    return line->IsEndpoint(point);
}

bool OneLineConstraint::IsAssociatedTo(const SketchLineList::iterator& l) const
{
    return (line == l) || line->IsEndpoint(l->first) || line->IsEndpoint(l->second);
}

bool OneLineConstraint::IsAssociatedTo(const SketchCircleList::iterator& circle) const
{
    return line->IsEndpoint(circle->center) || line->IsEndpoint(circle->radius);
}

BEGIN_SERIALIZATION_SCHEME(OneLineConstraint)
    SERIALIZATION_FIELD(line)
    SERIALIZATION_INHERIT(SketchConstraint)
END_SERIALIZATION_SCHEME()
//IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(OneLineConstraint)

// TwoLineConstraint
bool TwoLineConstraint::IsAssociatedTo(const SketchPointList::iterator& p) const
{
    return first->IsEndpoint(p) || second->IsEndpoint(p);
}

bool TwoLineConstraint::IsAssociatedTo(const SketchLineList::iterator& l) const
{
     return (first == l) || (second == l) || IsAssociatedTo(l->first) || IsAssociatedTo(l->first);
}

bool TwoLineConstraint::IsAssociatedTo(const SketchCircleList::iterator& c) const
{
    return IsAssociatedTo(c->center) || IsAssociatedTo(c->radius);
}

BEGIN_SERIALIZATION_SCHEME(TwoLineConstraint)
    SERIALIZATION_FIELD(first)
    SERIALIZATION_FIELD(second)
    SERIALIZATION_INHERIT(SketchConstraint)
END_SERIALIZATION_SCHEME()
//IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(TwoLineConstraint)

// CIRCLE BASED CONSTRAINTS
// =================================================================================================

// OneCircleConstraint
bool OneCircleConstraint::IsAssociatedTo(const SketchPointList::iterator& p) const
{
    return circle->IsCenterOrRadius(p);
}

bool OneCircleConstraint::IsAssociatedTo(const SketchLineList::iterator& l) const
{
    return circle->IsCenterOrRadius(l->first) || circle->IsCenterOrRadius(l->second);
}

bool OneCircleConstraint::IsAssociatedTo(const SketchCircleList::iterator& c) const
{
    return (circle == c) || IsAssociatedTo(c->center) || IsAssociatedTo(c->radius);
}

BEGIN_SERIALIZATION_SCHEME(OneCircleConstraint)
    SERIALIZATION_FIELD(circle)
    SERIALIZATION_INHERIT(SketchConstraint)
END_SERIALIZATION_SCHEME()
//IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(OneCircleConstraint)

// TwoCircleConstraint
bool TwoCircleConstraint::IsAssociatedTo(const SketchPointList::iterator& p) const
{
    return first->IsCenterOrRadius(p) || second->IsCenterOrRadius(p);
}

bool TwoCircleConstraint::IsAssociatedTo(const SketchLineList::iterator& l) const
{
    return IsAssociatedTo(l->first) || IsAssociatedTo(l->second);
}

bool TwoCircleConstraint::IsAssociatedTo(const SketchCircleList::iterator& c) const
{
    return (first == c) || (second == c) || IsAssociatedTo(c->center) || IsAssociatedTo(c->radius);
}

BEGIN_SERIALIZATION_SCHEME(TwoCircleConstraint)
    SERIALIZATION_FIELD(first)
    SERIALIZATION_FIELD(second)
    SERIALIZATION_INHERIT(SketchConstraint)
END_SERIALIZATION_SCHEME()
//IMPLEMENT_POLYMORPHIC_SERIALIZATION_MEMBERS(TwoCircleConstraint)
