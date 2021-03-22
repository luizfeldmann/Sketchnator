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

#include "ToggleButtonGroup.h"

ToggleButtonGroup::ToggleButtonGroup(const std::initializer_list<wxToggleButton*>& buttons)
{
    Add(buttons);
}

ToggleButtonGroup::~ToggleButtonGroup()
{
    for (wxToggleButton* btn : group)
    {
        btn->Unbind(wxEVT_TOGGLEBUTTON, &ToggleButtonGroup::OnToggle, this, wxID_ANY);
        btn->Unbind(wxEVT_DESTROY, &ToggleButtonGroup::OnDestroy, this, wxID_ANY);
    }
}

void ToggleButtonGroup::Add(wxToggleButton* btn)
{
    btn->Bind(wxEVT_TOGGLEBUTTON, &ToggleButtonGroup::OnToggle, this, wxID_ANY);
    btn->Bind(wxEVT_DESTROY, &ToggleButtonGroup::OnDestroy, this, wxID_ANY);

    group.push_back(btn);
}

void ToggleButtonGroup::Add(const std::initializer_list<wxToggleButton*>& buttons)
{
    for (wxToggleButton* btn : buttons)
        Add(btn);
}

void ToggleButtonGroup::OnDestroy(wxWindowDestroyEvent& evt)
{
    evt.Skip();

    group.erase(std::remove(group.begin(), group.end(), dynamic_cast<wxToggleButton*>(evt.GetEventObject()) ));
}

void ToggleButtonGroup::OnToggle(wxCommandEvent& evt)
{
    evt.Skip();

    wxToggleButton* object = dynamic_cast<wxToggleButton*>(evt.GetEventObject());

    if (!object->GetValue())
        return; // button was unclicked

    // button was clicked - now unclick others
    for (wxToggleButton* btn : group)
        if (btn != object)
            btn->SetValue(false);
}

void ToggleButtonGroup::UnToggle()
{
    for (wxToggleButton* btn : group)
        btn->SetValue(false);
}

std::list<std::unique_ptr<ToggleButtonGroup>> ToggleButtonGroupManager::allGroups;

ToggleButtonGroup* ToggleButtonGroupManager::NewGroup()
{
    ToggleButtonGroup* g = new ToggleButtonGroup();
    allGroups.emplace_back( std::unique_ptr<ToggleButtonGroup>(g) );
    return g;
}

ToggleButtonGroup* ToggleButtonGroupManager::NewGroup(const std::initializer_list<wxToggleButton*>& buttons)
{
    ToggleButtonGroup* g = new ToggleButtonGroup(buttons);
    allGroups.emplace_back( std::unique_ptr<ToggleButtonGroup>(g) );
    return g;
}
