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

#include "DimensionEditDialog.h"
#include <wx/artprov.h>

DimensionEditDialog::DimensionEditDialog(const wxString& text) :
    wxDialog(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize)
{
    // create text editor
    tc = new wxTextCtrl(this, wxID_ANY, text, wxPoint(0,0), wxDefaultSize, wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC));
    tc->Bind(wxEVT_TEXT_ENTER, &DimensionEditDialog::OnTextEnter, this);

    // create buttons
    wxButton *okButton = new wxButton(this, wxID_OK, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxBU_NOTEXT);
    okButton->SetBitmap(wxArtProvider::GetBitmap(wxART_TICK_MARK));

    wxButton *closeButton = new wxButton(this, wxID_CANCEL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxBU_NOTEXT);
    closeButton->SetBitmap(wxArtProvider::GetBitmap(wxART_CLOSE));

    // place controls
    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
    hbox->Add(tc, 1, wxEXPAND);
    hbox->Add(okButton, 0);
    hbox->Add(closeButton, 0);

    // style the window
    SetIcon(wxArtProvider::GetIcon(wxART_EDIT));
    SetSizer(hbox);
    Layout();
    hbox->Fit(this);
    Centre();

    tc->SelectAll();
}

void DimensionEditDialog::OnTextEnter(wxCommandEvent& evt)
{
    evt.Skip();
    EndModal(wxID_OK);
}

SketchDimension::Coord DimensionEditDialog::Read(const SketchDimension* dim)
{
    return (dim->GetType() & DIMENSION_ANGULAR) ? glm::degrees(dim->GetValue()) : dim->GetValue();
}

template<class T> static constexpr T HugeValue();
template<> constexpr float  HugeValue<float> () { return HUGE_VALF; }
template<> constexpr double HugeValue<double>() { return HUGE_VAL ; }

template<class T> static T strtonumber(const char*, char**);
//template<> float  strtonumber<float> (const char* in, char** out)  { return std::strtof(in, out); }
template<> double strtonumber<double>(const char* in, char** out)  { return std::strtod(in, out); }

bool DimensionEditDialog::Write(SketchDimension* dim, const char* value)
{
    char* str_end;
    const SketchDimension::Coord newval = strtonumber<SketchDimension::Coord>(value, &str_end);

    if (str_end == value)
        return false;

    if (newval >= HugeValue<SketchDimension::Coord>())
        return false;

    dim->DesiredValue = (dim->GetType() & DIMENSION_ANGULAR) ? glm::radians(newval) : newval;
    return true;
}

#include <wx/windowptr.h>
bool DimensionEditDialog::Show(SketchDimension* dim)
{
    if (!dim)
        return false;

    // angular values will be converted from radians to degrees
    const auto readValue = Read(dim);

    // create dialog
    wxWindowPtr<DimensionEditDialog> diag(new DimensionEditDialog( std::to_string( readValue ) ));
    if (diag->ShowModal() != wxID_OK)
        return false;

    return Write(dim, (const char*)diag->tc->GetValue().mbc_str());
}
