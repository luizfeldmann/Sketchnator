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

#include "TextAnnotationEditDialog.h"
#include "sketchnatorGUI.h"
#include "Osifont.h"
#include "SketchAnnotationText.h"

#include <codecvt>
#include <locale>
#include <algorithm>
// ===============================================================================================
// GUI
// ===============================================================================================

class AnnotationEditorFrameEx;
static AnnotationEditorFrameEx* singleton = NULL;
// there is (at most) one single instance of the editor at any time
// closing the editor does not destroy the window - it just hides it
// open the editor again will retrive the existing window - this saves processing in populating the characters list

class AnnotationEditorFrameEx : public annotationEditorFrame
{
protected:
    wxWindow* refresher;
    SketchTextAnnotation* annotation = NULL;

    inline void RefreshDisplay() {
        if (refresher) refresher->Refresh();
    }

    void bold_toggle( wxCommandEvent& event ) {
        if (!annotation)
            return;

        annotation->font.SetWeight( m_toggle_bold->GetValue() ? wxFontWeight::wxFONTWEIGHT_BOLD : wxFontWeight::wxFONTWEIGHT_NORMAL);
        m_textEdit->SetFont(annotation->font);
        RefreshDisplay();
    }

    void italic_toggle( wxCommandEvent& event ) {
         if (!annotation)
            return;

        annotation->font.SetStyle( m_toggle_italic->GetValue() ? wxFontStyle::wxFONTSTYLE_ITALIC : wxFontStyle::wxFONTSTYLE_NORMAL);
        m_textEdit->SetFont(annotation->font);
        RefreshDisplay();
    }

    void underline_toggle( wxCommandEvent& event ) {
        if (!annotation)
            return;

        annotation->font.SetUnderlined( m_toggle_underline->GetValue() );
        m_textEdit->SetFont(annotation->font);
        RefreshDisplay();
    }

    void color_changed( wxColourPickerEvent& event ) {
        if (!annotation)
            return;

        annotation->color = event.GetColour();
        m_textEdit->SetForegroundColour(annotation->color);
        m_textEdit->Refresh();
        RefreshDisplay();
    }

    void change_size( wxSpinEvent& event ) {
        if (!annotation)
            return;

        annotation->font.SetPointSize(event.GetValue());
        m_textEdit->SetFont(annotation->font);
        RefreshDisplay();
    }

    void symbol_insert( wxListEvent& event ) {
        m_textEdit->AppendText(event.GetLabel());
    }

    void text_changed( wxCommandEvent& event ) {
        if (!annotation)
            return;

        annotation->text = m_textEdit->GetValue();
        RefreshDisplay();
    }

    void text_enter( wxCommandEvent& event ) {
        Close(false);
    }

public:
    AnnotationEditorFrameEx(wxWindow* parent, wxWindow* refr) :
        annotationEditorFrame(parent),
        refresher(refr)
    {
        // register self
        singleton = this;

        // prevent updates while list is populated
        Freeze();
        m_listCtrl_Symbols->SetFont( Osifont::GetFont().Larger() );

        static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
        const auto& symbols = Osifont::EnumerateCharacters();

        long index = 0;
        for (uint16_t symbol : symbols)
            m_listCtrl_Symbols->InsertItem(index++,wxString::FromUTF8(convert.to_bytes(    symbol     ).data()));

        // re-enable updates
        Thaw();
    }

    bool Show(SketchTextAnnotation* select)
    {
        if (!select)
            return Hide();

        annotation = select;

        m_textEdit->SetFont(annotation->font);
        m_textEdit->SetForegroundColour(annotation->color);
        m_textEdit->SetValue(annotation->text);

        m_spinCtrl_font_size->SetValue(annotation->font.GetPointSize());
        m_colourPicker_font->SetColour(annotation->color);

        m_toggle_bold->SetValue( annotation->font.GetWeight() == wxFontWeight::wxFONTWEIGHT_BOLD );
        m_toggle_italic->SetValue( annotation->font.GetStyle() == wxFontStyle::wxFONTSTYLE_ITALIC);
        m_toggle_underline->SetValue( annotation->font.GetUnderlined() );

        const bool bResult = wxFrame::Show(true);
        SetFocus(); // focus the window
        m_textEdit->SetFocus(); // focus the text
        m_textEdit->SelectAll(); // select text

        return bResult;
    }

    bool Hide()
    {
        annotation = NULL;
        return wxFrame::Hide();
    }

