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

#ifndef _TOGGLE_BUTTON_GROUP_
#define _TOGGLE_BUTTON_GROUP_

#include <list>
#include <wx/tglbtn.h>
#include <memory>

class ToggleButtonGroup
{
private:
    ToggleButtonGroup(const ToggleButtonGroup&) = delete; // non copyable
    ToggleButtonGroup& operator=(const ToggleButtonGroup&) = delete; // non assignable

protected:
    std::list<wxToggleButton*> group;

public:
    ToggleButtonGroup() = default;
    ToggleButtonGroup(const std::initializer_list<wxToggleButton*>& buttons);

    ~ToggleButtonGroup();

    void Add(wxToggleButton* btn);
    void Add(const std::initializer_list<wxToggleButton*>& buttons);
    void UnToggle();

    void OnToggle(wxCommandEvent& evt);
    void OnDestroy(wxWindowDestroyEvent& evt);
};

class ToggleButtonGroupManager
{
protected:
    static std::list<std::unique_ptr<ToggleButtonGroup>> allGroups;

public:
    static ToggleButtonGroup* NewGroup();
    static ToggleButtonGroup* NewGroup(const std::initializer_list<wxToggleButton*>& buttons);
};

#endif // _TOGGLE_BUTTON_GROUP_
