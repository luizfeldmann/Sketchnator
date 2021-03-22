#ifndef _OSIFONT_H_
#define _OSIFONT_H_

#include <wx/font.h>

class Osifont
{
public:
    static const char* GetPath();
    static const wxFont& GetFont();
    static const std::vector<uint16_t>& EnumerateCharacters(); // characters are returned in Unicode "code-point" format
};

#endif // _OSIFONT_H_
