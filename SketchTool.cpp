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

#include "SketchTool.h"
#include <iostream>
#include <array>
#include <functional>
#include <glm/gtx/closest_point.hpp>
#include <glm/gtx/norm.hpp>

#include "almost_equal.h"
#include "SketchSnaps.h"
#include "SnapBallon.h"
#include "SketchIntersection.h"

/// NOTICE:
/// SketchPoint and vec2 appear interchangeable but there is one important difference: SketchPoint means a point listed in the Sketch.points, while vec2 means an *abstract* point

/// DUMMY IMPLEMENTATION
void SketchTool::Begin()                        { }
void SketchTool::End()                          { }
bool SketchTool::LeftUp(const wxMouseEventEx& evt)      { return false; }
bool SketchTool::LeftDown(const wxMouseEventEx& evt)    { return false; }
bool SketchTool::LeftDouble(const wxMouseEventEx& evt)  { return false; }
bool SketchTool::MouseMove(const wxMouseEventEx& evt)   { return false; }
bool SketchTool::KeyUp(wxKeyEvent& evt)         { return false; }
bool SketchTool::KeyDown(wxKeyEvent& evt)       { return false; }

/// SELECTION
/// ========================================================
void SelectionSketchTool::Begin()
{
    // synchronize local list of selection based on sketch's objects bSelected state
    if (Filter & SSF_POINT)
    {
        for (SketchPoint& p : sketch.points)
        {
            if (p.bSelected)
                selection.push_back(&p);
        }
    }

    if (Filter & SSF_LINE)
        for (SketchLine& l : sketch.lines)
            if (l.bSelected)
                selection.push_back(&l);

    if (Filter & SSF_CIRCLE)
    {
        for (SketchCircle& c : sketch.circles)
        {
            if (c.bSelected)
                selection.push_back(&c);
        }
    }
}

void SelectionSketchTool::End()
{
    if (DisplayDragBox)
        DisplayDragBox(false, mouse, dragStart);
}

bool SelectionSketchTool::LeftUp(const wxMouseEventEx& evt)
{
    if (DisplayDragBox)
        DisplayDragBox(false, mouse, dragStart);

    return true;
}

void SelectionSketchTool::ClearSelection()
{
    for (SketchSelectable* sel : selection)
        sel->bSelected = false;

    selection.clear();
}

bool SelectionSketchTool::LeftDown(const wxMouseEventEx& evt)
{
    dragStart = mouse;

    // Remove old selection if:
    // 1 - single selection mode
    // 2 - control key not pressed
    if ( !evt.ControlDown() || !(Filter & SSF_MULTIPLE))
        ClearSelection();

    // add highlight to selection or remove from it (there cannot be copies)
    if (highlight)
    {
        bool bErased = false;
        for (auto it = selection.begin(); it != selection.end(); ++it)
        {
            if (*it == highlight)
            {
                selection.erase(it);
                bErased = true;
                break;
            }
        }

        if (!bErased)
            selection.push_back(highlight);
        else
            highlight->bSelected = false;

        highlight = NULL;
    }

    return true;
}

bool SelectionSketchTool::MoveNotDrag()
{
    // find closest "selectable" to mouse
    Coord minDist = std::numeric_limits<Coord>::max();
    SketchSelectable* toSelect = NULL;
    {
        // find closest point to mouse
        if (Filter & SSF_POINT)
        {
            for (SketchPoint& p : sketch.points)
            {
                Coord distance;
                Vector2 closest;
                if (!SketchFeatureBoundingRegion::Hit(p, mouse, distance, closest))
                    continue;

                if (distance < minDist)
                {
                    toSelect = &p;
                    minDist = distance;
                }
            }
        }

        // find closest line to mouse
        if (Filter & SSF_LINE)
        {
            for (SketchLine& l : sketch.lines)
            {
                Coord distance;
                Vector2 closest;
                if (!SketchFeatureBoundingRegion::Hit(l, mouse, distance, closest))
                    continue;

                if (distance < minDist)
                {
                    toSelect = &l;
                    minDist = distance;
                }
            }
        }

        // find closest circle to mouse
        if (Filter & SSF_CIRCLE)
        {
            for (SketchCircle& c : sketch.circles)
            {
                Coord distance;
                Vector2 closest;
                if (!SketchFeatureBoundingRegion::Hit(c, mouse, distance, closest))
                    continue;

                if (distance < minDist)
                {
                    toSelect = &c;
                    minDist = distance;
                }
            }
        }
    }

    if (highlight == toSelect)
        return false; // no changes

    // unselect previous highlight (unless it's already listed in permanent selection)
    if (highlight)
        if (!IsSelected(highlight))
            highlight->bSelected = false;

    if (toSelect)
        toSelect->bSelected = true;

    highlight = toSelect; // update

    return true;
}

bool SelectionSketchTool::MoveDrag()
{
    ClearSelection();

    // display the box
    if (DisplayDragBox)
        DisplayDragBox(true, mouse, dragStart);

    // dragging to the left  = select only fully enclosed objects
    // dragging to the right = select partially enclosed objects
    const RectangleDragTest requiredEnclosure = (mouse.x > dragStart.x) ? RectangleDragTest::DRAG_FULLY_IN : RectangleDragTest::DRAG_PARTIALLY_IN;

    for (SketchLine& line : sketch.lines)
    {
        if (!(SketchFeatureBoundingRectangle::Contains(line, dragStart, mouse) >= requiredEnclosure))
            continue;

        selection.push_back(&line);
        line.bSelected = true;
    }

    for (SketchCircle& circle : sketch.circles)
    {
        if (!(SketchFeatureBoundingRectangle::Contains(circle, dragStart, mouse) >= requiredEnclosure))
            continue;

        selection.push_back(&circle);
        circle.bSelected = true;
    }

    return true;
}

bool SelectionSketchTool::MouseMove(const wxMouseEventEx& evt)
{
    // get current mouse position
    {
        Coord x, y;
        evt.GetPosition(x, y);
        mouse = Vector2(x, y);
    }

    if (evt.Dragging() && (Filter & SSF_MULTIPLE) && !evt.ControlDown())
        return MoveDrag();
    else
        return MoveNotDrag();
}

static void PurgeUnusedPoints(Sketch& sketch)
{
    // @MODIFIED
    // loop in iterator because functions below take iterators
    for (SketchPointList::iterator p = sketch.points.begin(); p!= sketch.points.end(); ++p)
    {
        // do not delete point that are used by circles
        if (sketch.circles.IsPointUsed(p))
            continue;

        // do not delete line endpoints
        if (sketch.lines.IsPointUsed(p))
            continue;

        // do not delete points referenced by constraints
        if (sketch.constraints.FindAssociated(p, sketch.constraints.begin()) != sketch.constraints.end())
            continue;

        // point is deleted safely
        auto newp = std::next(p); // get next now, because p will be invalidated on erase
        sketch.points.erase(p);
        p = std::prev(newp); // go back so the for will increment it
    }
}

bool SelectionSketchTool::KeyDown(wxKeyEvent& evt)
{
    if (evt.GetKeyCode() != WXK_DELETE)
        return false;

    // process deletion of current selection
    if (selection.empty())
        return false;

    for (decltype(selection)::iterator sel = selection.begin(); sel != selection.end(); ++sel)
    {
        SketchPoint*  p;
        SketchLine*   l;
        SketchCircle* c;

        if ((p = dynamic_cast<SketchPoint*>(*sel)) != NULL)
        {
            // find iterator to point
            SketchPointList::iterator pit = std::find(sketch.points.begin(), sketch.points.end(), *p);
            if (pit == sketch.points.end())
                continue; // not found

            // find objects which share this point (must be deleted too...)
            // lines whose endpoint is P
            for(;;) {
                SketchLineList::iterator line_it = sketch.lines.FindByEndpoint(pit);
                if (line_it == sketch.lines.end())
                    break;

                selection.erase(std::remove(selection.begin(), selection.end(), (SketchLine*)&*line_it ), selection.end()); // in case this line was marked for deletion
                sketch.constraints.erase_associated(line_it); // don't leave a dangling constraint for this deleted line
                sketch.lines.erase(line_it);
            }

            // circles whose center or radius are P
            for(;;) {
                SketchCircleList::iterator circle_it = sketch.circles.FindByEndpoint(pit);
                if (circle_it == sketch.circles.end())
                    break;

                selection.erase(std::remove(selection.begin(), selection.end(), (SketchCircle*)&*circle_it ), selection.end()); // in case this line was marked for deletion
                sketch.constraints.erase_associated(circle_it); // don't leave a dangling constraint for this deleted circle
                sketch.circles.erase(circle_it);
            }

            // remove the point itself
            sketch.constraints.erase_associated(pit); // don't leave a dangling constraint for this deleted circle
            sketch.points.erase(pit);
        }
        else if ((l = dynamic_cast<SketchLine*>(*sel)) != NULL)
        {
            // find iterator to line
            SketchLineList::iterator lit = std::find(sketch.lines.begin(), sketch.lines.end(), *l);
            if (lit == sketch.lines.end())
                continue; // not found

            // erase the line
            sketch.constraints.erase_associated(lit);
            sketch.lines.erase(lit);
        }
        else if ((c = dynamic_cast<SketchCircle*>(*sel)) != NULL)
        {
            // find iterator to circle
            SketchCircleList::iterator cit = std::find(sketch.circles.begin(), sketch.circles.end(), *c);
            if (cit == sketch.circles.end())
                continue; // not found

            sketch.constraints.erase_associated(cit);
            sketch.circles.erase(cit);
        }

        // @MODIFIED
        sel++; // increment first, so we don't invalidate the pointer
        selection.erase(std::prev(sel)); // perform erase
        sel--; // let the for loop increment it
    }

    // delete dangling points (left orfaned when owner was deleted)
    PurgeUnusedPoints(sketch);
    selection.clear();

    return true;
}

