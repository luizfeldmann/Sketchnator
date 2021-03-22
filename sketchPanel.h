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

#ifndef _SKETCH_PANEL_H_
#define _SKETCH_PANEL_H_

#include <wx/panel.h>
#include "SketchTool.h"
#include "Document.h"

class SketchPanel : public wxPanel
{
private:
    std::unique_ptr<SketchTool> currentTool;

    struct SelectionDragBoxInfo
    {
        bool bEnable = false;
        SketchFeature::Vector2 a;
        SketchFeature::Vector2 b;
    } dragBox;

public:
    AppDocument document;

    SketchPanel(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxPanelNameStr);
    ~SketchPanel();


    template<class T, class...Args>
    T* Tool(Args... args)
    {
        EndTool();

        currentTool.reset( new T(this->document.sketch, args...) );
        if (currentTool)
            currentTool->Begin();

        this->SetFocus(); // otherwise, keyboard events wont be received before the panel is clicked
        this->Refresh();

        return dynamic_cast<T*>( currentTool.get() );
    }
    void EndTool();

    inline bool AcceptsFocus() const {
        return true;
    }

    inline void UpdateDragSelectionBoxCB(bool en, const SketchFeature::Vector2& a, const SketchFeature::Vector2& b)
    {
        dragBox.bEnable = en;
        dragBox.a = a;
        dragBox.b = b;
    }

    void ZoomPageExtents();

    void OnPaint(wxPaintEvent& evt);
    void OnLeftUp(wxMouseEvent& evt);
    void OnLeftDown(wxMouseEvent& evt);
    void OnRightDown(wxMouseEvent& evt);
    void OnLeftDouble(wxMouseEvent& evt);
    void OnKeyUp(wxKeyEvent& evt);
    void OnKeyDown(wxKeyEvent& evt);
    void OnMouseMove(wxMouseEvent& evt);
    void OnMouseWheel(wxMouseEvent& evt);

    DECLARE_EVENT_TABLE()
};


#endif // _SKETCH_PANEL_H_
