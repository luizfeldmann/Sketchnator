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

#ifndef _EXT_STRING_H_
#define _EXT_STRING_H_

#include "Serializable.h"
#include <wx/string.h>
#include <glm/vec2.hpp>
#include <sstream>

inline bool operator>>(const SerializationInValue& in, wxString& data)
{
    if (!in.IsString())
        return false;

    data = wxString::FromUTF8(in.GetString());
    return true;
}

inline bool operator<<(SerializationOut& out, const wxString& data)
{
    const auto& utf8 = data.ToUTF8();
    return out.String(utf8.data(), utf8.length());
}

#endif // _EXT_STRING_H_
