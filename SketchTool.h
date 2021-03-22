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

#ifndef _SKETCH_TOOLS_H_
#define _SKETCH_TOOLS_H_

#include "sketch.h"
#include <wx/event.h>
#include "enum_flag_operators.h"
#include "ExtendedMouseEvent.h"
#include "SketchDimensionalConstraints.h"
#include "SketchGeometricConstraints.h"

/// GENERIC
/// ========================================================
class SketchTool
{
protected:
    Sketch& sketch;

public:
    using Coord = Sketch::Coord;
    using Vector2 = Sketch::Vector2;

    SketchTool(Sketch& s) : sketch(s){ }
    virtual ~SketchTool() = default;

    // called when tool enters/leaves
    virtual void Begin();
    virtual void End();

    // called when panel receives that event
    virtual bool LeftUp(const wxMouseEventEx& evt);
    virtual bool LeftDown(const wxMouseEventEx& evt);
    virtual bool LeftDouble(const wxMouseEventEx& evt);
    virtual bool MouseMove(const wxMouseEventEx& evt);
    virtual bool KeyUp(wxKeyEvent& evt);
    virtual bool KeyDown(wxKeyEvent& evt);
};

/// SELECTION
/// ========================================================
enum SketchSelectionFilter
{
    SSF_POINT       = 1,
    SSF_LINE        = 2,
    SSF_CIRCLE      = 4,
    SSF_MULTIPLE    = 256,
    SSF_ANY         = SSF_POINT | SSF_LINE | SSF_CIRCLE,
};
DEFINE_ENUM_FLAG_OPERATORS(SketchSelectionFilter)

class SelectionSketchTool : public SketchTool
{
protected:
    SketchSelectable* highlight = NULL;     // object currently focused by the mouse
    std::list<SketchSelectable*> selection; // objects that have actually been selected
    Vector2 mouse;                        // cursor position updated every OnMouseMove event
    Vector2 dragStart;                    // set when LeftDown is called to keep track of dragging motion

    void ClearSelection();                  // clears the list "selection" and sets each element bSelected to false
    bool MoveDrag();                        // relayed by MouseMove event when dragging the mouse (selection rectangle)
    bool MoveNotDrag();                     // relayed by MouseMove event when *NOT* dragging the mouse (single click selection)
    const std::function<void(bool, const Vector2&, const Vector2&)> DisplayDragBox;

public:
    inline SelectionSketchTool(Sketch& s, std::function<void(bool, const Vector2&, const Vector2&)> boxcallback) :
        SketchTool(s), DisplayDragBox(boxcallback) {}

    inline bool IsSelected(const SketchSelectable* t) const
    {
        return (std::find(selection.cbegin(), selection.cend(), t) != selection.cend());
    }

    SketchSelectionFilter Filter = SketchSelectionFilter(SketchSelectionFilter::SSF_ANY | SketchSelectionFilter::SSF_MULTIPLE);

    void Begin();
    void End();
    bool LeftUp(const wxMouseEventEx& evt);
    bool LeftDown(const wxMouseEventEx& evt);
    bool MouseMove(const wxMouseEventEx& evt);
    bool KeyDown(wxKeyEvent& evt);
};

/// DRAG/MOVE objects
/// ========================================================
class MoveSketchTool : public SketchTool
{
public:
    MoveSketchTool(Sketch& s); // must declare and implement constructor because of incomplete type struct MoveSketchTool_Filter

    bool LeftUp(const wxMouseEventEx& evt);
    bool LeftDown(const wxMouseEventEx& evt);
    bool MouseMove(const wxMouseEventEx& evt);

    struct MoveSketchTool_Filter;

protected:
    std::list<std::pair<SketchPointList::iterator, Vector2>> initialPositions;
    Vector2 mouse;     // cursor position updated every OnMouseMove event
    Vector2 dragStart; // set when LeftDown is called to keep track of dragging motion
    std::unique_ptr<MoveSketchTool_Filter> filter;
};

/// COMMON BASES
/// ========================================================

// SINGLE SELECTION TOOL
// Picks a single object of filtered type
// =========================================================
class SingleSelectionEditTool: public SketchTool
{
protected:
    SketchSelectionFilter Filter = SketchSelectionFilter::SSF_ANY;