/// MOVE OBJECTS
/// ========================================================
MoveSketchTool::MoveSketchTool(Sketch& s) :
    SketchTool(s)
{

}

bool MoveSketchTool::LeftUp(const wxMouseEventEx&)
{
    initialPositions.clear();
    filter.reset();
    return false;
}

// when moving objects, we must make sure they snap only to non-selected objects (no self snapping)
// in addition to storing the points we're moving, we need to store handlers to filter auto snap-able entities
struct MoveSketchTool::MoveSketchTool_Filter
{
    IgnoreTest_Multiple_Dynamic<SketchLine>   selectedLines;
    IgnoreTest_Multiple_Dynamic<SketchCircle> selectedCircles;

    MoveSketchTool_Filter() = default;
};

bool MoveSketchTool::LeftDown(const wxMouseEventEx&)
{
    // begin movement
    dragStart = mouse;
    filter.reset( new MoveSketchTool_Filter() );

    auto blacklist_point = [&](const SketchPointList::iterator& p)->void
    {
        // cannot let this point snap to the lines or circles that own it (would gridlock the drag motion)
        for (SketchLineList::iterator it = sketch.lines.FindByEndpoint(p, sketch.lines.begin()); it != sketch.lines.end(); it = sketch.lines.FindByEndpoint(p, std::next(it) ))
            filter->selectedLines.push_back( &*it );

        for (SketchCircleList::iterator it = sketch.circles.FindByEndpoint(p, sketch.circles.begin()); it != sketch.circles.end(); it = sketch.circles.FindByEndpoint(p, std::next(it) ))
            filter->selectedCircles.push_back( &*it );
    };

    // list initial position of all points
    // add all selected points
    for (SketchPointList::iterator p = sketch.points.begin(); p != sketch.points.end(); ++p)
    {
        if (p->bSelected)
        {
            initialPositions.emplace_back( std::pair<SketchPointList::iterator, Vector2>(p, *p) );
            blacklist_point(p);
        }
    }

    // add line and circle endpoints without repeating
    auto maybe_add = [&](SketchPointList::iterator& p) -> void
    {
        if (std::find_if(initialPositions.begin(), initialPositions.end(), [&p](const std::pair<SketchPointList::iterator, Vector2>& test)->bool {
            return (test.first == p);
        }) == initialPositions.end())
        {
            initialPositions.emplace_back( std::pair<SketchPointList::iterator, Vector2>(p, *p) );
        }
    };


    for (SketchLineList::iterator l = sketch.lines.begin(); l != sketch.lines.end(); ++l)
    {
        if (l->bSelected)
        {
            maybe_add(l->first);
            maybe_add(l->second);
            blacklist_point(l->first);
            blacklist_point(l->second);
            //filter->selectedLines.push_back( &*l ); - blacklisting the point will automatically blacklist this owner line
        }
    }

    for (SketchCircleList::iterator c = sketch.circles.begin(); c != sketch.circles.end(); ++c)
    {
        if (c->bSelected)
        {
            maybe_add(c->center);
            maybe_add(c->radius);
            blacklist_point(c->center);
            blacklist_point(c->radius);
            //filter->selectedCircles.push_back( &*c ); // blacklisting the center/radius will automatically blacklist the owner circle
        }
    }

    return false;
}

static bool MoveToolSnapTest(const Sketch& sketch, const SketchSnaps::Vector2& mouse, const MoveSketchTool::MoveSketchTool_Filter* filter, bool bDragging, SketchSnaps::Vector2& out_snap)
{
    // not dragging = snap anywhere
    // dragging = snap non-selected only
    LineIgnoreTestHandler   ignore_lines   = NULL;
    CircleIgnoreTestHandler ignore_circles = NULL;

    if (bDragging && filter)
    {
        ignore_lines = filter->selectedLines;
        ignore_circles = filter->selectedCircles;
    }

    // save reference to snapped features to (possibly) place constraints
    SketchPointList::const_iterator snap_iter_point;

    if (SketchSnaps::ToLineEndpoint(sketch, ignore_lines, mouse, snap_iter_point))
    {
        out_snap = *snap_iter_point;
        SnapBallon::Show(SNAP_BALLOON_LINE_ENDPOINT);
    }

    else if (SketchSnaps::ToLineMidpoint(sketch, ignore_lines, mouse, out_snap))
        SnapBallon::Show(SNAP_BALLOON_LINE_MIDPOINT);

    else if (SketchSnaps::ToLine(sketch, ignore_lines, mouse, out_snap))
        SnapBallon::Show(SNAP_BALLOON_ON_LINE);

    else if (SketchSnaps::ToArcCenter(sketch, ignore_circles, mouse, snap_iter_point))
    {
        out_snap = *snap_iter_point;
        SnapBallon::Show(SNAP_BALLOON_CIRCLE_CENTER);
    }

    else if (SketchSnaps::ToArcEndpoints(sketch, ignore_circles, mouse, out_snap))
        SnapBallon::Show(SNAP_BALLOON_ARC_ENDPOINT);

    else if (SketchSnaps::ToAnyCircleQuadrants(sketch, ignore_circles, mouse, out_snap))
        SnapBallon::Show(SNAP_BALLOON_CIRCLE_QUADRANT);

    else if (SketchSnaps::ToArc(sketch, ignore_circles, mouse, out_snap))
        SnapBallon::Show(SNAP_BALLOON_ON_ARC);

    else
    {
        SnapBallon::Show(SNAP_BALLOON_NONE);
        return false;
    }

    return true;
}

bool MoveSketchTool::MouseMove(const wxMouseEventEx& evt)
{
    // get current mouse position
    {
        Coord x, y;
        evt.GetPosition(x, y);
        mouse = Vector2(x, y);
    }

    // attempt snaps
    Vector2 snap_point;
    if (MoveToolSnapTest(sketch, mouse, filter.get(), evt.Dragging(), snap_point))
        mouse = snap_point;

    // check we're actually dragging something
    if (!evt.Dragging())
        return false;

    // using the original position (saved when left button went down), calculate mouse offset
    const Vector2 offset = mouse - dragStart;

    std::vector<ConstraintEquation::Param*> constantParameters;
    constantParameters.reserve(initialPositions.size() * 2);

    for (std::pair<SketchPointList::iterator, Vector2>& p : initialPositions)
    {
        p.first->x = p.second.x + offset.x;
        p.first->y = p.second.y + offset.y;

        // make sure final position is set precisely by the solver (moved points are constant - ignored by solver)
        constantParameters.push_back(&p.first->x);
        constantParameters.push_back(&p.first->y);
    }

    if (!sketch.Solve(constantParameters))
    {
        // if moving failed, restore original state (before moving)
        // so the sketch is never left in an unsolvable state
        for (std::pair<SketchPointList::iterator, Vector2>& p : initialPositions)
        {
            p.first->x = p.second.x;
            p.first->y = p.second.y;
        }
    }

    return true;
}

/// SINGLE SELECTION TOOL
/// ========================================================
void SingleSelectionEditTool::Begin()
{
    // clear selection
    highlight = NULL;

    for (auto& p : sketch.points)
        p.bSelected = false;

    for (auto& l : sketch.lines)
        l.bSelected = false;

    for (auto& c : sketch.circles)
        c.bSelected = false;
}

void SingleSelectionEditTool::End()
{
    if (highlight)
        highlight->bSelected = false;
}

SketchSelectable* SingleSelectionEditTool::GetHover(const wxMouseEventEx& evt)
{
    // get mouse position
    Coord x, y;
    evt.GetPosition(x, y);
    mouse = Vector2(x, y);

    // find closest entity to mouse
    Coord minDist = std::numeric_limits<Coord>::max();
    SketchSelectable* toSelect = NULL;

    if (Filter & SketchSelectionFilter::SSF_POINT)
    {
        for (auto pit = sketch.points.begin(); pit != sketch.points.end(); ++pit)
        {
            SketchPoint& p = *pit;

            Coord distance;
            Vector2 closest;

            if (!SketchFeatureBoundingRegion::Hit(p, mouse, distance, closest))
                continue;

            if (distance > minDist)
                continue;

            toSelect = &p;
            minDist = distance;
            currentPoint = pit;
        }
    }

    // points have selection preference when applicable
    // if a point was found, avoid other selections
    if (!toSelect)
    {
        // find closest line to mouse
        if (Filter & SketchSelectionFilter::SSF_LINE)
        {
            for (auto lit = sketch.lines.begin(); lit != sketch.lines.end(); ++lit)
            {
                SketchLine& l = *lit;

                Coord distance;
                Vector2 closest;

                if (!SketchFeatureBoundingRegion::Hit(l, mouse, distance, closest))
                    continue;

                if (distance > minDist)
                    continue;

                toSelect = &l;
                minDist = distance;
                currentLine = lit;
            }
        }

        // closest circle to mouse
        if (Filter & SketchSelectionFilter::SSF_CIRCLE)
        {
            for (auto cit = sketch.circles.begin(); cit != sketch.circles.end(); ++cit)
            {
                SketchCircle& c = *cit;

                Coord distance;
                Vector2 closest;
                if (!SketchFeatureBoundingRegion::Hit(c, mouse, distance, closest))
                    continue;

                if (distance > minDist)
                    continue;

                toSelect = &c;
                minDist = distance;
                currentCircle = cit;
            }
        }
    }

    return toSelect;
}

