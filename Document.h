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

#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

#include "DrawingLayout.h"
#include "sketch.h"

/// ENCAPSULATES ALL "PROJECT DATA"
/// all the data saved and loaded from project files
struct AppDocument {
    Sketch sketch;
    DrawingLayout layout;
};

DECLARE_SERIALIZATION_SCHEME(AppDocument);

#endif // _DOCUMENT_H_