    SketchSelectable* highlight = NULL;         // object currently focused by the mouse
    SketchPointList::iterator currentPoint;     // valid only if highlight can dynamic cast to SketchPoint*
    SketchLineList::iterator currentLine;       // valid only if highlight can dynamic cast to SketchLine*
    SketchCircleList::iterator currentCircle;   // valid only if highlight can dynamic cast to SketchCircle*

    Vector2 mouse;

    SketchSelectable* GetHover(const wxMouseEventEx& evt);
    bool UpdateHighlight(SketchSelectable* const toSelect);

public:
    using SketchTool::SketchTool;

    void Begin();
    void End();
    bool MouseMove(const wxMouseEventEx& evt);
};

// CONSECUTIVE SELECTION TOOL
// Picks two objects of filtered type in sequence
// Dispatches a function to treat combination accordingly
// =========================================================
class AbstractConsecutiveSelectionSketchTool : public SingleSelectionEditTool
{
protected:
    SketchSelectable* previous = NULL;
    SketchPointList::iterator previousPoint;
    SketchLineList::iterator previousLine;
    SketchCircleList::iterator previousCircle;

    // pure virtuals to make sure the compiler will not call on 'this'
    virtual bool OnPointPoint(SketchPointList::iterator&, SketchPointList::iterator&) = 0;
    virtual bool OnPointLine(SketchPointList::iterator&, SketchLineList::iterator&) = 0;
    virtual bool OnPointCircle(SketchPointList::iterator&, SketchCircleList::iterator&) = 0;
    virtual bool OnLineLine(SketchLineList::iterator&, SketchLineList::iterator&) = 0;
    virtual bool OnLineCircle(SketchLineList::iterator&, SketchCircleList::iterator&) = 0;
    virtual bool OnCircleCircle(SketchCircleList::iterator&, SketchCircleList::iterator&) = 0;

public:
    using SingleSelectionEditTool::SingleSelectionEditTool;
    virtual bool LeftDown(const wxMouseEventEx& evt);
};

class ConsecutiveSelectionSketchTool : public AbstractConsecutiveSelectionSketchTool
{
protected:
    // functions bellow are implemented trivially (just return false) to spare actual derived class from having to implement the unused everytime
    virtual bool OnPointPoint(SketchPointList::iterator&, SketchPointList::iterator&);
    virtual bool OnPointLine(SketchPointList::iterator&, SketchLineList::iterator&);
    virtual bool OnPointCircle(SketchPointList::iterator&, SketchCircleList::iterator&);
    virtual bool OnLineLine(SketchLineList::iterator&, SketchLineList::iterator&);
    virtual bool OnLineCircle(SketchLineList::iterator&, SketchCircleList::iterator&);
    virtual bool OnCircleCircle(SketchCircleList::iterator&, SketchCircleList::iterator&);

public:
    using AbstractConsecutiveSelectionSketchTool::AbstractConsecutiveSelectionSketchTool;
};

/// SPLIT/TRIM
/// ========================================================
class SplitSketchTool : public SingleSelectionEditTool
{
public:
    inline SplitSketchTool(Sketch& sketch) : SingleSelectionEditTool(sketch) { Filter = SketchSelectionFilter::SSF_LINE | SketchSelectionFilter::SSF_CIRCLE; }
    bool LeftDown(const wxMouseEventEx& evt);
};

class TrimSketchTool : public SingleSelectionEditTool
{
public:
    inline TrimSketchTool(Sketch& sketch) : SingleSelectionEditTool(sketch) { Filter = SketchSelectionFilter::SSF_LINE | SketchSelectionFilter::SSF_CIRCLE; }
    bool LeftDown(const wxMouseEventEx& evt);
};

/// EXTEND
/// ========================================================
class ExtendSketchTool : public SingleSelectionEditTool
{
protected:
    void Invalidate();                              // deletes previous state
    void NewProposal(SketchLineList::iterator line, SketchPointList::iterator endpoint);   // attempts creating extension proposal from given line

    bool bCurrent = false;                   // is there an extension being proposed? (means ALL the fields below are valid)
    SketchLineList::iterator proposal_line; // the line for which the extension proposal was built (not the same as currentLine, which is the current mouse selection)
    SketchPointList::iterator near_endpoint; // the endpoint of original line that will be pushed further
    SketchLineList::iterator temporaryLine;  // the dummy line displayed on screen
    SketchPointList::iterator far_endpoint;  // the endpoint of the temporary line on the stopping intersection

public:
    inline ExtendSketchTool(Sketch& sketch) : SingleSelectionEditTool(sketch) { Filter = SketchSelectionFilter::SSF_LINE; }