bool SingleSelectionEditTool::UpdateHighlight(SketchSelectable* const toSelect)
{
    if (toSelect == highlight)
        return false; // no changes

    // unselect old
    if (highlight)
        highlight->bSelected = false;

    // select new
    if (toSelect)
        toSelect->bSelected = true;

    // update
    highlight = toSelect;

    return true; // highlighted item changed
}

bool SingleSelectionEditTool::MouseMove(const wxMouseEventEx& evt)
{
    // this is divided into 2 steps because derived class may want to perform changes
    // for example, filter out a given highlight, or include it's own suggestions
    return UpdateHighlight(GetHover(evt));
}

/// ConsecutiveSelectionSketchTool
/// Selects two objects of mixed type in a row
/// =========================================================
bool AbstractConsecutiveSelectionSketchTool::LeftDown(const wxMouseEventEx& evt)
{
    bool bResult = false;
    if (dynamic_cast<SketchPoint*>(highlight))
    {
        if (dynamic_cast<SketchPoint*>(previous))
            bResult = this->OnPointPoint(currentPoint, previousPoint);

        else if (dynamic_cast<SketchLine*>(previous))
            bResult = this->OnPointLine(currentPoint, previousLine);

        else if (dynamic_cast<SketchCircle*>(previous))
            bResult = this->OnPointCircle(currentPoint, previousCircle);

        if (!bResult)
            previousPoint = currentPoint;
    }
    else if (dynamic_cast<SketchLine*>(highlight))
    {
        if (dynamic_cast<SketchPoint*>(previous))
            bResult = this->OnPointLine(previousPoint, currentLine);

        else if (dynamic_cast<SketchLine*>(previous))
            bResult = this->OnLineLine(currentLine, previousLine);

        else if (dynamic_cast<SketchCircle*>(previous))
            bResult = this->OnLineCircle(currentLine, previousCircle);

        if (!bResult)
            previousLine = currentLine;
    }
    else if (dynamic_cast<SketchCircle*>(highlight))
    {
        if (dynamic_cast<SketchPoint*>(previous))
            bResult = this->OnPointCircle(previousPoint, currentCircle);

        else if (dynamic_cast<SketchLine*>(previous))
            bResult = this->OnLineCircle(previousLine, currentCircle);

        else if (dynamic_cast<SketchCircle*>(previous))
            bResult = this->OnCircleCircle(currentCircle, previousCircle);

        if (!bResult)
            previousCircle = currentCircle;
    }

    if (bResult)
    {
        // operation complete - erase history (start fresh, don't chain operations)
        previous = NULL;
        return true;
    }
    else
    {
        // operation pending - save for later
        previous = highlight;
        return false;
    }
}

bool ConsecutiveSelectionSketchTool::OnPointPoint(SketchPointList::iterator&, SketchPointList::iterator&) {return false;}
bool ConsecutiveSelectionSketchTool::OnPointLine(SketchPointList::iterator&, SketchLineList::iterator&) {return false;}
bool ConsecutiveSelectionSketchTool::OnPointCircle(SketchPointList::iterator&, SketchCircleList::iterator&) {return false;}
bool ConsecutiveSelectionSketchTool::OnLineLine(SketchLineList::iterator&, SketchLineList::iterator&) {return false;}
bool ConsecutiveSelectionSketchTool::OnLineCircle(SketchLineList::iterator&, SketchCircleList::iterator&) {return false;}
bool ConsecutiveSelectionSketchTool::OnCircleCircle(SketchCircleList::iterator&, SketchCircleList::iterator&) {return false;}

/// SPLIT/TRIM LINES
/// ========================================================
static std::list<SketchIntersection::Vector2> ToolFindIntersections(const Sketch& sketch, const SketchSelectable* highlight)
{
    using Vector2 = SketchIntersection::Vector2;

    // sanity check
    std::list<Vector2> intersection_points;
    if (!highlight)
        return intersection_points;

    // what type of object is selected?
    const SketchLine* highlight_line = dynamic_cast<const SketchLine*>(highlight);
    const SketchCircle* highlight_circle = dynamic_cast<const SketchCircle*>(highlight);

    // search for intersection points
    auto line_line = [&intersection_points](const SketchLine& line1, const SketchLine& line2)->void
    {
        Vector2 p;
        if (!SketchIntersection::LineLine(*line1.first, *line1.second, *line2.first, *line2.second, p))
            return;

        intersection_points.push_back( p );
    };

    auto line_circle = [&intersection_points](const SketchLine& line, const SketchCircle& circle)->void
    {
        const Vector2& C = *circle.center;
        // intersectLineCircle treats circle as origin centered - must offset the line
        const Vector2 A = *line.first - C;
        const Vector2 B = *line.second - C;

        unsigned char numIntersections;
        Vector2 out_points[2];

        if (!SketchIntersection::LineCircle(circle.GetRadius(), A, B, numIntersections, out_points))
            return;

        for (unsigned char j = 0; j < numIntersections; j++)
        {
            const Vector2& p = out_points[j] + C; // the origin of out_points[j] is the center of the circle

            // test if P is within the arc's bounds
            if (circle.GetPointAngle(p) > circle.GetAngleRad())
                continue;

            intersection_points.push_back(p);
        }
    };

    auto circle_circle = [&intersection_points](const SketchCircle& circle1, const SketchCircle& circle2)->void
    {
        Vector2 out_points[2];

        if (!SketchIntersection::CircleCircle(*circle1.center, circle1.GetRadius(), *circle2.center, circle2.GetRadius(), out_points))
            return;

        for (const Vector2& p : out_points)
        {
            if (circle1.GetPointAngle(p) >= circle1.GetAngleRad())
                continue;

            if (circle2.GetPointAngle(p) >= circle2.GetAngleRad())
                continue;

            intersection_points.push_back( p );
        }
    };

    // look for lines intersecting this
    for (const SketchLine& testLine : sketch.lines)
    {
        if (&testLine == highlight)
            continue;

        if (highlight_line)
            line_line(testLine, *highlight_line);     // LINE TO LINE INTERSECTION
        else if (highlight_circle)
            line_circle(testLine, *highlight_circle); // LINE TO CIRCLE INTERSECTION
    }

    // look for circles intersecting this
    for (const SketchCircle& testCircle : sketch.circles)
    {
        if (&testCircle == highlight)
            continue;

        if (highlight_line)
            line_circle(*highlight_line, testCircle); // LINE TO CIRCLE INTERSECTION
        else if (highlight_circle)
            circle_circle(*highlight_circle, testCircle); // CIRCLE TO CIRCLE INTERSECTION
    }

    return intersection_points;
}

struct EraseEndpointsFromIntersectionList
{
using Vector2 = SketchPoint::Vector2;

private:
    bool operator() (const Vector2& p, const SketchLine* highlight_line) const {
        return (almost_equal(p, *highlight_line->first) || almost_equal(p, *highlight_line->second));
    }

    bool operator() (const Vector2& p, const SketchCircle* highlight_circle) const {

        return (almost_equal(p, *highlight_circle->radius) || almost_equal(p, highlight_circle->GetArcEndpoint()));
    }

    const SketchLine* const line;
    const SketchCircle* const circle;

public:
    EraseEndpointsFromIntersectionList(const SketchLine* l, const SketchCircle* c) :
         line(l), circle(c)
    {

    }

    bool operator() (const Vector2& p) const
    {
        if (line)
            return operator()(p, line);
        else if (circle)
            return operator()(p, circle);

        throw;
        return false;
    }
};

bool SplitSketchTool::LeftDown(const wxMouseEventEx& evt)
{
    if (!highlight)
        return false;

    // what type of object is this?
    SketchLine* highlight_line = dynamic_cast<SketchLine*>(highlight);
    SketchCircle* highlight_circle = dynamic_cast<SketchCircle*>(highlight);

    std::list<Vector2> intersection_points = ToolFindIntersections(sketch, highlight);
    intersection_points.erase(std::remove_if(intersection_points.begin(), intersection_points.end(), EraseEndpointsFromIntersectionList(highlight_line, highlight_circle)), intersection_points.end());

    // if no intersections, do nothing
    if (intersection_points.empty())
        return false;

    // pick closest intersection to mouse
    Coord minDist = std::numeric_limits<Coord>::max();
    Vector2 intersection;

    for (const Vector2& p : intersection_points)
    {
        Coord dist;
        if ((dist = glm::distance(p, mouse)) < minDist)
        {
            minDist = dist;
            intersection = p;
        }
    }

    if (highlight_line != NULL)
    {
        // the size of the line will change so we must unconstraint it to allow it
        sketch.constraints.erase_associated(currentLine);

        // split line
        SketchPointList::iterator a = highlight_line->first;
        SketchPointList::iterator b = highlight_line->second;

        SketchPointList::iterator p = sketch.points.add(intersection);
        highlight_line->first = a;
        highlight_line->second = p;


        sketch.lines.add(p, b)->type = highlight_line->type; // make sure the line we created has the same construction/centerline attributes
    }
    else if (highlight_circle != NULL)
    {
        sketch.constraints.erase_associated(currentCircle);

        // split circle
        const Coord angle1 = highlight_circle->GetPointAngle(intersection);
        const Coord angle2 = highlight_circle->GetAngleRad() - angle1;

        // shorten original circle
        highlight_circle->SetAngleRad(angle1);

        // add new circle
        sketch.circles.add(highlight_circle->center, sketch.points.add(intersection), angle2)->type = highlight_circle->type; // make sure the circle we created has the same construction/centerline attributes
    }
    else
        return false;

    MouseMove(evt);
    return true;
}

