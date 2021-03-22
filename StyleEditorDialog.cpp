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

#include "StyleEditorDialog.h"
#include "sketchnatorGUI.h"
#include <wx/dcbuffer.h>

static bool diagOpen = false;

class StyleEditorDialogEx : public styleEditorDialog
{
protected:
    wxPenEx* currentPen = NULL;

    void synchronize_controls()
    {
        m_panel_style_preview->Refresh();

        if (!currentPen)
            return;

        m_spinCtrlDouble_width->SetValue( currentPen->GetWidthDouble() );
        m_lineColourPicker->SetColour( currentPen->GetColour() );

        // combo box with line styles
        const wxPenStyle seachStyle = currentPen->GetStyle();
        for (unsigned int i = 0; i < m_choice_line_type->GetCount(); i++) {
            if (  (wxPenStyle)(uintptr_t)m_choice_line_type->GetClientData(i) == seachStyle) {
                m_choice_line_type->SetSelection(i);
                return;
            }
        }

        // not found - select none
        m_choice_line_type->SetSelection(-1);
    }

    void on_select_item( wxListEvent& event )
    {
        DrawingStyle::SketchLineStyle* style = reinterpret_cast<DrawingStyle::SketchLineStyle*>(event.GetData());
        if (!style) // sanity check
            return;

        currentPen = m_notebook_state->GetSelection() ? &style->selected : &style->normal;
        synchronize_controls();
    }

    void on_changed_page( wxNotebookEvent& event )
    {
        // get selected type of line on the left list
        const long selcat = m_listCtrl_styles->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (selcat <= -1)
            return; // no category is selected

        // get the pointer
        DrawingStyle::SketchLineStyle* style = reinterpret_cast<DrawingStyle::SketchLineStyle*>( m_listCtrl_styles->GetItemData(selcat) );
        if (!style) // sanity check
            return;

        // choose which version based on tab
        currentPen = m_notebook_state->GetSelection() ? &style->selected : &style->normal;
        synchronize_controls();
    }

    inline void RefreshStyle()
    {
        // refresh preview
        m_panel_style_preview->Refresh();

        // refresh parent
        wxWindow* parent;
        if ((parent = wxWindow::GetParent()) != NULL)
            parent->Refresh();
    }

    void on_width_change( wxSpinDoubleEvent& event )
    {
        if (!currentPen)
            return;

        currentPen->SetWidth( event.GetValue() );
        RefreshStyle();
    }

    void on_color_change( wxColourPickerEvent& event )
    {
        if (!currentPen)
            return;

        currentPen->SetColour( event.GetColour() );
        RefreshStyle();
    }

    void on_dash_change(  wxCommandEvent& event  )
    {
        if (!currentPen)
            return;

        int selection;
        if ((selection = m_choice_line_type->GetSelection()) < 0)
            return;

        currentPen->SetStyle( (wxPenStyle)(uintptr_t)m_choice_line_type->GetClientData(selection) );
        RefreshStyle();
    }

    void on_paint_preview(wxPaintEvent& evt) {
        wxPaintDC dc(m_panel_style_preview);

        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();

        if(!currentPen)
            return;

        int w, h;
        dc.GetSize(&w, &h);

        dc.SetPen( *currentPen );
        dc.DrawLine(0, h/2, w, h/2);
    }

    public:
    StyleEditorDialogEx(wxWindow* parent, DrawingStyle& style) :
        styleEditorDialog(parent)
    {
        // make sure there is only on of these
        if (diagOpen)
            throw;
        else
            diagOpen = true;

        // connect panel paint event
        m_panel_style_preview->Bind(wxEVT_PAINT, &on_paint_preview, this);

        // populate list of line styles
        long index = 0;
        m_listCtrl_styles->SetItemPtrData(m_listCtrl_styles->InsertItem(index++, "Normal"), (wxUIntPtr)&style.normal);
        m_listCtrl_styles->SetItemPtrData(m_listCtrl_styles->InsertItem(index++, "Construction"), (wxUIntPtr)&style.construction);
        m_listCtrl_styles->SetItemPtrData(m_listCtrl_styles->InsertItem(index++, "Centerline"), (wxUIntPtr)&style.center);
        m_listCtrl_styles->SetItemPtrData(m_listCtrl_styles->InsertItem(index++, "Dimension"), (wxUIntPtr)&style.dimension);

        // populate list of dashes
        m_choice_line_type->Append("_______", (void*)wxPenStyle::wxPENSTYLE_SOLID);
        m_choice_line_type->Append("-------", (void*)wxPenStyle::wxPENSTYLE_SHORT_DASH);
        m_choice_line_type->Append("- - - -", (void*)wxPenStyle::wxPENSTYLE_LONG_DASH);
        m_choice_line_type->Append("-+-+-+-", (void*)wxPenStyle::wxPENSTYLE_DOT_DASH);

        // select first
        m_listCtrl_styles->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    }

    void OnClose(wxCloseEvent& evt) {
        Destroy(); // this window must be destroyed explicitly; wxWidgets may delay deletion, which will prevent user from creating a new window
    }

    ~StyleEditorDialogEx() {
        m_panel_style_preview->Unbind(wxEVT_PAINT, &on_paint_preview, this);

        // allow user to create a new window now...
        diagOpen = false;
    }

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(StyleEditorDialogEx, styleEditorDialog)
    EVT_CLOSE(StyleEditorDialogEx::OnClose)
END_EVENT_TABLE()

void StyleEditor::Show(wxWindow* parent, DrawingStyle& ds)
{
    if (diagOpen)
        return;

    auto d = new StyleEditorDialogEx(parent, ds);
    d->Show();
}
