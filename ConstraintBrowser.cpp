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

#include "ConstraintBrowser.h"

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <type_traits>

#include "iconLoader.h"
#include "DimensionEditDialog.h"

// ====================================================================================================
// IMAGE LIST
// ====================================================================================================
static const IconSize icon_list_size = IconSize::ICON_SIZE_MEDIUM;
static wxImageList icon_list(icon_list_size, icon_list_size);
static int icon_list_add(const wxString& name) {
    return icon_list.Add(IconLoader::GetIcon(name, icon_list_size));
}

// ====================================================================================================
// GUI SPECIFIC DATA FOR EACH CONSTRAINT
// ====================================================================================================
struct SketchConstraintGuiData {
    int IconId;                       // the index of the icon for this type of constraint stored in icon_list
    const char* label;                // label at bottom of icon
    std::function<void(bool)> select; // a function that selects or deselects the features related to this constraint
};

// ====================================================================================================
// SPECIFIC IMPLEMENTATION FOR EACH TYPE OF CONSTRAINT
// ====================================================================================================
static void SelectConstraint2p(const TwoPointConstraint* constraint, bool bSelect)
{
    constraint->first->bSelected = bSelect;
    constraint->second->bSelected = bSelect;
}

static void SelectConstraint1l(const OneLineConstraint* constraint, bool bSelect)
{
    constraint->line->bSelected = bSelect;
}

static void SelectConstraint2l(const TwoLineConstraint* constraint, bool bSelect)
{
    constraint->first->bSelected = bSelect;
    constraint->second->bSelected = bSelect;
}

static void SelectConstraint2c(const TwoCircleConstraint* constraint, bool bSelect)
{
    constraint->first->bSelected = bSelect;
    constraint->second->bSelected = bSelect;
}

static void SelectConstraintDim(SketchDimension* constraint, bool bSelect)
{
    constraint->bSelected = bSelect;
}

std::unique_ptr<SketchConstraintGuiData> SketchConstraint::GetGuiData() {
    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        0,
        NULL,
        NULL
    });
}

std::unique_ptr<SketchConstraintGuiData> HorizontalConstraint::GetGuiData() {
    const static int IconId = icon_list_add("constraint_horizontal");
    const static char* label = "Horizontal";

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        IconId,
        label,
        std::bind(SelectConstraint1l, this, std::placeholders::_1)
    });
}

std::unique_ptr<SketchConstraintGuiData> VerticalConstraint::GetGuiData() {
    const static int IconId = icon_list_add("constraint_vertical");
    const static char* label = "Vertical";

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        IconId,
        label,
        std::bind(SelectConstraint1l, this, std::placeholders::_1)
    });
}

std::unique_ptr<SketchConstraintGuiData> ParallelConstraint::GetGuiData() {
    const static int IconId = icon_list_add("constraint_parallel");
    const static char* label = "Parallel";

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        IconId,
        label,
        std::bind(SelectConstraint2l, this, std::placeholders::_1)
    });
}

std::unique_ptr<SketchConstraintGuiData> OrthogonalConstraint::GetGuiData() {
    const static int IconId = icon_list_add("constraint_orthogonal");
    const static char* label = "Perpendicular";

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        IconId,
        label,
        std::bind(SelectConstraint2l, this, std::placeholders::_1)
    });
}

std::unique_ptr<SketchConstraintGuiData> EqualLengthConstraint::GetGuiData() {
    const static int IconId = icon_list_add("constraint_equal");
    const static char* label = "Equal length";

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        IconId,
        label,
        std::bind(SelectConstraint2l, this, std::placeholders::_1)
    });
}

std::unique_ptr<SketchConstraintGuiData> EqualRadiusConstraint::GetGuiData() {
    const static int IconId = icon_list_add("constraint_equal_circle");
    const static char* label = "Equal radius";

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        IconId,
        label,
        std::bind(SelectConstraint2c, this, std::placeholders::_1)
    });
}

std::unique_ptr<SketchConstraintGuiData> TangentCircleConstraint::GetGuiData() {
    const static int IconId = icon_list_add("constraint_tangent_circle");
    const static char* label = "Tangent circles";

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        IconId,
        label,
        std::bind(SelectConstraint2c, this, std::placeholders::_1)
    });
}

std::unique_ptr<SketchConstraintGuiData> PointOnPointAxisConstraint::GetGuiData() {
    const static int IconId_x = icon_list_add("constraint_coincident_x");
    const static char* labelx = "Same X";

    const static int IconId_y = icon_list_add("constraint_coincident_y");
    const static char* labely = "Same Y";

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        (bX ? IconId_x : IconId_y),
        (bX ? labelx : labely),
        std::bind(SelectConstraint2p, this, std::placeholders::_1)
    });
}