bool TrimSketchTool::LeftDown(const wxMouseEventEx& evt)
{
    if (!highlight)
        return false;

    // what type of object is this?
    SketchLine* highlight_line = dynamic_cast<SketchLine*>(highlight);
    SketchCircle* highlight_circle = dynamic_cast<SketchCircle*>(highlight);

    // find intersections (except for endpoints, which must be ignored)
    std::list<Vector2> intersection_points = ToolFindIntersections(sketch, highlight);
    intersection_points.erase(std::remove_if(intersection_points.begin(), intersection_points.end(), EraseEndpointsFromIntersectionList(highlight_line, highlight_circle)), intersection_points.end());

    // if no intersections, just delete this
    if (intersection_points.empty())
    {
        if (highlight_line)
        {
            sketch.constraints.erase_associated(currentLine);
            sketch.lines.erase(currentLine);
        }
        else
        {
            sketch.constraints.erase_associated(currentCircle);
            sketch.circles.erase(currentCircle);
        }

        PurgeUnusedPoints(sketch);
        highlight = NULL;

        return true;
    }


    if (highlight_line != NULL)
    {
        // pick closest (linear distance) intersection to mouse
        Coord minDist = std::numeric_limits<Coord>::max();
        Vector2 intersection;

        for (const Vector2& p : intersection_points)
        {
            Coord dist;
            if ((dist = glm::distance(p, mouse)) < minDist)
            {
                minDist = dist;
                intersection = p;
            }
        }

        // find the endpoint opposite the intersection and the mouse
        SketchPointList::iterator endpoint_mouse = (glm::dot(mouse - intersection, mouse - *highlight_line->first) < 0) ? highlight_line->first : highlight_line->second;
        SketchPointList::iterator endpoint_other = (endpoint_mouse == highlight_line->first) ? highlight_line->second : highlight_line->first;

        // is there another (or more) intersection(s) before the endpoint?
        minDist = std::numeric_limits<Coord>::max();
        const Vector2* otherIntersection = NULL;

        for (const Vector2& p : intersection_points)
        {
            if (almost_equal(p, intersection))
                continue; // don't match self

            if (glm::dot(p - intersection, p - *endpoint_mouse) >= 0)
                continue; // not between

            Coord dist;
            if ((dist = glm::distance(p, intersection)) < minDist)
            {
                minDist = dist;
                otherIntersection = &p;
            }
        }

        // unconstrain this line (because it's dimension will change)
        sketch.constraints.erase_associated(currentLine);

        // double trim
        if (otherIntersection)
            sketch.lines.add(endpoint_mouse, sketch.points.add(*otherIntersection))->type = highlight_line->type;

        // simple trim
        highlight_line->first = endpoint_other;
        highlight_line->second = sketch.points.add(intersection);
    }
    else if (highlight_circle != NULL)
    {
        const Coord mouse_angle = highlight_circle->GetPointAngle(mouse);

        // pick closest (angular distance) intersection to mouse without crossing the arc gap
        Coord minDist = std::numeric_limits<Coord>::max();
        Vector2 intersection;

        for (const Vector2& p : intersection_points)
        {
            Coord dist;
            if ((dist = glm::abs(highlight_circle->GetPointAngle(p) - mouse_angle)) < minDist)
            {
                minDist = dist;
                intersection = p;
            }
        }

        const Coord inter_angle = highlight_circle->GetPointAngle(intersection);
        const Coord arcDefAngle = highlight_circle->GetAngleRad();

        // is there another (or more) intersection(s) before the endpoint?
        minDist = std::numeric_limits<Coord>::max();
        const Vector2* otherIntersection = NULL;

        for (const Vector2& p : intersection_points)
        {
            if (almost_equal(p, intersection))
                continue; // don't match self

            const Coord other_angle = highlight_circle->GetPointAngle(p);

            // mouse before intersection = other intersection must be yet before the mouse
            if ((mouse_angle < inter_angle) && (other_angle > mouse_angle))
                continue;

            // mouse after intersection = other intersection must be yet after mouse
            if ((mouse_angle > inter_angle) && (other_angle < mouse_angle))
                continue;

            Coord dist;
            if ((dist = glm::abs(other_angle - inter_angle)) < minDist)
            {
                minDist = dist;
                otherIntersection = &p;
            }
        }

        if (inter_angle <= 0)
            goto bail;

        // unconstrain this circle
        sketch.constraints.erase_associated(currentCircle);

        // SIMPLE TRIM
        if (mouse_angle < inter_angle)
        {
            // DOUBLE TRIM
            if (otherIntersection)
                sketch.circles.add(highlight_circle->center, highlight_circle->radius, highlight_circle->GetPointAngle(*otherIntersection))->type = highlight_circle->type;

            // start point of circle is changed
            highlight_circle->radius = sketch.points.add(intersection);
            highlight_circle->SetAngleRad( arcDefAngle - inter_angle );
        }
        else
        {
            if (otherIntersection)
                sketch.circles.add(highlight_circle->center, sketch.points.add(*otherIntersection), arcDefAngle - highlight_circle->GetPointAngle(*otherIntersection))->type = highlight_circle->type;;

            // start point of circle is preserved
            highlight_circle->SetAngleRad( inter_angle );
        }
    }
    else
        return false;

    bail:
    highlight->bSelected = false; // segment under mouse was deleted - meaning it can't be selected anymore
    highlight = NULL;

    PurgeUnusedPoints(sketch);
    return true;
}

/// EXTEND
/// ========================================================
void ExtendSketchTool::Invalidate()
{
    sketch.points.erase(far_endpoint);
    sketch.lines.erase(temporaryLine);
    bCurrent = false;
}

void ExtendSketchTool::End()
{
    if (bCurrent)
        Invalidate();

    SingleSelectionEditTool::End();
}

void ExtendSketchTool::NewProposal(SketchLineList::iterator line, SketchPointList::iterator endpoint)
{
    proposal_line = line;
    near_endpoint = endpoint;

    // attempt finding intersection with lines or circles
    bool bFound = false;
    Vector2 intersection;
    Coord minParam = std::numeric_limits<Coord>::max();

    const Vector2& a = (endpoint == line->first) ? *(line->second) : *(line->first);
    const Vector2& b = *endpoint;

    // find prolonged intersections to other lines
    {
        // cannot use previously implemented function because here were're testing for different bounds in parameter
        for (const SketchLine& hit : sketch.lines)
        {
            if (&hit == &*line)
                continue; // never match self

            const Vector2& c = *hit.first;
            const Vector2& d = *hit.second;

            const glm::tmat2x2<Coord> M(b-a,c-d);

            if ( almost_zero(glm::determinant(M)) )
                continue;

            const Vector2 ts = glm::inverse(M) * (c - a);
            const Coord& t = ts.x; // parameter for line AB; must be *after* the line (t > 1)
            const Coord& s = ts.y; // parameter for line DC; must be *over* the line (0 < s < 1)

            if ((t < 1.0f) || almost_zero(t - 1.0f) || (s < 0) || (s > 1))
                continue;

            // choose closest intersection (smallest t)
            if (t < minParam)
            {
                const Vector2 p = (b-a)*t + a; // get intersection point from parameter
                intersection = p;
                minParam = t;
                bFound = true;
            }
        }
    }

    // find prolonged intersections to circles
    {
        const Coord dx = b.x - a.x;         // difference is not changed by the offeset (-center) used below
        const Coord dy = b.y - a.y;
        const Coord baskA = dx*dx + dy*dy;

        for (const SketchCircle& hit : sketch.circles)
        {
            if (almost_zero(baskA))
                break; // many many divisions by zero...

            const Vector2& center = *hit.center;
            const Coord r = hit.GetRadius();

            // offset by the circle center to facilitate the equations
            const Vector2 A = a - center;
            //const Vector2 B = b - center;

            const Coord baskB = 2.0f*(A.x*dx + A.y*dy);
            const Coord baskC = A.x*A.x + A.y*A.y - r*r;

            const Coord discriminant = baskB*baskB - 4*baskA*baskC;

            if (discriminant < 0)
                continue; // no intersection

            const Coord sqrtdelta = glm::sqrt(discriminant);

            const Coord t[2] = { (-baskB + sqrtdelta)/(2.0f*baskA),
                                 (-baskB - sqrtdelta)/(2.0f*baskA) };

            for (unsigned char i = 0; i < 2; i++)
            {
                if (t[i] < 1.0f || almost_zero(t[i] - 1.0f)) // intersection must happen *after* the end of the line
                    continue;

                const Vector2 p = a + (b-a)*t[i]; // here we use original points a & b (instead of offset point A & B) to get point in original coordinates system (it works because length AB and ab is the same, so parameter is the same)

                // make sure point belongs to arc
                if (hit.GetPointAngle(p) > hit.GetAngleRad())
                    continue;

                // select closest point
                if (t[i] < minParam)
                {
                    intersection = p;
                    minParam = t[i];
                    bFound = true;
                }
            }
        }
    }

    if (!bFound)
        return; // could not find suitable extension

    far_endpoint = sketch.points.add(intersection);
    temporaryLine = sketch.lines.add(near_endpoint, far_endpoint);
    temporaryLine->type = SketchFeatureType::TEMPORARY;
    bCurrent = true;
}