    void End();
    bool LeftDown(const wxMouseEventEx& evt);
    bool MouseMove(const wxMouseEventEx& evt);
};

/// AUTOMATIC CONSTRAINT TOOL
/// ========================================================

// implements utilities to suggest constraints during feature creation
// and then to actually place those constraints when creation is done, if desired
struct AutomaticConstraintPlacement
{
public:
    static bool bAutoConstrain; // should constraints be placed automatically during feature creation?

private:
    std::list<std::unique_ptr<SketchConstraint>> suggestedConstraints;

protected:
    void SetSuggested(std::list<SketchConstraint*>&& suggested = {});
    void PlaceSuggestedConstraints(Sketch& sketch);
};

/// CREATION TOOL
/// ========================================================
class CreationSketchTool : public SketchTool
{
public:
    static enum SketchFeatureType creationStyle;        // all lines/circles/rectangles will be created with this style
    static void ApplyToCurrentSelection(Sketch& s);     // all selected entities will be set to this style
    using SketchTool::SketchTool;
};

/// LINE
/// ========================================================
class LineSketchTool : public CreationSketchTool, public AutomaticConstraintPlacement
{
private:
    Vector2 mouse;
    bool currentLine = false; // is the iterator below valid ?
    SketchLineList::iterator currentLineIt;
    bool TestSnap(Vector2& out_snap);

public:
    using CreationSketchTool::CreationSketchTool;

    void End();
    bool LeftDown(const wxMouseEventEx& evt);
    bool MouseMove(const wxMouseEventEx& evt);
};

/// CIRCLE
/// ========================================================
class CircleSketchTool : public CreationSketchTool, public AutomaticConstraintPlacement
{
private:
    Vector2 mouse;

    bool currentCircle = false;                 // is the iterator below valid ?
    SketchCircleList::iterator currentCircleIt; // the circle currently being created
    bool chooseTheta = false;                   // currently choosing the arc angle

    bool TestSnap(Vector2& out_snap);

public:
    using CreationSketchTool::CreationSketchTool;

    void End();
    bool LeftDown(const wxMouseEventEx& evt);
    bool MouseMove(const wxMouseEventEx& evt);
};

/// RECTANGLE
/// ========================================================
struct SketchRectangle;
class RectangleSketchTool : public CreationSketchTool
{
private:
    Vector2 mouse;
    std::unique_ptr<SketchRectangle> currentRectangle;

public:
    RectangleSketchTool(Sketch& s); // incomplete type struct SketchRectangle

    enum RectangleSketchMode {
        RECT_BY_CORNER,
        RECT_BY_CENTER
    } creationMode = RECT_BY_CENTER;

    void End();
    bool LeftDown(const wxMouseEventEx& evt);
    bool MouseMove(const wxMouseEventEx& evt);
};

/// DIMENSION CONSTRAINT PLACEMENT
/// ========================================================
enum DimensionSketchToolState
{
    DSTS_RESET          = 0,  // nothing selected
    DSTS_DROPDIM        = 1,  // dimension has just been created and it's being moved to drop in place
    DSTS_EDITDIM        = 2,  // the current dimension is not being created - it's being edited

    DSTS_FIRST_POINT    = 4,  // 1 point has been selected
    DSTS_SECOND_POINT   = 8,  // 2 points have been selected

    DSTS_FIRST_LINE     = 16, // 1 line has been selected
    DSTS_SECOND_LINE    = 32, // 2 lines have been selected

    DSTS_CIRCLE         = 64, // a circle has been selected
};
DEFINE_ENUM_FLAG_OPERATORS(DimensionSketchToolState)

class DimensionSketchTool : public ConsecutiveSelectionSketchTool
{
private:
    DimensionSketchToolState state = DimensionSketchToolState::DSTS_RESET;
    SketchConstraintList::iterator currentDimension; // valid only if DSTS_DROPDIM is set

    template<class T, class...Args> bool PlaceDimension(SketchDimensionType type, Args... args);

public:
    inline DimensionSketchTool(Sketch& s) : ConsecutiveSelectionSketchTool(s) { Filter = SketchSelectionFilter::SSF_ANY; }

