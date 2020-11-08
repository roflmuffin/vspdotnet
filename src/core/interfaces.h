/*
 *  This file is part of VSP.NET.
 *  VSP.NET is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VSP.NET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with VSP.NET.  If not, see <https://www.gnu.org/licenses/>. *
 */

#pragma once

#include <public/tier1/interface.h>

namespace vspdotnet {
  namespace interfaces {
    struct InterfaceHelper_t {
      const char *interface;
      void **global;
    };

    bool InitializeInterfaces(InterfaceHelper_t* interface_list, CreateInterfaceFn factory);
    bool InitializeEngineInterfaces(CreateInterfaceFn factory);
    bool InitializeGameInterfaces(CreateInterfaceFn factory);
  };
}