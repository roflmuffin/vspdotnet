#pragma once
#include "dt_send.h"

namespace vspdotnet {
class SendTableSharedExt {
 public:
  static int FindOffset(SendTable* pTable, const char* name);
  static SendProp* FindDescription(SendTable* pTable, const char* name);
};
}  // namespace vspdotnet