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

#include "PolymorphicSerializableObject.h"

BEGIN_SERIALIZATION_SCHEME(PolymorphicSerializable)
    SERIALIZATION_METHOD(class_name)
END_SERIALIZATION_SCHEME()

std::map<std::string, std::function<PolymorphicSerializable*(void)>>& PolymorphicObjectFactory::factory_functions() {
    static std::map<std::string, std::function<PolymorphicSerializable*(void)>> the_map;
    return the_map;
}

PolymorphicSerializable* PolymorphicObjectFactory::Make(const std::string& class_name)
{
    auto found = factory_functions().find(class_name);
    if (found == factory_functions().cend())
        return NULL;

    if (!(found->second))
        return NULL;

    return found->second();
}