bool ExtendSketchTool::MouseMove(const wxMouseEventEx& evt)
{
    SingleSelectionEditTool::MouseMove(evt);

    SketchLine* highlight_line = dynamic_cast<SketchLine*>(highlight);

    if (!highlight_line) // no line
    {
        if (bCurrent)    // previous proposal is no longer valid
            Invalidate();

        return true;
    }

    // there is a selected line...
    SketchPointList::iterator test_endpoint = (glm::distance2(*currentLine->first, mouse) < glm::distance2(*currentLine->second, mouse)) ? currentLine->first : currentLine->second;

    if (!bCurrent) // first time selecting it
    {
        NewProposal(currentLine, test_endpoint);
        return true;
    }

    // there is a currently selected line which may, or not, be the same as before
    if ((proposal_line == currentLine) && (test_endpoint == near_endpoint))
        return false; // proposal is still the same

    // there as a currently selected line and endpoint, there is a previously selected line and endpoint, and they are not the same
    Invalidate();
    NewProposal(currentLine, test_endpoint);
    return true;
}

bool ExtendSketchTool::LeftDown(const wxMouseEventEx& evt)
{
    if (!bCurrent)
        return false;

    // unconstrain the line to allow it's size to change
    sketch.constraints.erase_associated(proposal_line);

    // move line further, without editing existing old endpoint (may be shared by other features)
    if (proposal_line->first == near_endpoint)
        proposal_line->first  = far_endpoint;
    else
        proposal_line->second = far_endpoint;

    // delete temporary line
    sketch.lines.erase(temporaryLine);

    // delete near endpoint if no longer used
    if (!sketch.lines.IsPointUsed(near_endpoint) && !sketch.circles.IsPointUsed(near_endpoint))
        sketch.points.erase(near_endpoint); // IsPointUsed checked that no constraint has it

    // conclude operation
    bCurrent = false;
    return true;
}

/// GENERIC CREATION TOOL
/// ========================================================
enum SketchFeatureType CreationSketchTool::creationStyle = SketchFeatureType::NORMAL;

void CreationSketchTool::ApplyToCurrentSelection(Sketch& sketch)
{
    for (SketchCircle& c : sketch.circles)
        if (c.bSelected)
            c.type = creationStyle;

    for (SketchLine& l : sketch.lines)
        if (l.bSelected)
            l.type = creationStyle;
}

/// AUTOMATIC CONSTRAINT TOOL
/// ========================================================
bool AutomaticConstraintPlacement::bAutoConstrain = true;

void AutomaticConstraintPlacement::SetSuggested(std::list<SketchConstraint*>&& newConstraints)
{
    suggestedConstraints.clear(); // erase the previous saved constraint suggestions, which will destroy the unique_ptr's to avoid leak
    for (SketchConstraint* c : newConstraints)
        suggestedConstraints.emplace_back( std::unique_ptr<SketchConstraint>(c) );

    newConstraints.clear(); // clear the passed list to avoid having duplicates of the pointers, which will invariable get deleted eventually
}

void AutomaticConstraintPlacement::PlaceSuggestedConstraints(Sketch& sketch)
{
    for (std::unique_ptr<SketchConstraint>& c : suggestedConstraints)
        sketch.TryAddConstraint( std::move(c), false );

    suggestedConstraints.clear();
}

/// LINE
/// ========================================================
void LineSketchTool::End()
{
    SnapBallon::Show(SNAP_BALLOON_NONE);

    if (!currentLine)
        return;

    sketch.points.erase(currentLineIt->second);
    sketch.lines.erase(currentLineIt);
    currentLine = false;
}

bool LineSketchTool::LeftDown(const wxMouseEventEx&)
{
    if (!currentLine)
    {
        // create a new line from nothing
        SketchPointList::iterator p1 = sketch.points.add(mouse);
        SketchPointList::iterator p2 = sketch.points.add(mouse);
        currentLineIt = sketch.lines.add(p1, p2);
    }
    else
    {
        // if desired, set suggested constraints
        if (bAutoConstrain)
            PlaceSuggestedConstraints(sketch);

        // finish this line
        currentLineIt->bSelected = false;

        // continue previous line, by creating a new line that shares previous point
        SketchPointList::iterator p3 = sketch.points.add(mouse.x, mouse.y);
        currentLineIt = sketch.lines.add(currentLineIt->second, p3);
    }

    currentLineIt->bSelected = true;
    currentLineIt->type = creationStyle;
    currentLine = true;

    return true;
}

template<typename U>
static typename U::iterator RemoveConstness(U& container, const typename U::const_iterator& ci) {
    return std::find(container.begin(), container.end(), (const typename U::value_type&)*ci);
}

bool LineSketchTool::TestSnap(SketchSnaps::Vector2& out_snap)
{
    // if there is a current line being built, lets ignore it on some snaps (prevents snapping to self)
    // or use if for some snaps (like orthogonal or tangent) that require a partially defined line
    const SketchLine* currentLine = this->currentLine ? &*currentLineIt : NULL;
    const IgnoreTest_Single<SketchLine> ignoreCurrent(currentLine); //

    // saves snapped features to (possibly) place constraints
    // these iterators are const because SketchSnaps:: always takes (const Sketch&) and can only return a const reference to the features
    SketchPointList::const_iterator snap_iter_point;
    SketchLineList::const_iterator snap_iter_line;
    SketchCircleList::const_iterator snap_iter_circle;

    SetSuggested(); // no constraints are suggested by default

    if (SketchSnaps::ToLineEndpoint(sketch, ignoreCurrent, mouse, snap_iter_point))
    {
        out_snap = *snap_iter_point;

        if (currentLine)
        {
            auto coincident_endpoint = RemoveConstness(sketch.points, snap_iter_point);
            SetSuggested({
                new PointOnPointAxisConstraint(coincident_endpoint, currentLineIt->second, true),
                new PointOnPointAxisConstraint(coincident_endpoint, currentLineIt->second, false)
            });
        }

        SnapBallon::Show(SNAP_BALLOON_LINE_ENDPOINT);
    }

    else if (SketchSnaps::ToLineMidpoint(sketch, ignoreCurrent, mouse, snap_iter_line, out_snap))
    {
        if (currentLine)
        {
            auto coincident_line = RemoveConstness(sketch.lines, snap_iter_line);
            SetSuggested({
                new PointOnLineMidpoint(currentLineIt->second, coincident_line, true ),
                new PointOnLineMidpoint(currentLineIt->second, coincident_line, false ),
            });
        }

        SnapBallon::Show(SNAP_BALLOON_LINE_MIDPOINT);
    }

    else if (SketchSnaps::LineOrthogonal(sketch, currentLine, mouse, snap_iter_line, out_snap))
    {
        if (currentLine)
        {
            auto orthogonal_line = RemoveConstness(sketch.lines, snap_iter_line);
            SetSuggested({
                new OrthogonalConstraint(currentLineIt, orthogonal_line ),
                new PointOnLineConstraint(currentLineIt->second, orthogonal_line )
            });
        }

        SnapBallon::Show(SNAP_BALLOON_LINE_ORTHOGONAL);
    }

    else if (SketchSnaps::ToLine(sketch, ignoreCurrent, mouse, snap_iter_line, out_snap))
    {
        if (currentLine)
        {
            auto coincident_line = RemoveConstness(sketch.lines, snap_iter_line);
            SetSuggested({
                new PointOnLineConstraint(currentLineIt->second, coincident_line )
            });
        }

        SnapBallon::Show(SNAP_BALLOON_ON_LINE);
    }

    else if (SketchSnaps::ToArcCenter(sketch, NULL, mouse, snap_iter_point))
    {
        out_snap = *snap_iter_point;

        if (currentLine)
        {
            auto coincident_center = RemoveConstness(sketch.points, snap_iter_point);
            SetSuggested({
                new PointOnPointAxisConstraint(coincident_center, currentLineIt->second, true),
                new PointOnPointAxisConstraint(coincident_center, currentLineIt->second, false)
            });
        }

        SnapBallon::Show(SNAP_BALLOON_CIRCLE_CENTER);
    }

    else if (SketchSnaps::ToArcEndpoints(sketch, NULL, mouse, out_snap))
        SnapBallon::Show(SNAP_BALLOON_ARC_ENDPOINT);

    else if (SketchSnaps::LineTangentToCircle(sketch, currentLine, mouse, snap_iter_circle, out_snap))
    {
        if (currentLine)
        {
            auto tangent_circle = RemoveConstness(sketch.circles, snap_iter_circle);
            SetSuggested({
                new TangentLineConstraint(currentLineIt, tangent_circle ),
                new PointOnCircumferenceConstraint(currentLineIt->second, tangent_circle)
            });
        }

        SnapBallon::Show(SNAP_BALLOON_LINE_TANGENT);
    }

    else if (SketchSnaps::ToAnyCircleQuadrants(sketch, NULL, mouse, out_snap))
        SnapBallon::Show(SNAP_BALLOON_CIRCLE_QUADRANT);

    else if (SketchSnaps::ToArc(sketch, NULL, mouse, snap_iter_circle, out_snap))
    {
        if (currentLine)
        {
            auto snap_circle = RemoveConstness(sketch.circles, snap_iter_circle);
            SetSuggested({
                new PointOnCircumferenceConstraint(currentLineIt->second, snap_circle)
            });
        }

        SnapBallon::Show(SNAP_BALLOON_ON_ARC);
    }

    else if (SketchSnaps::LineHorizontal(currentLine, mouse, out_snap))
    {
        SnapBallon::Show(SNAP_BALLOON_LINE_HORIZONTAL);

        if (currentLine)
        SetSuggested({new HorizontalConstraint(currentLineIt) });
    }

    else if (SketchSnaps::LineVertical(currentLine, mouse, out_snap))
    {
        SnapBallon::Show(SNAP_BALLOON_LINE_VERTICAL);

        if (currentLine)
            SetSuggested({new VerticalConstraint(currentLineIt) });
    }

    else
    {
        SnapBallon::Show(SNAP_BALLOON_NONE);
        return false;
    }

    return true;
}