std::unique_ptr<SketchConstraintGuiData> TangentLineConstraint::GetGuiData() {
    const static int IconId = icon_list_add("constraint_tangent");
    const static char* label = "Tangent line";

    static auto selector = [](TangentLineConstraint* tlc, bool b)->void{
        tlc->line->bSelected = b;
        tlc->circle->bSelected = b;
    };

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        IconId,
        label,
        std::bind(selector, this, std::placeholders::_1)
    });
}

std::unique_ptr<SketchConstraintGuiData> PointOnLineConstraint::GetGuiData() {
    const static int IconId = icon_list_add("snap_on_line");
    const static char* label = "Point on line";

    static auto selector = [](PointOnLineConstraint* pl, bool b)->void{
        pl->line->bSelected = b;
        pl->point->bSelected = b;
    };

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        IconId,
        label,
        std::bind(selector, this, std::placeholders::_1)
    });
}

std::unique_ptr<SketchConstraintGuiData> PointOnLineMidpoint::GetGuiData(){
    const static int IconId = icon_list_add("snap_line_midpoint");
    const static char* label = "On midpoint";

    static auto selector = [](PointOnLineMidpoint* pl, bool b)->void{
        pl->line->bSelected = b;
        pl->point->bSelected = b;
    };

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        IconId,
        label,
        std::bind(selector, this, std::placeholders::_1)
    });
}

std::unique_ptr<SketchConstraintGuiData> PointOnCircumferenceConstraint::GetGuiData() {
    const static int IconId = icon_list_add("snap_circumference");
    const static char* label = "On arc";

    static auto selector = [](PointOnCircumferenceConstraint* pc, bool b)->void{
        pc->point->bSelected = b;
        pc->circle->bSelected = b;
    };

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        IconId,
        label,
        std::bind(selector, this, std::placeholders::_1)
    });
}

static const char* GetDimensionLabel(const SketchDimension* dim)
{
    static char label[32];
    snprintf(label, sizeof(label), "%f", DimensionEditDialog::Read(dim));
    return label;
}

std::unique_ptr<SketchConstraintGuiData> SketchDimensionLinear::GetGuiData() {
    const static int IconId = icon_list_add("dimension");

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        IconId,
        GetDimensionLabel(this),
        std::bind(SelectConstraintDim, this, std::placeholders::_1)
    });
}

std::unique_ptr<SketchConstraintGuiData> SketchDimensionCircular::GetGuiData() {
    const static int IconId = icon_list_add("dimension_radial");

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        IconId,
        GetDimensionLabel(this),
        std::bind(SelectConstraintDim, this, std::placeholders::_1)
    });
}

std::unique_ptr<SketchConstraintGuiData> SketchDimensionAngular::GetGuiData() {
    const static int IconId = icon_list_add("dimension_angular");

    return std::unique_ptr<SketchConstraintGuiData>(new SketchConstraintGuiData{
        IconId,
        GetDimensionLabel(this),
        std::bind(SelectConstraintDim, this, std::placeholders::_1)
    });
}

// GUI
// ===========================
class ConstraintBrowserGUI;
static std::unique_ptr<ConstraintBrowserGUI> singleton;

class ConstraintBrowserGUI : public wxFrame
{
protected:
    wxListCtrl* m_list;
    Sketch& sketch;
    const std::function<void()> refreshCb;

    inline void RefreshSketch() const {
        if (refreshCb)
            refreshCb();
    };

    void OnItemChange(wxListEvent& evt, bool bSelect)
    {
        evt.Skip();
        SketchConstraint* sc = dynamic_cast<SketchConstraint*>(reinterpret_cast<SketchConstraint*>(evt.GetData()));
        if (!sc)
            return;

        auto selectCb = sc->GetGuiData()->select;
        if (!selectCb)
            return;

        selectCb(bSelect);

        if (refreshCb)
            refreshCb();
    }

    void OnKeyDown(wxListEvent& evt)
    {
        evt.Skip();
        //evt.ResumePropagation(INT_MAX);

        if (evt.GetKeyCode() != WXK_DELETE)
            return;

        SketchConstraint* sc = dynamic_cast<SketchConstraint*>(reinterpret_cast<SketchConstraint*>(evt.GetData()));
        if (!sc)
            return;

        // unselect the items from deleted constraint
        auto selectCb = sc->GetGuiData()->select;
        if (selectCb)
            selectCb(false);

        // erase the constraint
        sketch.constraints.erase(std::find_if(sketch.constraints.begin(), sketch.constraints.end(), [&sc](const std::unique_ptr<SketchConstraint>& test)->bool {
            return test.get() == sc;
        }));

        // make sure the item loses the reference to deleted object
        m_list->SetItemPtrData(evt.GetIndex(), wxUIntPtr(0) );
        m_list->DeleteItem(evt.GetIndex());

        if (refreshCb)
            refreshCb();
    }

