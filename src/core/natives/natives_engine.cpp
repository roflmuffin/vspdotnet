#include <edict.h>
#include <core/autonative.h>
#include <core/script_engine.h>

#include <eiface.h>
#include <IEngineSound.h>
#include <public/worldsize.h>

#include "core/engine_trace.h"
#include "core/user_message_manager.h"

namespace vspdotnet
{

const char* GetMapName(ScriptContext& script_context) {
  return globals::gpGlobals->mapname.ToCStr();
}

float GetTickInterval(ScriptContext& script_context) {
  return globals::gpGlobals->interval_per_tick;
}

float GetCurrentTime(ScriptContext& script_context) {
  return globals::gpGlobals->curtime;
}

int GetTickCount(ScriptContext& script_context) {
  return globals::gpGlobals->tickcount;
}

float GetGameFrameTime(ScriptContext& script_context) {
  return globals::gpGlobals->frametime;
}

double GetEngineTime(ScriptContext& script_context) { return Plat_FloatTime(); }

void ServerCommand(ScriptContext& script_context) {
  auto command = script_context.GetArgument<const char*>(0);

  auto clean_command = std::string(command);
  clean_command.append("\n\0");
  globals::engine->ServerCommand(clean_command.c_str());
}

void PrecacheModel(ScriptContext& script_context)
{
  auto name = script_context.GetArgument<const char*>(0);
  globals::engine->PrecacheModel(name);
}

bool PrecacheSound(ScriptContext& script_context) {
  auto name = script_context.GetArgument<const char*>(0);
  auto preload = script_context.GetArgument<bool>(1);

  return globals::engine_sound->PrecacheSound(name, preload);
}

bool IsSoundPrecached(ScriptContext& script_context) {
  auto name = script_context.GetArgument<const char*>(0);

  return globals::engine_sound->IsSoundPrecached(name);
}

float GetSoundDuration(ScriptContext& script_context) {
  auto name = script_context.GetArgument<const char*>(0);

  return globals::engine_sound->GetSoundDuration(name);
}

void EmitSound(ScriptContext& script_context)
{
  auto client = script_context.GetArgument<int>(0);
  auto entitySource = script_context.GetArgument<int>(1);
  auto channel = script_context.GetArgument<int>(2);
  auto sound = script_context.GetArgument<const char*>(3);
  auto volume = script_context.GetArgument<float>(4);
  auto attenuation = script_context.GetArgument <float>(5);
  auto flags = script_context.GetArgument<int>(6);
  auto pitch = script_context.GetArgument<int>(7);
  auto origin = script_context.GetArgument<Vector*>(8);
  auto direction = script_context.GetArgument<Vector*>(9);

  auto recipients = new CustomRecipientFilter();
  recipients->AddPlayer(client);

  globals::engine_sound->EmitSound(static_cast<IRecipientFilter&>(*recipients),
                                   entitySource,channel, sound, -1, sound, volume,
                         attenuation, 0, flags, pitch, origin, direction);
}

Ray_t* CreateRay1(ScriptContext& script_context)
{
  auto ray_type = script_context.GetArgument<RayType>(0);
  auto vec1 = script_context.GetArgument<Vector*>(1);
  auto vec2 = script_context.GetArgument<Vector*>(2);

  Ray_t* pRay = new Ray_t;

  if (ray_type == RayType_EndPoint) {
    pRay->Init(*vec1, *vec2);
    return pRay;
  } else if (ray_type == RayType_Infinite) {
    QAngle angles;
    Vector endVec;
    angles.Init(vec2->x, vec2->y, vec2->z);
    AngleVectors(angles, &endVec);

    endVec.NormalizeInPlace();
    endVec = *vec1 + endVec * MAX_TRACE_LENGTH;

    pRay->Init(*vec1, endVec);
    return pRay;
  }

  return nullptr;
}

Ray_t* CreateRay2(ScriptContext& script_context)
{
  auto vec1 = script_context.GetArgument<Vector*>(0);
  auto vec2 = script_context.GetArgument<Vector*>(1);
  auto vec3 = script_context.GetArgument<Vector*>(2);
  auto vec4 = script_context.GetArgument<Vector*>(3);

  Ray_t* pRay = new Ray_t;
  pRay->Init(*vec1, *vec2, *vec3, *vec4);
  return pRay;
}

void TraceRay(ScriptContext& script_context)
{
  auto ray = script_context.GetArgument<Ray_t*>(0);
  auto pTrace = script_context.GetArgument<CGameTrace*>(1);
  auto trace_filter = script_context.GetArgument<ITraceFilter*>(2);
  auto flags = script_context.GetArgument<uint32_t>(3);

  globals::engine_trace->TraceRay(*ray, flags, trace_filter, pTrace);
}

CSimpleTraceFilter* NewSimpleTraceFilter(ScriptContext& script_context)
{
  auto index_to_ignore = script_context.GetArgument<int>(0);

  return new CSimpleTraceFilter(index_to_ignore);
}

TraceFilterProxy* NewTraceFilterProxy(ScriptContext& script_context) {
  return new TraceFilterProxy();
}

void TraceFilterProxySetTraceTypeCallback(ScriptContext& script_context)
{
  auto trace_filter = script_context.GetArgument<TraceFilterProxy*>(0);
  auto callback = script_context.GetArgument<CallbackT>(1);

  trace_filter->SetGetTraceTypeCallback(callback);
}

void TraceFilterProxySetShouldHitEntityCallback(ScriptContext& script_context) {
  auto trace_filter = script_context.GetArgument<TraceFilterProxy*>(0);
  auto callback = script_context.GetArgument<CallbackT>(1);

  trace_filter->SetShouldHitEntityCallback(callback);
}

CGameTrace* NewTraceResult(ScriptContext& script_context) {
  return new CGameTrace();
}

CREATE_GETTER_FUNCTION(Trace, bool, DidHit, CGameTrace*, obj->DidHit());
CREATE_GETTER_FUNCTION(TraceResult, CBaseEntity*, Entity, CGameTrace*, obj->m_pEnt);

REGISTER_NATIVES(engine, {
  ScriptEngine::RegisterNativeHandler("GET_MAP_NAME", GetMapName);
  ScriptEngine::RegisterNativeHandler("GET_TICK_INTERVAL", GetTickInterval);
  ScriptEngine::RegisterNativeHandler("GET_TICK_COUNT", GetTickCount);
  ScriptEngine::RegisterNativeHandler("GET_CURRENT_TIME", GetCurrentTime);
  ScriptEngine::RegisterNativeHandler("GET_GAMEFRAME_TIME", GetGameFrameTime);
  ScriptEngine::RegisterNativeHandler("GET_ENGINE_TIME", GetEngineTime);
  ScriptEngine::RegisterNativeHandler("ISSUE_SERVER_COMMAND", ServerCommand);
  ScriptEngine::RegisterNativeHandler("PRECACHE_MODEL", PrecacheModel);
  ScriptEngine::RegisterNativeHandler("PRECACHE_SOUND", PrecacheSound);
  ScriptEngine::RegisterNativeHandler("IS_SOUND_PRECACHED", IsSoundPrecached);
  ScriptEngine::RegisterNativeHandler("GET_SOUND_DURATION", GetSoundDuration);
  ScriptEngine::RegisterNativeHandler("EMIT_SOUND", EmitSound);

  ScriptEngine::RegisterNativeHandler("NEW_SIMPLE_TRACE_FILTER", NewSimpleTraceFilter);
  ScriptEngine::RegisterNativeHandler("NEW_TRACE_RESULT", NewTraceResult);
  ScriptEngine::RegisterNativeHandler("TRACE_DID_HIT", TraceGetDidHit);
  ScriptEngine::RegisterNativeHandler("TRACE_RESULT_ENTITY", TraceResultGetEntity);

  ScriptEngine::RegisterNativeHandler("NEW_TRACE_FILTER_PROXY", NewTraceFilterProxy);
  ScriptEngine::RegisterNativeHandler("TRACE_FILTER_PROXY_SET_TRACE_TYPE_CALLBACK", TraceFilterProxySetTraceTypeCallback);
  ScriptEngine::RegisterNativeHandler("TRACE_FILTER_PROXY_SET_SHOULD_HIT_ENTITY_CALLBACK", TraceFilterProxySetShouldHitEntityCallback);

  ScriptEngine::RegisterNativeHandler("CREATE_RAY_1", CreateRay1);
  ScriptEngine::RegisterNativeHandler("CREATE_RAY_2", CreateRay2);
  ScriptEngine::RegisterNativeHandler("TRACE_RAY", TraceRay);
})
}