bool LineSketchTool::MouseMove(const wxMouseEventEx& evt)
{
    // get current mouse position
    {
        Coord x, y;
        evt.GetPosition(x, y);
        mouse = Vector2(x, y);
    }

    Vector2 snap_point;
    if (TestSnap(snap_point))
        mouse = snap_point;

    if (!currentLine)
        return false;

    currentLineIt->second->x = mouse.x;
    currentLineIt->second->y = mouse.y;

    return true;
}

/// CIRCLE
/// ========================================================
void CircleSketchTool::End()
{
    SnapBallon::Show(SNAP_BALLOON_NONE);

    if (!currentCircle)
        return;

    currentCircle = false;
    sketch.points.erase(currentCircleIt->center);
    sketch.points.erase(currentCircleIt->radius);
    sketch.circles.erase(currentCircleIt);
}

bool CircleSketchTool::LeftDown(const wxMouseEventEx&)
{
    if (!currentCircle)
    {
        // set center, start picking radius
        SketchPointList::iterator c = sketch.points.add(mouse);
        SketchPointList::iterator r = sketch.points.add(mouse);
        currentCircleIt = sketch.circles.add(c, r);
        currentCircleIt->type = creationStyle;
        currentCircleIt->bSelected = true;
        currentCircle = true;
        chooseTheta = false;
    }
    else
    {
        if (!chooseTheta)
            chooseTheta = true; // set radius, start picking angle
        else
        {
            // finish editing this circle
            //std::cout << "Finished creating circle with theta = " << currentCircleIt->GetAngleDeg() << std::endl;
            if (bAutoConstrain)
                PlaceSuggestedConstraints(sketch);

            currentCircleIt->bSelected = false;
            currentCircle = false;
        }
    }

    return true;
}

bool CircleSketchTool::TestSnap(SketchSnaps::Vector2& out_snap)
{
    const SketchCircle* currentCircle = this->currentCircle ? &*currentCircleIt : NULL;
    IgnoreTest_Single<SketchCircle> ignoreSelf(currentCircle);

    if (currentCircle && !chooseTheta)
        SetSuggested(); // erase previous constraints, unless we're choosing second point, in which case the previously chosen snap will be kept

    SketchPointList::const_iterator snap_iter_point;
    SketchLineList::const_iterator snap_iter_line;
    SketchCircleList::const_iterator snap_iter_circle;

    if (SketchSnaps::ToLineEndpoint(sketch, NULL, mouse, snap_iter_point))
    {
        out_snap = *snap_iter_point;

        if (currentCircle && !chooseTheta)
        {
            auto line_endpoint = RemoveConstness(sketch.points, snap_iter_point);
            SetSuggested({
                new PointOnPointAxisConstraint(currentCircleIt->radius, line_endpoint, false),
                new PointOnPointAxisConstraint(currentCircleIt->radius, line_endpoint, true)
            });
        }

        SnapBallon::Show(SNAP_BALLOON_LINE_ENDPOINT);
    }

    else if (SketchSnaps::ToLineMidpoint(sketch, NULL, mouse, snap_iter_line, out_snap))
    {
        if (currentCircle && !chooseTheta)
        {
            auto line_midpoint = RemoveConstness(sketch.lines, snap_iter_line);
            SetSuggested({
                new PointOnLineMidpoint(currentCircleIt->radius, line_midpoint, false),
                new PointOnLineMidpoint(currentCircleIt->radius, line_midpoint, true),
            });
        }

        SnapBallon::Show(SNAP_BALLOON_LINE_MIDPOINT);
    }

    else if (SketchSnaps::ToArcCenter(sketch, ignoreSelf, mouse, snap_iter_point))
    {
        out_snap = *snap_iter_point;

        if (currentCircle && !chooseTheta)
        {
            auto arc_center = RemoveConstness(sketch.points, snap_iter_point);
            SetSuggested({
                new PointOnPointAxisConstraint(currentCircleIt->radius, arc_center, false),
                new PointOnPointAxisConstraint(currentCircleIt->radius, arc_center, true)
            });
        }

        SnapBallon::Show(SNAP_BALLOON_CIRCLE_CENTER);
    }

    else if (SketchSnaps::ToArcEndpoints(sketch, ignoreSelf, mouse, out_snap))
        SnapBallon::Show(SNAP_BALLOON_ARC_ENDPOINT);

    else if (SketchSnaps::ToLine(sketch, NULL, mouse, snap_iter_line, out_snap))
    {
        if (currentCircle && !chooseTheta)
        {
            auto line_coincident = RemoveConstness(sketch.lines, snap_iter_line);
            SetSuggested({
                new PointOnLineConstraint(currentCircleIt->radius, line_coincident)
            });
        }

        SnapBallon::Show(SNAP_BALLOON_ON_LINE);
    }

    else if (chooseTheta)
    {
        if (SketchSnaps::ToCirclesIntersection(sketch, currentCircle, mouse, out_snap))
            SnapBallon::Show(SNAP_BALLOON_ARC_INTERSECTION);

        else if (SketchSnaps::ToCircleOwnQuadrants(sketch, currentCircle, mouse, out_snap))
            SnapBallon::Show(SNAP_BALLOON_CIRCLE_QUADRANT);

        else
        {
            SnapBallon::Show(SNAP_BALLOON_NONE);
            return false;
        }
    }
    else
    {
        if (SketchSnaps::ToArc(sketch, ignoreSelf, mouse, snap_iter_circle, out_snap))
        {
            if (currentCircle && !chooseTheta)
            {
                auto circle_coincident = RemoveConstness(sketch.circles, snap_iter_circle);
                SetSuggested({
                    new PointOnCircumferenceConstraint(currentCircleIt->radius, circle_coincident)
                });
            }

            SnapBallon::Show(SNAP_BALLOON_ON_ARC);
        }

        else if (SketchSnaps::ToArcOwnAxis(sketch, currentCircle, mouse, out_snap))
            SnapBallon::Show(SNAP_BALLOON_CIRCLE_QUADRANT);

        else
        {
            SnapBallon::Show(SNAP_BALLOON_NONE);
            return false;
        }
    }

    return true;
}

bool CircleSketchTool::MouseMove(const wxMouseEventEx& evt)
{
    // get current mouse position
    {
        Coord x, y;
        evt.GetPosition(x, y);
        mouse = Vector2(x, y);
    }

    // attempt snaps
    Vector2 snap_point;
    if ( TestSnap(snap_point) )
        mouse = snap_point;

    if (!currentCircle)
        return false;

    if (!chooseTheta)
    {
        // moving first endpoint
        currentCircleIt->radius->x = mouse.x;
        currentCircleIt->radius->y = mouse.y;
    }
    else
    {
        // changing angle
        Coord mouse_angle = currentCircleIt->GetPointAngle(mouse);
        if (mouse_angle <= 0)
            mouse_angle += 2.0f*glm::pi<decltype(mouse_angle)>();

        currentCircleIt->SetAngleRad(mouse_angle);
    }

    return true;
}

/// RECTANGLE
/// ========================================================

/// GENERIC RECTANGLE
struct SketchRectangle
{
private:
    bool bValid = false;

protected:
    SketchPointList::iterator a;
    SketchPointList::iterator b;
    SketchPointList::iterator c;
    SketchPointList::iterator d;

    SketchLineList::iterator ab;
    SketchLineList::iterator bc;
    SketchLineList::iterator cd;
    SketchLineList::iterator da;

public:
    using Coord = Sketch::Coord;
    using Vector2 = Sketch::Vector2;

    SketchRectangle() = default;
    virtual ~SketchRectangle() = default;

    LineIgnoreTestHandler selfFilter;

    // initializes iterators a, b, c, d and lines ab, bc.... etc on the sketch
    virtual void InsertOnSketch(Sketch& sketch, const Vector2& where)
    {
        a = sketch.points.add(where);
        b = sketch.points.add(where);
        c = sketch.points.add(where);
        d = sketch.points.add(where);

        ab = sketch.lines.add(a, b);
        ab->type = CreationSketchTool::creationStyle;

        bc = sketch.lines.add(b, c);
        bc->type = CreationSketchTool::creationStyle;

        cd = sketch.lines.add(c, d);
        cd->type = CreationSketchTool::creationStyle;

        da = sketch.lines.add(d, a);
        da->type = CreationSketchTool::creationStyle;

        bValid = true;
    }

    // remove self from sketch in case creation is canceled
    // no need to sketch.constraints.erase_associated because surely this new-born rectangle has no constraints
    virtual void EraseFromSketch(Sketch& sketch)
    {
        bValid = false;

        sketch.lines.erase(ab);
        sketch.lines.erase(bc);
        sketch.lines.erase(cd);
        sketch.lines.erase(da);

        sketch.points.erase(a);
        sketch.points.erase(b);
        sketch.points.erase(c);
        sketch.points.erase(d);
    }

    // sets bSelected on all features
    virtual void Select(bool bSelect = true)
    {
        ab->bSelected = bSelect;
        bc->bSelected = bSelect;
        cd->bSelected = bSelect;
        da->bSelected = bSelect;
    }

    // when placing on the sketch for good ...
    // ... create the constraints that make this rectangle a rectangle
    virtual void CommitConstraints(Sketch& sketch)
    {
        sketch.TryAddConstraint( std::make_unique<HorizontalConstraint>(ab) );
        sketch.TryAddConstraint( std::make_unique<HorizontalConstraint>(cd) );
        sketch.TryAddConstraint( std::make_unique<VerticalConstraint>(bc) );
        sketch.TryAddConstraint( std::make_unique<VerticalConstraint>(da) );
    }