    void OnActivateItem(wxListEvent& evt)
    {
        evt.Skip();
        m_list->EditLabel(evt.GetIndex());
    }

    void OnBeginEditItem(wxListEvent& evt)
    {
        SketchDimension* sc = dynamic_cast<SketchDimension*>(reinterpret_cast<SketchConstraint*>(evt.GetData()));
        if (!sc)
            evt.Veto();
        else
            evt.Skip();
    }

    void OnFinishEditItem(wxListEvent& evt)
    {
        // sanity check
        SketchDimension* sc = dynamic_cast<SketchDimension*>(reinterpret_cast<SketchConstraint*>(evt.GetData()));
        if (!sc)
            evt.Veto();

        // save the previous value for restoration in case value is not possible
        const auto saved = sc->DesiredValue;

        if (!DimensionEditDialog::Write(sc, evt.GetLabel().mbc_str()))
            evt.Veto();

        if (!sketch.Solve())
        {
            // failed - roll back
            sc->DesiredValue = saved;
            evt.Veto();
            return;
        }

        // accept
        evt.Skip();
        if (refreshCb)
            refreshCb();
    }

    void OnItemSelect(wxListEvent& evt)  { OnItemChange(evt, true); }
    void OnItemDeselect(wxListEvent& evt){ OnItemChange(evt, false); }

public:
    ConstraintBrowserGUI(Sketch& s, std::function<void()> refcb, wxWindow* parent = wxTheApp->GetTopWindow(), const wxSize& size = wxSize( 400,500 )) :
        wxFrame(
            parent, wxID_ANY, wxEmptyString,
            (!parent) ? wxDefaultPosition : parent->GetScreenPosition() + wxPoint(parent->GetSize().GetWidth() - size.GetWidth(), 0),
            size, wxSYSTEM_MENU | wxCAPTION | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR | wxTAB_TRAVERSAL
        ),
        sketch(s),
        refreshCb(refcb)
    {
        // preconf
        SetIcon(IconLoader::GetIcon("constraint_browse", IconSize::ICON_SIZE_SMALL));
        SetSizeHints( wxDefaultSize, wxDefaultSize );

        // create gui
        m_list = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_AUTOARRANGE | wxLC_ICON | wxLC_SINGLE_SEL | wxLC_EDIT_LABELS );
        m_list->SetImageList(&icon_list, wxIMAGE_LIST_NORMAL);
        m_list->Bind(wxEVT_LIST_ITEM_SELECTED, &OnItemSelect, this);
        m_list->Bind(wxEVT_LIST_ITEM_DESELECTED, &OnItemDeselect, this);
        m_list->Bind(wxEVT_LIST_KEY_DOWN, &OnKeyDown, this);
        m_list->Bind(wxEVT_LIST_ITEM_ACTIVATED, &OnActivateItem, this);
        m_list->Bind(wxEVT_LIST_BEGIN_LABEL_EDIT, &OnBeginEditItem, this);
        m_list->Bind(wxEVT_LIST_END_LABEL_EDIT, &OnFinishEditItem, this);

        unsigned int i = 0;
        for (std::unique_ptr<SketchConstraint>& ptr : sketch.constraints)
        {
            SketchConstraint* const sc = ptr.get();
            if (!sc)
                continue;

            auto data = sc->GetGuiData();
            int icon = data ? data->IconId : -1;
            const char* text = data->label;

            long item;
            if ((item = m_list->InsertItem(i++, text, icon)) >= 0)
                m_list->SetItemPtrData(item, reinterpret_cast<wxUIntPtr>(ptr.get()) );
        }

        wxBoxSizer* theSizer = new wxBoxSizer( wxVERTICAL );
        theSizer->Add( m_list, 1, wxALL|wxEXPAND, 5 );

        // postconf
        this->SetSizer( theSizer );
        this->Layout();
    }

    ~ConstraintBrowserGUI()
    {
        m_list->Unbind(wxEVT_LIST_ITEM_SELECTED, &OnItemSelect, this);
        m_list->Unbind(wxEVT_LIST_ITEM_DESELECTED, &OnItemDeselect, this);
        m_list->Unbind(wxEVT_LIST_KEY_DOWN, &OnKeyDown, this);
        m_list->Unbind(wxEVT_LIST_ITEM_ACTIVATED, &OnActivateItem, this);

        if(singleton.get() == this) // don't destroy twice
            singleton.release();
    }
};

// TOOL
// ===========================
ConstraintBrowser::ConstraintBrowser(Sketch& s, std::function<void()> refresh, wxWindow* parent) : SketchTool(s)
{
    singleton.reset( new ConstraintBrowserGUI(s, refresh, parent) );
}

void ConstraintBrowser::Begin()
{
    if (!singleton)
        return;

    singleton.get()->Show(true);
}

void ConstraintBrowser::End()
{
    singleton.reset(NULL);
}


