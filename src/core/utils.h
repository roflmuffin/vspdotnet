#pragma once

#include <cstdint>

#include "eiface.h"
#include "globals.h"


namespace vspdotnet {
    namespace utils {
        inline std::string GameDirectory()
        {
          char* gamePath = new char[255];
          vspdotnet::globals::engine->GetGameDir(gamePath, 255);

          return std::string(gamePath);
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