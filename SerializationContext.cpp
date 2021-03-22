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

#include "SerializationContext.h"

static std::unique_ptr<SerializationContext> singleton;

SerializationContext::SerializationContext()
{
    if (singleton)
        throw std::runtime_error("Construction of a SerializationContext while the previous instance still exists");

    singleton.reset(this);
}

SerializationContext::~SerializationContext()
{
    singleton.release(); // this is being deleted, so remove the reference to avoid: 1) dangling pointers and 2) deleting and object that has already been deleted
}

SerializationContext& SerializationContext::Get()
{
    if (singleton)
        return *singleton;

    throw std::runtime_error("An instance of SerializationContext has been requested, but none exists");
}
