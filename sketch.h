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

#ifndef _SKETCH_H_
#define _SKETCH_H_

#include "SketchAnnotation.h"
#include "SketchConstraints.h"
#include "SketchFeatures.h"

// CONSTRAINTS
// ============================================================================

// SKETCH
// ============================================================================
class Sketch
{
public:
    // TYPES
    // Uniformly choose types across units using Sketch
    // These definitions below will be *imported* mostly everywhere
    // ============================================================================
    using Coord = SketchFeature::Coord;
    using Vector2 = SketchFeature::Vector2;
    // ============================================================================

    // FEATURES
    // ============================================================================
    SketchPointList points;
    SketchLineList lines;
    SketchCircleList circles;
    SketchConstraintList constraints;
    SketchAnnotationList annotations;

    // METHODS
    // ============================================================================

    // uses Newton-Raphson to solve sketch enforcing all present constraints
    // additional ad-hoc equations may be provided (note those pointers will invariably be deleted when function returns)
    bool Solve(const std::vector<ConstraintEquation::Param*>& constantParameters = {});

    // adds constraint and attempts solving the sketch
    // if succeeded, returns iterator to the added constraint
    // if failed, sketch is rolled back, constraint is removed, pointer is deleted, returns iterator to constraints.end()
    SketchConstraintList::iterator TryAddConstraint(std::unique_ptr<SketchConstraint> newConstraint, bool msgBox = true);

    // adds multiple constraints and attempts solving the sketch
    // if successful, returns iterators to all the just-added items
    // if failed, sketch is rolled back, empty vector is returned
    std::vector<SketchConstraintList::iterator> TryAddConstraints(std::unique_ptr<SketchConstraint>(&&newConstraints)[], size_t count, bool msgBox = true);
};

DECLARE_SERIALIZATION_SCHEME(Sketch)

#endif // _SKETCH_H_
