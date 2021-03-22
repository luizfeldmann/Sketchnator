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

#ifndef _ALMOST_EQUAL_H_
#define _ALMOST_EQUAL_H_

#include <glm/glm.hpp>

template <typename T> inline T almost_equal_default_tolerance() { return 5*std::numeric_limits<T>::epsilon(); }

template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr> inline bool almost_equal(const T& a, const T& b, T tolerance = almost_equal_default_tolerance<T>())
{
    T delta = std::fabs(a - b);
    if (delta <= tolerance)
        return true;
    else if (delta < (std::fmax(std::fabs(a), std::fabs(b)) * tolerance) )
        return true;
    else
        return false;
}

inline bool almost_equal(const glm::vec2& a, const glm::vec2& b, float t = almost_equal_default_tolerance<float>())
{
    return (almost_equal(a.x, b.x, t) && almost_equal(a.y, b.y, t));
}

template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr> inline bool almost_zero(const T& a, T tolerance = almost_equal_default_tolerance<T>())
{
    return (std::fabs(a) <= tolerance);
}

inline bool almost_zero(const glm::vec2& a, float t = almost_equal_default_tolerance<float>()) {
    return (almost_zero(a.x, t) && almost_zero(a.y, t));
}

/// RANGE GUESSING
/// ====================================================================================

// MAX/MIN OF MULTIPLE VALUES

// the max/min of a single value is itself
template <typename T, typename... Ts> inline T rmax(const T& first) { return first; }
template <typename T, typename... Ts> inline T rmin(const T& first) { return first; }

// recursively compare pairs
template <typename T, typename... Ts> inline T rmax(const T& first, const Ts&... rest) { return std::fmax(first, rmax(rest...)); }
template <typename T, typename... Ts> inline T rmin(const T& first, const Ts&... rest) { return std::fmin(first, rmin(rest...)); }

// ABSOLUTE MAX/MIN OF MULTIPLE VALUES
template <typename T, typename... Ts> inline T rmaxabs(const T& first) { return std::fabs(first); }
template <typename T, typename... Ts> inline T rminabs(const T& first) { return std::fabs(first); }
template <typename T, typename... Ts> inline T rmaxabs(const T& first, const Ts&... rest) { return std::fmax(std::fabs(first), rmaxabs(rest...)); }
template <typename T, typename... Ts> inline T rminabs(const T& first, const Ts&... rest) { return std::fmin(std::fabs(first), rminabs(rest...)); }

template <typename T, typename... Ts> inline T relative_tolerance(const T& first, const Ts&... rest)
{
    return almost_equal_default_tolerance<T>() * rmaxabs(first, rest...);
}

#endif // _ALMOST_EQUAL_H_
