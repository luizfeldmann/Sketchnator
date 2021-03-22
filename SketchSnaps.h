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

#ifndef _SKETCH_SNAPS_H_
#define _SKETCH_SNAPS_H_

#include <functional>
#include "sketch.h"

/// SNAP FILTER
/// Allows ignoring certain features from snapping
/// ======================================================================
typedef std::function<bool(const SketchLine*)> LineIgnoreTestHandler;
typedef std::function<bool(const SketchCircle*)> CircleIgnoreTestHandler;

// IgnoreTest_Single
// A callable that matches a single feature that must be ignored
// =======================================================================
template<class T>
struct IgnoreTest_Single
{
    const T* const ignore;

    IgnoreTest_Single(const T* t) : ignore(t) { }

    bool operator()(const T* t) const
    {
        return (t == ignore);
    }
};

// IgnoreTest_Multiple
// IgnoreTest_Multiple_Dynamic
// Callables that matches multiple features
// =======================================================================
template<class T, size_t N>
struct IgnoreTest_Multiple : public std::array<const T*, N>
{
    IgnoreTest_Multiple(std::array<const T*, N> arr) : std::array<const T*, N>(arr) {}

    bool operator()(const T* t) const
    {
        auto be = std::array<const T*, N>::cbegin();
        auto en = std::array<const T*, N>::cend();
        return (std::find(be, en, t) != en);
    }
};

template<class T>
struct IgnoreTest_Multiple_Dynamic : public std::list<const T*>
{
    IgnoreTest_Multiple_Dynamic() : std::list<const T*>() {}

    bool operator()(const T* t) const
    {
        auto be = std::list<const T*>::cbegin();
        auto en = std::list<const T*>::cend();
        return (std::find(be, en, t) != en);
    }
};

/// SNAPS
/// Tests if mouse coordinates may snap to certain features for precise input
/// ======================================================================
class SketchSnaps
{
public:
    using Coord = SketchFeature::Coord;
    using Vector2 = SketchFeature::Vector2;

    static Coord ProximityThreshold;

    /// LINE SNAPS
    /// ==============================================================

    // Line snaps that *don't* take a partially defined line as input
    // ==================================================================
    static bool ToLineEndpoint(const Sketch& sketch, LineIgnoreTestHandler ignore, const Vector2& mouse, SketchPointList::const_iterator& out_snap);

    static bool ToLine(const Sketch& sketch, LineIgnoreTestHandler ignore, const Vector2& mouse, Vector2& out_snap);
    static bool ToLine(const Sketch& sketch, LineIgnoreTestHandler ignore, const Vector2& mouse, SketchLineList::const_iterator& out_line, Vector2& out_snap);

    static bool ToLineMidpoint(const Sketch& sketch, LineIgnoreTestHandler ignore, const Vector2& mouse, Vector2& out_snap);
    static bool ToLineMidpoint(const Sketch& sketch, LineIgnoreTestHandler ignore, const Vector2& mouse, SketchLineList::const_iterator& out_line, Vector2& out_snap);

    static bool LineHorizontal(const SketchLine* line, const Vector2& mouse, Vector2& out_snap);
    static bool LineVertical(const SketchLine* line, const Vector2& mouse, Vector2& out_snap);

    // Line snaps that *do* take a partially defined line as input
    // ==================================================================
    static bool LineOrthogonal(const Sketch& sketch, const SketchLine* currLine, const Vector2& mouse, Vector2& out_snap);
    static bool LineOrthogonal(const Sketch& sketch, const SketchLine* currLine, const Vector2& mouse, SketchLineList::const_iterator& out_line, Vector2& out_snap);

    static bool LineTangentToCircle(const Sketch& sketch, const SketchLine* currLine, const Vector2& mouse, Vector2& out_snap);
    static bool LineTangentToCircle(const Sketch& sketch, const SketchLine* currLine, const Vector2& mouse, SketchCircleList::const_iterator& out_circle, Vector2& out_snap);

    /// CIRCLE SNAPS
    /// ==============================================================

    // Circle snaps that *don't* take a partially defined circle as input
    // ==================================================================
    static bool ToArcEndpoints(const Sketch& sketch, CircleIgnoreTestHandler ignore, const Vector2& mouse, Vector2& out_snap);
    static bool ToArcCenter(const Sketch& sketch, CircleIgnoreTestHandler ignore, const Vector2& mouse, SketchPointList::const_iterator& out_snap);
    static bool ToAnyCircleQuadrants(const Sketch& sketch, CircleIgnoreTestHandler ignore, const Vector2& mouse, Vector2& out_snap);

    static bool ToArc(const Sketch& sketch, CircleIgnoreTestHandler ignore, const Vector2& mouse, Vector2& out_snap);
    static bool ToArc(const Sketch& sketch, CircleIgnoreTestHandler ignore, const Vector2& mouse, SketchCircleList::const_iterator& out_circle, Vector2& out_snap);

    // Circle snaps that *do* take a partially defined circle as input
    // ==================================================================
    static bool ToCircleOwnQuadrants(const Sketch& sketch, const SketchCircle* cir, const Vector2& mouse, Vector2& out_snap);
    static bool ToArcOwnAxis(const Sketch& sketch, const SketchCircle* cir, const Vector2& mouse, Vector2& out_snap);
    static bool ToCirclesIntersection(const Sketch& sketch, const SketchCircle* self, const Vector2& mouse, Vector2& out_snap);
};

/// BOUNDING BOX TEST (FOR SELECTION)
/// ==========================================================================================================================================================================
class SketchFeatureBoundingRegion
{
public:
    using Coord = SketchSnaps::Coord;
    using Vector2 = SketchSnaps::Vector2;

    static bool Hit(const SketchPoint& sketchpoint, const Vector2& point, Coord& out_distance, Vector2& out_closest);
    static bool Hit(const SketchLine& line, const Vector2& point, Coord& out_distance, Vector2& out_closest);
    static bool Hit(const SketchCircle& circle, const Vector2& point, Coord& out_distance, Vector2& out_closest);
};

/// RECTANGLE TEST (DRAGGING SELECTION)
/// ===========================================================================================================================================================================
enum RectangleDragTest
{
    DRAG_FULLY_OUT      = 0,
    DRAG_PARTIALLY_IN   = 1,
    DRAG_FULLY_IN       = 2,
};

class SketchFeatureBoundingRectangle
{
public:
    using Coord = SketchSnaps::Coord;
    using Vector2 = SketchSnaps::Vector2;

    static RectangleDragTest Contains(const Vector2& p, const Vector2& r1, const Vector2& r2);
    static RectangleDragTest Contains(const SketchPoint& p, const Vector2& r1, const Vector2& r2);
    static RectangleDragTest Contains(const SketchLine& l, const Vector2& r1, const Vector2& r2);
    static RectangleDragTest Contains(const SketchCircle& l, const Vector2& r1, const Vector2& r2);
};

#endif // _SKETCH_SNAPS_H_
