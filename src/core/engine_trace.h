#pragma once

#include "core/callback_manager.h"
#include "public/engine/IEngineTrace.h"
#include "public/gametrace.h"

namespace vspdotnet {
class TraceFilterProxy : public ITraceFilter {
 public:
  TraceFilterProxy() {}
  bool ShouldHitEntity(IHandleEntity* pServerEntity, int contentsMask);
  TraceType_t GetTraceType() const;

  void SetShouldHitEntityCallback(CallbackT cb);
  void SetGetTraceTypeCallback(CallbackT cb);

 private:
  CallbackT m_cb_should_hit_entity;
  CallbackT m_cb_get_trace_type;
};

class CSimpleTraceFilter : public ITraceFilter {
 public:
  CSimpleTraceFilter(int index) : m_index_to_exclude(index) {}
  bool ShouldHitEntity(IHandleEntity* pServerEntity, int contentsMask);

  TraceType_t GetTraceType() const { return TRACE_EVERYTHING; }

 private:
  int m_index_to_exclude = -1;
};

enum RayType { RayType_EndPoint, RayType_Infinite };

}  // namespace vspdotnet