    bool OnPointPoint(SketchPointList::iterator&, SketchPointList::iterator&);
    bool OnPointCircle(SketchPointList::iterator&, SketchCircleList::iterator&);
    bool OnLineLine(SketchLineList::iterator&, SketchLineList::iterator&);
    bool OnCircleCircle(SketchCircleList::iterator&, SketchCircleList::iterator&);

    void End();
    bool LeftDown(const wxMouseEventEx& evt);
    bool LeftDouble(const wxMouseEventEx& evt);
    bool MouseMove(const wxMouseEventEx& evt);
    bool KeyDown(wxKeyEvent& evt);
};

/// GEOMETRIC CONSTRAINT PLACEMENT
/// ========================================================
template<class T>
class OneLineConstraintSketchTool : public SingleSelectionEditTool
{
public:
    inline OneLineConstraintSketchTool(Sketch& s) : SingleSelectionEditTool(s) { Filter = SketchSelectionFilter::SSF_LINE; }

    bool LeftDown(const wxMouseEventEx& evt)
    {
        if (!dynamic_cast<SketchLine*>(highlight))
            return false;

        sketch.TryAddConstraint( std::make_unique<T>(currentLine) );
        return true;
    }
};

using HorizontalSketchTool = OneLineConstraintSketchTool<HorizontalConstraint>;
using VerticalSketchTool = OneLineConstraintSketchTool<VerticalConstraint>;

class CoincidentSketchTool : public ConsecutiveSelectionSketchTool
{
public:
    using ConsecutiveSelectionSketchTool::ConsecutiveSelectionSketchTool;

    bool OnPointPoint(SketchPointList::iterator&, SketchPointList::iterator&);
    bool OnPointLine(SketchPointList::iterator&, SketchLineList::iterator&);
    bool OnPointCircle(SketchPointList::iterator&, SketchCircleList::iterator&);
};

class ColinearSketchTool : public ConsecutiveSelectionSketchTool
{
public:
    inline ColinearSketchTool(Sketch& s) : ConsecutiveSelectionSketchTool(s) { Filter = SketchSelectionFilter::SSF_LINE; }
    bool OnLineLine(SketchLineList::iterator&, SketchLineList::iterator&);
};

class ConcentricSketchTool : public ConsecutiveSelectionSketchTool
{
public:
    inline ConcentricSketchTool(Sketch& s) : ConsecutiveSelectionSketchTool(s) { Filter = SketchSelectionFilter::SSF_CIRCLE; }
    bool OnCircleCircle(SketchCircleList::iterator&, SketchCircleList::iterator&);
};

class TangentSketchTool : public ConsecutiveSelectionSketchTool
{
public:
    inline TangentSketchTool(Sketch& s) : ConsecutiveSelectionSketchTool(s) { Filter = SketchSelectionFilter::SSF_LINE | SketchSelectionFilter::SSF_CIRCLE; }
    bool OnLineCircle(SketchLineList::iterator&, SketchCircleList::iterator&);
    bool OnCircleCircle(SketchCircleList::iterator&, SketchCircleList::iterator&);
};

class EqualSketchTool : public ConsecutiveSelectionSketchTool
{
public:
    inline EqualSketchTool(Sketch& s) : ConsecutiveSelectionSketchTool(s) { Filter = SketchSelectionFilter::SSF_LINE | SketchSelectionFilter::SSF_CIRCLE; }
    bool OnLineLine(SketchLineList::iterator&, SketchLineList::iterator&);
    bool OnCircleCircle(SketchCircleList::iterator&, SketchCircleList::iterator&);
};

class ParallelSketchTool : public ConsecutiveSelectionSketchTool
{
public:
    inline ParallelSketchTool(Sketch& s) : ConsecutiveSelectionSketchTool(s) { Filter = SketchSelectionFilter::SSF_LINE; }
    bool OnLineLine(SketchLineList::iterator&, SketchLineList::iterator&);
};

class OrthogonalSketchTool : public ConsecutiveSelectionSketchTool
{
public:
    inline OrthogonalSketchTool(Sketch& s) : ConsecutiveSelectionSketchTool(s) { Filter = SketchSelectionFilter::SSF_LINE; }
    bool OnLineLine(SketchLineList::iterator&, SketchLineList::iterator&);
};

#endif // _SKETCH_TOOLS_H_
