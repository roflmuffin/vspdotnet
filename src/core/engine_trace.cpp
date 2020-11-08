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

#include "core/engine_trace.h"

#include "core/utilities/conversions.h"
#include "core/log.h"

namespace vspdotnet {

CTraceFilterHitAll g_HitAllFilter;

bool CSimpleTraceFilter::ShouldHitEntity(IHandleEntity* pServerEntity,
                                         int contentsMask) {
  int index = ExcIndexFromBaseHandle(pServerEntity->GetRefEHandle());
  if (index == m_index_to_exclude) return false;

  return true;
}

TraceType_t TraceFilterProxy::GetTraceType() const {

  auto nativeContext = fxNativeContext{};
  auto scriptContext = ScriptContextRaw(nativeContext);

  m_cb_get_trace_type(&nativeContext);

  return scriptContext.GetResult<TraceType_t>();
}

bool TraceFilterProxy::ShouldHitEntity(IHandleEntity* pServerEntity,
                                       int contentsMask) {
  auto entity = ExcIndexFromBaseHandle(pServerEntity->GetRefEHandle());
  if (entity < 0) return false;

  auto nativeContext = fxNativeContext{};
  auto scriptContext = ScriptContextRaw(nativeContext);

  scriptContext.Push(entity);
  scriptContext.Push<int>(0);

  m_cb_should_hit_entity(&nativeContext);

  auto result = scriptContext.GetResult<bool>();

  VSPDN_CORE_INFO("Received result {0} from `ShouldHitEntity`", result);

  return result;
  /*return result;
  return true;*/
}

void TraceFilterProxy::SetShouldHitEntityCallback(CallbackT cb) {
  m_cb_should_hit_entity = cb;
}

void TraceFilterProxy::SetGetTraceTypeCallback(CallbackT cb) {
  m_cb_get_trace_type = cb;
}

/*
Ray_t* CreateRay1(RayType ray_type, const Vector& vec1, const Vector& vec2) {
  Ray_t* pRay = new Ray_t;

  if (ray_type == RayType_EndPoint) {
    pRay->Init(vec1, vec2);
    return pRay;
  } else if (ray_type == RayType_Infinite) {
    QAngle angles;
    Vector endVec;
    angles.Init(vec2.x, vec2.y, vec2.z);
    AngleVectors(angles, &endVec);

    endVec.NormalizeInPlace();
    endVec = vec1 + endVec * MAX_TRACE_LENGTH;

    pRay->Init(vec1, endVec);
    return pRay;
  }

  return nullptr;
}
*/

/*Ray_t* CreateRay2(const Vector& vec1, const Vector& vec2, const Vector& vec3,
                  const Vector& vec4) {
  Ray_t* pRay = new Ray_t;
  pRay->Init(vec1, vec2, vec3, vec4);
  return pRay;
}

void TraceRay(Ray_t* ray, CGameTrace* pTrace, ITraceFilter* trace_filter,
              unsigned int flags) {
  globals::engine_trace->TraceRay(*ray, flags, trace_filter, pTrace);
}*/

}  // namespace vspdotnet