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

#include <public/eiface.h>

#include "core/globals.h"


namespace vspdotnet {
    namespace utils {

        static std::string game_directory;

        inline std::string GameDirectory()
        {
          if (game_directory.empty())
          {
            char* gamePath = new char[255];
            globals::engine->GetGameDir(gamePath, 255);
            game_directory = std::string(gamePath);
          }

          return game_directory;
        }

        inline std::string CSGODotNetDirectory()
        {
          return GameDirectory() + "/addons/csgodotnet";
        }

        inline std::string ConfigDirectory()
        {
          // /addons/csgodotnet/config
          return CSGODotNetDirectory() + "/config";
        }
    }
}