    // move one corder during creation
    virtual void MoveCorner(const Vector2& p) = 0;
    inline bool bOk() const { return bValid; }
};

/// RECTANGLE DEFINED BY 2 CORNERS
struct SketchRectangle_ByCorner : public SketchRectangle
{
public:
    SketchRectangle_ByCorner() = default;

    void InsertOnSketch(Sketch& sketch, const Vector2& where)
    {
        // define basic rect
        SketchRectangle::InsertOnSketch(sketch, where);

        // create list of ignored snaps
        selfFilter = IgnoreTest_Multiple<SketchLine, 4>({&*ab, &*bc, &*cd, &*da});
    }

    // point A is static (fixed on creation), mouse moves opposite point C
    void MoveCorner(const Vector2& p)
    {
        c->x = p.x;
        c->y = p.y;

        b->x = p.x;
        b->y = a->y;

        d->x = a->x;
        d->y = p.y;
    }
};

/// RECTANGLE DEFINED BY CENTER AND 1 CORNER
// a ------- b
// |    p    |
// d ------- c
struct SketchRectangle_ByCenter : public SketchRectangle
{
protected:
    // this triangle has one additional center point ...
    SketchPointList::iterator p;

    // and four additional construction lines (diagonals)
    SketchLineList::iterator ap;
    SketchLineList::iterator bp;
    SketchLineList::iterator cp;
    SketchLineList::iterator dp;

public:
    SketchRectangle_ByCenter() = default;

    void InsertOnSketch(Sketch& sketch, const Vector2& where)
    {
        // define basic rect
        SketchRectangle::InsertOnSketch(sketch, where);

        // define construction lines
        p = sketch.points.add(where);
        ap = sketch.lines.add(a, p);
        bp = sketch.lines.add(b, p);
        cp = sketch.lines.add(c, p);
        dp = sketch.lines.add(d, p);

        ap->type = SketchFeatureType::CONSTRUCTION;
        bp->type = SketchFeatureType::CONSTRUCTION;
        cp->type = SketchFeatureType::CONSTRUCTION;
        dp->type = SketchFeatureType::CONSTRUCTION;

        // define snap auto-filter
        selfFilter = IgnoreTest_Multiple<SketchLine, 8>({&*ab, &*bc, &*cd, &*da, &*ap, &*bp, &*cp, &*dp});
    }

    void EraseFromSketch(Sketch& sketch)
    {
        // erase extra features
        sketch.lines.erase(ap);
        sketch.lines.erase(bp);
        sketch.lines.erase(cp);
        sketch.lines.erase(dp);

        sketch.points.erase(p);

        // erase basic rectangle
        SketchRectangle::EraseFromSketch(sketch);
    }

    void Select(bool bSelect = true)
    {
        SketchRectangle::Select(bSelect);
        ap->bSelected = bSelect;
        bp->bSelected = bSelect;
        cp->bSelected = bSelect;
        dp->bSelected = bSelect;
    }

    void MoveCorner(const Vector2& q)
    {
        a->x = q.x;
        a->y = q.y;

        const Coord dx = p->x - a->x;
        const Coord dy = p->y - a->y;

        c->x = p->x + dx;
        c->y = p->y + dy;

        b->x = c->x;
        b->y = a->y;

        d->x = a->x;
        d->y = c->y;
    }

    void CommitConstraints(Sketch& sketch)
    {
        SketchRectangle::CommitConstraints(sketch);

        sketch.TryAddConstraint( std::make_unique<ParallelConstraint>(ap, cp) );
        sketch.TryAddConstraint( std::make_unique<ParallelConstraint>(bp, dp) );
    }
};

/// RECTANGLE CREATION TOOL
RectangleSketchTool::RectangleSketchTool(Sketch& s) :
    CreationSketchTool(s)
{

}

void RectangleSketchTool::End()
{
    if (!currentRectangle)
        return;

    currentRectangle->EraseFromSketch(sketch);
    currentRectangle.reset();
}

bool RectangleSketchTool::LeftDown(const wxMouseEventEx&)
{
    if (!currentRectangle)
    {
        startOver:

        // start creating rectangle
        if (creationMode == RECT_BY_CENTER)
            currentRectangle.reset( new SketchRectangle_ByCenter() );
        else
            currentRectangle.reset( new SketchRectangle_ByCorner() );

        currentRectangle->InsertOnSketch(sketch, mouse);
        currentRectangle->Select(true);
    }
    else
    {
        if (!currentRectangle->bOk())
        {
            currentRectangle.reset();
            goto startOver;
        }

        // end triangle
        currentRectangle->CommitConstraints(sketch);
        currentRectangle->Select(false);
        currentRectangle.reset();
    }

    return true;
}

static bool TestRectangleSnaps(const Sketch& sketch, const SketchRectangle* currentRect, const SketchSnaps::Vector2& mouse, SketchSnaps::Vector2& out_snap)
{
    SketchPointList::const_iterator snap_iter_point;

    if (SketchSnaps::ToLineEndpoint(sketch, (currentRect) ? currentRect->selfFilter : NULL, mouse, snap_iter_point))
    {
        out_snap = *snap_iter_point;
        SnapBallon::Show(SNAP_BALLOON_LINE_ENDPOINT);
    }

    else if (SketchSnaps::ToLineMidpoint(sketch, (currentRect) ? currentRect->selfFilter : NULL, mouse, out_snap))
        SnapBallon::Show(SNAP_BALLOON_LINE_MIDPOINT);

    else if (SketchSnaps::ToArcEndpoints(sketch, NULL, mouse, out_snap))
        SnapBallon::Show(SNAP_BALLOON_ARC_ENDPOINT);

    else if (SketchSnaps::ToArcCenter(sketch, NULL, mouse, snap_iter_point))
    {
        out_snap = *snap_iter_point;
        SnapBallon::Show(SNAP_BALLOON_CIRCLE_CENTER);
    }

    else if (SketchSnaps::ToAnyCircleQuadrants(sketch, NULL, mouse, out_snap))
        SnapBallon::Show(SNAP_BALLOON_CIRCLE_QUADRANT);

    else if (SketchSnaps::ToArc(sketch, NULL, mouse, out_snap))
        SnapBallon::Show(SNAP_BALLOON_ON_ARC);

    else if (SketchSnaps::ToLine(sketch, (currentRect) ? currentRect->selfFilter : NULL, mouse, out_snap))
        SnapBallon::Show(SNAP_BALLOON_ON_LINE);

    else
    {
        SnapBallon::Show(SNAP_BALLOON_NONE);
        return false;
    }

    return true;
}

bool RectangleSketchTool::MouseMove(const wxMouseEventEx& evt)
{
    // get current mouse position
    {
        Coord x, y;
        evt.GetPosition(x, y);
        mouse = Vector2(x, y);
    }

    // attempt snaps
    Vector2 snap_point;
    if (TestRectangleSnaps(sketch, currentRectangle.get(), mouse, snap_point))
        mouse = snap_point;

    if (!currentRectangle)
        return false;

    if (!currentRectangle->bOk())
        return false;

    currentRectangle->MoveCorner(mouse);

    return true;
}

/// DIMENSION
/// ========================================================
#include "DimensionEditDialog.h"

template<class T, class...Args>
bool DimensionSketchTool::PlaceDimension(SketchDimensionType type, Args... args)
{
    // create the dimension
    T* dim = new T(args...);

    // configure it
    dim->SetType( type );
    dim->bSelected = true;
    dim->DesiredValue = dim->GetValue();

    // try adding it to the sketch (may fail)
    if ((currentDimension = sketch.TryAddConstraint(std::unique_ptr<SketchConstraint>(dim))) != sketch.constraints.end())
    {
        state |= DimensionSketchToolState::DSTS_DROPDIM;
        return true;
    }

    state |= DimensionSketchToolState::DSTS_RESET;
    return false;
}

bool DimensionSketchTool::OnPointPoint(SketchPointList::iterator& first, SketchPointList::iterator& second)
{
    if (first == second)
        return false;

    PlaceDimension<SketchDimensionLinear>(SketchDimensionType::DIMENSION_LINEAR_ALIGNED, first, second);
    return true;
}

bool DimensionSketchTool::OnLineLine(SketchLineList::iterator& first, SketchLineList::iterator& second)
{
    // both lines are the same - measure the line itself
    if (first == second)
    {
        PlaceDimension<SketchDimensionLinear>(SketchDimensionType::DIMENSION_LINEAR_ALIGNED, first->first, first->second);
        return true;
    }

    // lines are different
    // if parallel lines, measure distance, otherwise measure angle

    const auto& a = first->first;
    const auto& b = first->second;
    const auto& c = second->first;
    const auto& d = second->second;
    const Vector2 ba = *b - *a;
    const Vector2 dc = *d - *c;

    if (almost_zero( ba.x*dc.y - dc.x*ba.y ))
    {
        // LINES ARE PARALLEL
        // on the last line, pick point closest to mouse
        // on the first line, pick point closest to second
        SketchPointList::iterator P1 = (glm::distance2(mouse, *c) <  glm::distance2(mouse, *d)) ? c : d;
        SketchPointList::iterator P2 = (glm::distance2((const Vector2&)*P1, *a) <  glm::distance2((const Vector2&)*P1, *b)) ? a : b;

        PlaceDimension<SketchDimensionLinear>(SketchDimensionType::DIMENSION_LINEAR_ALIGNED, P1, P2);
        return true;
    }
    else
    {
        // LINES ARE *NOT* PARALLEL
        PlaceDimension<SketchDimensionAngular>(SketchDimensionType::DIMENSION_ANGULAR, first, second);
        return true;
    }
}

