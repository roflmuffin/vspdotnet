#pragma once

#include "globals.h"
#include "tier1/interface.h"

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