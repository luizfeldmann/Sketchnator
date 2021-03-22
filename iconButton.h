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

#ifndef _ICON_BUTTON_H_
#define _ICON_BUTTON_H_

#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/tglbtn.h>

class IconButton : public wxBitmapButton
{
public:
    IconButton(	wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& label = wxEmptyString, const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxButtonNameStr );
};

class ToggleIconButton : public wxBitmapToggleButton
{
public:
    ToggleIconButton(	wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& label = wxEmptyString, const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxButtonNameStr );

    void OnToggle(wxCommandEvent& evt);

DECLARE_EVENT_TABLE()
};

#endif // _ICON_BUTTON_H_
