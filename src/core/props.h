#pragma once

#include <public/dt_send.h>

namespace vspdotnet {

struct SendPropInfo {
  SendProp* sendprop;
  int actual_offset;
};

class SendTableSharedExt {
 public:
  static int FindOffset(SendTable* pTable, const char* name);
  static SendPropInfo* FindDescription(SendTable* pTable, const char* name);
};
}  // namespace vspdotnet