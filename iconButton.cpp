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

#include "iconButton.h"
#include <wx/dcbuffer.h>
#include "iconLoader.h"

IconButton::IconButton(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name) :
    wxBitmapButton(parent, id, IconLoader::GetBitmap(name, IconSize::ICON_SIZE_MEDIUM), pos, size, style | wxBU_BOTTOM | wxBU_AUTODRAW, validator, wxEmptyString)
{
    SetLabel(label);
    SetToolTip(label);
}


ToggleIconButton::ToggleIconButton(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name) :
    wxBitmapToggleButton(parent, id, IconLoader::GetBitmap(name, IconSize::ICON_SIZE_MEDIUM), pos, size, style | wxBU_BOTTOM | wxBU_AUTODRAW, validator, wxEmptyString)
{
    SetLabel(label);
    SetToolTip(label);
}

// RELAY TOGGLE EVENTS THE SAME AS CLICK EVENTS...
void ToggleIconButton::OnToggle(wxCommandEvent& evt)
{
    wxCommandEvent click(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    click.SetEventObject(evt.GetEventObject());
    ProcessWindowEvent(click);
}

BEGIN_EVENT_TABLE(ToggleIconButton, wxBitmapToggleButton)
EVT_TOGGLEBUTTON(wxID_ANY, ToggleIconButton::OnToggle)
END_EVENT_TABLE()
