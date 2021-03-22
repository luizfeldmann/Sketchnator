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

#include "dirUtil.h"
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <map>

using namespace std;

wxString DirUtil::getRelativePath(wxString subfolder)
{
    static wxString executablePath;

    if (executablePath == wxEmptyString)
        executablePath = wxStandardPaths::Get().GetExecutablePath();

    wxFileName tempName(executablePath);
    tempName.AppendDir(subfolder);
    tempName.SetFullName(wxEmptyString);

    return tempName.GetFullPath();
}

const wxString& DirUtil::ApplicationDirectory(enum ContentDirectory dir)
{
    static std::map<const enum ContentDirectory, const wxString> dirMap;

    if (dirMap.size() == 0)
    {
        dirMap = {
            {Dir_Icons, getRelativePath("res")},
            {Dir_Shaders, getRelativePath("shaders")},
            {Dir_Textures, getRelativePath("textures")},
            {Dir_Materials, getRelativePath("materials")}
        };
    }

    return dirMap.at(dir);
}

wxArrayString DirUtil::ApplicationFiles(enum ContentDirectory dir, const wxString& filespec, int flags)
{
    wxArrayString files;
    wxDir::GetAllFiles(ApplicationDirectory(dir), &files, filespec, flags);

    return files;
}
