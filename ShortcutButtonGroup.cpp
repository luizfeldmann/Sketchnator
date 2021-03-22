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

#include "ShortcutButtonGroup.h"
#include <iostream>
#include <wx/tglbtn.h>

ShortcutButtonGroup::~ShortcutButtonGroup()
{
    // when we're destroyed, we must remove all bindings to avoid having wxControls calling our invalid pointer
    for (const auto& pair : list)
        pair.second->Unbind(wxEVT_DESTROY, &ShortcutButtonGroup::OnDestroyButton, this, wxID_ANY);

    if (parent)
    {
        parent->Unbind(wxEVT_DESTROY, &ShortcutButtonGroup::OnDestroyParent, this, wxID_ANY); // destroyed parents must be unreferenced from our variable
        parent->Unbind(wxEVT_KEY_DOWN, &ShortcutButtonGroup::OnKeyDown, this, wxID_ANY); // destroyed parents must be unreferenced from our variable
    }
}

ShortcutButtonGroup::ShortcutButtonGroup(wxWindow* p) :
    parent(p)
{
    if (parent)
        RegisterParent();
}

ShortcutButtonGroup::ShortcutButtonGroup(wxWindow* p, const std::initializer_list<std::pair<int, wxAnyButton*>>& init) :
    parent(p)
{
    if (parent)
        RegisterParent();

    Add(init);
}

void ShortcutButtonGroup::RegisterParent()
{
    parent->Bind(wxEVT_DESTROY, &ShortcutButtonGroup::OnDestroyParent, this, wxID_ANY); // destroyed parents must be unreferenced from our variable
    parent->Bind(wxEVT_KEY_DOWN, &ShortcutButtonGroup::OnKeyDown, this, wxID_ANY); // destroyed parents must be unreferenced from our variable
}

void ShortcutButtonGroup::Add(int shortcut, wxAnyButton* button)
{
    auto result = list.emplace( std::pair<int, wxAnyButton*>(shortcut, button) );

    if (!result.second)
        return; // insertion failed

    button->Bind(wxEVT_DESTROY, &ShortcutButtonGroup::OnDestroyButton, this, wxID_ANY); // destroyed buttons must be removed from list
}

void ShortcutButtonGroup::Add(const std::initializer_list<std::pair<int, wxAnyButton*>>& init)
{
    for (const auto& pair : init)
        Add(pair.first, pair.second);
}

void ShortcutButtonGroup::OnDestroyButton(wxWindowDestroyEvent& evt)
{
    evt.Skip();

    wxAnyButton* btn = dynamic_cast<wxAnyButton*>(evt.GetEventObject());
    if (!btn)
        return;

    // destroyed buttons must be removed from list
    list.erase(std::find_if(list.begin(), list.end(), [&btn](const std::pair<int, wxAnyButton*>& entry)->bool
    {
        return (entry.second == btn);
    }));
}

void ShortcutButtonGroup::OnDestroyParent(wxWindowDestroyEvent& evt)
{
    evt.Skip();
    parent = NULL;
}

void ShortcutButtonGroup::OnKeyDown(wxKeyEvent& evt)
{
    evt.Skip();

    decltype(list)::iterator found = list.find(evt.GetKeyCode());
    if (found == list.end())
        return;

    wxAnyButton *btn = found->second;
    if (!btn)
        return;

    wxToggleButton* tglbtn = dynamic_cast<wxToggleButton*>(btn);
    if (tglbtn)
    {
        tglbtn->SetValue(true);
        wxCommandEvent click(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, tglbtn->GetId());
        click.SetEventObject(tglbtn);
        tglbtn->ProcessWindowEvent(click);
    }
    else
    {
        wxCommandEvent click(wxEVT_COMMAND_BUTTON_CLICKED, btn->GetId());
        click.SetEventObject(btn);
        btn->ProcessWindowEvent(click);
    }
}