    void OnClose(wxCloseEvent& evt) {
        if (evt.CanVeto())
        {
            Hide();
            evt.Veto();
            return;
        }

        Destroy();
    }

    ~AnnotationEditorFrameEx() {
        singleton = NULL; // unregister self
    }

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(AnnotationEditorFrameEx, wxFrame)
    EVT_CLOSE(AnnotationEditorFrameEx::OnClose)
END_EVENT_TABLE()

// ===============================================================================================
// GUI
// ===============================================================================================
TextAnnotationTool::Vector2 dragOffset;
static SketchTextAnnotation* hover_text_ptr = NULL;

TextAnnotationTool::TextAnnotationTool(Sketch& sketch, wxWindow* parent, wxWindow* display) :
    SketchTool(sketch)
{
    if (!singleton)
        singleton = new AnnotationEditorFrameEx( parent, display ); // parent must be the main window - not the sketch panel
}

void TextAnnotationTool::End()
{
    if (singleton)
        singleton->Hide();
}

bool TextAnnotationTool::LeftDown(const wxMouseEventEx& evt)
{
    if (hover_text_ptr)
    {
        Vector2 mouse;
        evt.GetPosition(&mouse.x, &mouse.y);
        dragOffset = hover_text_ptr->position - mouse;
    }

    if (singleton)
    {
        if (singleton->IsShown())
        {
            if (!hover_text_ptr)
                singleton->Hide();
            else
                singleton->Show( hover_text_ptr );
        }
    }

    return false;
}

bool TextAnnotationTool::LeftDouble(const wxMouseEventEx& evt)
{
    // never create new text if there is one under the mouse already
    if (hover_text_ptr && singleton)
    {
       singleton->Show(hover_text_ptr);
       return false;
    }

    // get mouse position
    Vector2 mouse;
    evt.GetPosition(&mouse.x, &mouse.y);

    // create new annotation and select it
    hover_text_ptr = new SketchTextAnnotation(mouse, "[....]");
    hover_text_ptr->bSelected = true;
    sketch.annotations.emplace_back( std::unique_ptr<SketchAnnotation>( hover_text_ptr ) );
    singleton->Show(hover_text_ptr);

    return true;
}

static SketchTextAnnotation* FindHover(const TextAnnotationTool::Vector2 mouse, SketchAnnotationList& list)
{
    SketchTextAnnotation* found = NULL;
    TextAnnotationTool::Coord min_dist = std::numeric_limits<TextAnnotationTool::Coord>::max();

    for (auto it = list.begin(); it != list.end(); ++it)
    {
        SketchTextAnnotation* textann = dynamic_cast<SketchTextAnnotation*>(it->get());
        if (!textann)
            continue;

        TextAnnotationTool::Coord dist;
        if (!textann->HitBB(mouse, dist))
            continue;

        if (dist > min_dist)
            continue;

        min_dist = dist;
        found = textann;
    }

    return found;
}

bool TextAnnotationTool::MouseMove(const wxMouseEventEx& evt)
{
    // get mouse position
    Vector2 mouse;
    evt.GetPosition(&mouse.x, &mouse.y);

    if (evt.Dragging())
    {
        if(!hover_text_ptr)
            return false;

        hover_text_ptr->position = mouse + dragOffset;
    }
    else
    {
        // find annotation under mouse
        SketchTextAnnotation* const new_hover_text_ptr = FindHover(mouse, sketch.annotations);

        if (new_hover_text_ptr == hover_text_ptr)
            return false; // no changes

        // unselect old
        if (hover_text_ptr)
            hover_text_ptr->bSelected = false;

        // select new
        if (new_hover_text_ptr)
            new_hover_text_ptr->bSelected = true;

        hover_text_ptr = new_hover_text_ptr;
    }

    return true;
}

bool TextAnnotationTool::KeyDown(wxKeyEvent& evt)
{
    if(!hover_text_ptr)
        return false;

    if (evt.GetKeyCode() != WXK_DELETE)
        return false;

    if (singleton)
        singleton->Hide();

    sketch.annotations.erase(std::remove_if(sketch.annotations.begin(), sketch.annotations.end(), [/*&hover_text_ptr*/](const std::unique_ptr<SketchAnnotation>& ann)->bool{
        return (ann.get() == hover_text_ptr);
    }));

    hover_text_ptr = NULL;

    return true;
}
