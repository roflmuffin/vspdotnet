#ifndef __INTERFACES_H_
#define __INTERFACES_H_

#include "globals.h"

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

#endif // __INTERFACES_H_
