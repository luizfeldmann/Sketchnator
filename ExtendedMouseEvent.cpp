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

#include "ExtendedMouseEvent.h"

wxMouseEventEx::wxMouseEventEx(const wxMouseEvent& src) :
    wxMouseEvent(src) // use default copy constructor
{
    X = src.GetX();
    Y = src.GetY();
}

void wxMouseEventEx::GetPosition(double& x, double& y) const {
    x = X;
    y = Y;
}

void wxMouseEventEx::GetPosition(double* x, double* y) const {
    if (x) *x = X;
    if (y) *y = Y;
}

double wxMouseEventEx::GetX() const {
    return X;
}

double wxMouseEventEx::GetY() const {
    return Y;
}

void wxMouseEventEx::SetPosition(double x, double y) {
    X = x;
    Y = y;
}

void wxMouseEventEx::SetX(double x) {
    X = x;
}

void wxMouseEventEx::SetY(double y) {
    Y = y;
}