bool DimensionSketchTool::OnPointCircle(SketchPointList::iterator& point, SketchCircleList::iterator& circle)
{
    if (circle->center != point)
        return OnPointPoint(point, circle->center);

    PlaceDimension<SketchDimensionCircular>(SketchDimensionType::DIMENSION_CIRCULAR_RADIUS, circle);
    return true;
}

bool DimensionSketchTool::OnCircleCircle(SketchCircleList::iterator& circle, SketchCircleList::iterator& other)
{
    if (circle != other)
        return OnPointPoint(circle->center, other->center);

    PlaceDimension<SketchDimensionCircular>(SketchDimensionType::DIMENSION_CIRCULAR_DIAMETER, circle);
    return true;
}

void DimensionSketchTool::End()
{
    ConsecutiveSelectionSketchTool::End();

    if ((state & DimensionSketchToolState::DSTS_DROPDIM) && !(state & DimensionSketchToolState::DSTS_EDITDIM))
        sketch.constraints.erase(currentDimension);
}

bool DimensionSketchTool::LeftDown(const wxMouseEventEx& evt)
{
    // clicked where the dimension will be dropped - end this dimension edit
    if (state & DimensionSketchToolState::DSTS_DROPDIM)
    {
        dynamic_cast<SketchDimension*>(currentDimension->get())->bSelected = false;
        state = DimensionSketchToolState::DSTS_RESET;
        return true;
    }
    // pick dimension under the mouse to edit
    else if (dynamic_cast<SketchDimension*>(highlight) != NULL)
    {
        state |= DimensionSketchToolState::DSTS_DROPDIM | DimensionSketchToolState::DSTS_EDITDIM;
        return true;
    }

    // pick other features (lines, points, etc)
    return ConsecutiveSelectionSketchTool::LeftDown(evt);
}

bool DimensionSketchTool::LeftDouble(const wxMouseEventEx& evt)
{
    // sanity check
    if (!(state & DimensionSketchToolState::DSTS_DROPDIM))
        return false; // no current dimension to edit

    SketchDimension* dim = dynamic_cast<SketchDimension*>(currentDimension->get());
    if (!dim)
        return false;

    // save the desired value in case the solution fails
    const auto saved = dim->DesiredValue;

    // display prompt to get new dimension
    if (!DimensionEditDialog::Show(dim))
        return false;

    // update sketch
    if (!sketch.Solve())
        dim->DesiredValue = saved; // restore previous if unsolvable

    state = DimensionSketchToolState::DSTS_RESET; // finish editing
    return true;
}

bool DimensionSketchTool::MouseMove(const wxMouseEventEx& evt)
{
    // update mouse position
    // get features (point, line, circle etc) under the mouse
    SketchSelectable* toSelect = GetHover(evt);

    // if moving dimension, dont update highlights
    if (state & DimensionSketchToolState::DSTS_DROPDIM)
    {
        // move dimension around
        SketchDimension* dim = dynamic_cast<SketchDimension*>(currentDimension->get());
        if (!dim)
            return false;

        dim->Offset = mouse - dim->GetOrigin();

        // if a linear dimension, then choose to align horizontally, vertically or diagonally
        if (dim->GetType() & SketchDimensionType::DIMENSION_LINEAR)
        {
            const SketchDimensionLinear* lin = dynamic_cast<const SketchDimensionLinear*>(dim);
            if (!lin)
                return true;

            const auto xmax = std::max(lin->first->x, lin->second->x);
            const auto xmin = std::min(lin->first->x, lin->second->x);
            const auto ymax = std::max(lin->first->y, lin->second->y);
            const auto ymin = std::min(lin->first->y, lin->second->y);

            if (mouse.x > xmin && mouse.x < xmax && mouse.y > ymin && mouse.y < ymax)
                dim->SetType(SketchDimensionType::DIMENSION_LINEAR_ALIGNED);

            else
            {
                const Vector2 mouse_dir = glm::normalize( mouse - dim->GetOrigin() );

                if ((glm::abs(mouse_dir.x) > glm::abs(mouse_dir.y)) && !almost_equal(ymax, ymin) )
                    dim->SetType(SketchDimensionType::DIMENSION_LINEAR_VERTICAL);
                else if (!almost_equal(xmax, xmin))
                    dim->SetType(SketchDimensionType::DIMENSION_LINEAR_HORIZONTAL);
                else
                    dim->SetType(SketchDimensionType::DIMENSION_LINEAR_ALIGNED);
            }
        }

        // when moving a dimension it's type (and measurement) may change, so also update the desired value to match current value
        dim->DesiredValue = dim->GetValue();

        return true;
    }

    // not moving a dimension - attempt selecting stuff

    // 1. attempt selecting a dimension under the mouse
    if (!toSelect)
    {
        Coord minDist = std::numeric_limits<Coord>::max();
        for (auto dit = sketch.constraints.begin(); dit != sketch.constraints.end(); ++dit)
        {
            Coord dist;
            if (!dit->get()->HitBB(mouse, dist))
                continue;

            SketchDimension* dim = dynamic_cast<SketchDimension*>(dit->get());
            if (!dim)
                continue;

            if (dist < minDist)
            {
                minDist = dist;
                toSelect = dim;
                currentDimension = dit;
            }
        }
    }

    // 2. attempt selecting other sketch features under the mouse
    return UpdateHighlight(toSelect);
}

bool DimensionSketchTool::KeyDown(wxKeyEvent& evt)
{
    if (evt.GetKeyCode() != WXK_DELETE)
        return false;

    // can't delete dimension if none is selected
    if (!(state & DimensionSketchToolState::DSTS_DROPDIM))
        return false;

    // erase dimension and reset the state variables
    sketch.constraints.erase(currentDimension);
    state = DimensionSketchToolState::DSTS_RESET;
    highlight = NULL;
    previous = NULL;

    return true;
}

/// GEOMETRIC CONSTRAINT PLACEMENT
/// ========================================================
// TEMPLATED IN .h FILE
// HorizontalSketchTool
// VerticalSketchTool

// CoincidentSketchTool
bool CoincidentSketchTool::OnPointPoint(SketchPointList::iterator& a, SketchPointList::iterator& b)
{
    if (a == b)
        return false;

    sketch.TryAddConstraints({
        std::make_unique<PointOnPointAxisConstraint>(a, b, true),
        std::make_unique<PointOnPointAxisConstraint>(a, b, false)
    }, 2);

    return true;
}

bool CoincidentSketchTool::OnPointLine(SketchPointList::iterator& p, SketchLineList::iterator& l)
{
    if (l->IsEndpoint(p))
        return false;

    sketch.TryAddConstraint(std::make_unique<PointOnLineConstraint>(p,  l));

    return true;
}

bool CoincidentSketchTool::OnPointCircle(SketchPointList::iterator& p, SketchCircleList::iterator& c)
{
    if (c->IsCenterOrRadius(p))
        return false;

    sketch.TryAddConstraint(std::make_unique<PointOnCircumferenceConstraint>(p,  c));

    return true;
}

// ColinearSketchTool
bool ColinearSketchTool::OnLineLine(SketchLineList::iterator& first, SketchLineList::iterator& second)
{
    if (first == second)
        return false; // no same twice

    sketch.TryAddConstraints({
        std::make_unique<PointOnLineConstraint>(first->first,  second),
        std::make_unique<PointOnLineConstraint>(first->second, second)
    }, 2);

    return true;
}

// ConcentricSketchTool
bool ConcentricSketchTool::OnCircleCircle(SketchCircleList::iterator& first, SketchCircleList::iterator& second)
{
    if (first == second)
        return false;

    sketch.TryAddConstraints({
       std::make_unique<PointOnPointAxisConstraint>(first->center, second->center, true),
       std::make_unique<PointOnPointAxisConstraint>(first->center, second->center, false)
    }, 2);

    return true;
}

// TangentSketchTool
bool TangentSketchTool::OnLineCircle(SketchLineList::iterator& line, SketchCircleList::iterator& circle)
{
    sketch.TryAddConstraint( std::make_unique<TangentLineConstraint>(line, circle) );

    return true;
}

bool TangentSketchTool::OnCircleCircle(SketchCircleList::iterator& first, SketchCircleList::iterator& second)
{
    if (first == second)
        return false;

    sketch.TryAddConstraint( std::make_unique<TangentCircleConstraint>(first, second) );

    return true;
}

// EqualSketchTool
bool EqualSketchTool::OnLineLine(SketchLineList::iterator& first, SketchLineList::iterator& second)
{
    if (first == second)
        return false;

    sketch.TryAddConstraint( std::make_unique<EqualLengthConstraint>(first, second) );

    return true;
}

bool EqualSketchTool::OnCircleCircle(SketchCircleList::iterator& first, SketchCircleList::iterator& second)
{
    if (first == second)
        return false;

    sketch.TryAddConstraint( std::make_unique<EqualRadiusConstraint>(first, second) );

    return true;
}

// ParallelSketchTool
bool ParallelSketchTool::OnLineLine(SketchLineList::iterator& first, SketchLineList::iterator& second)
{
    if (first == second)
        return false;

    sketch.TryAddConstraint( std::make_unique<ParallelConstraint>(first, second) );

    return true;
}

// OrthogonalSketchTool
bool OrthogonalSketchTool::OnLineLine(SketchLineList::iterator& first, SketchLineList::iterator& second)
{
    if (first == second)
        return false;

     sketch.TryAddConstraint( std::make_unique<OrthogonalConstraint>(first, second) );

    return true;
}
