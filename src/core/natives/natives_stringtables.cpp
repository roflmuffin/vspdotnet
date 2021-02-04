#include <eiface.h>
#include <networkstringtabledefs.h>


#include "core/autonative.h"
#include "core/script_engine.h"

namespace vspdotnet {

bool LockStringTables(ScriptContext& script_context)
{
  auto lock = script_context.GetArgument<bool>(0);

  return globals::engine->LockNetworkStringTables(lock);
}

int FindStringTable(ScriptContext& script_context)
{
  auto name = script_context.GetArgument<const char*>(0);

  auto* table = globals::net_string_tables->FindTable(name);

  if (!table)
  {
    return INVALID_STRING_TABLE;
  }

  return table->GetTableId();
}

CREATE_STATIC_GETTER_FUNCTION(NumStringTables, int, globals::net_string_tables->GetNumTables());

int GetStringTableNumStrings(ScriptContext& script_context) {
  auto index = script_context.GetArgument<int>(0);
  auto table = globals::net_string_tables->GetTable(index);


  if (!table) {
    script_context.ThrowNativeError("Invalid string table index %d", index);
    return -1;
  }

  return table->GetNumStrings();
}

int GetStringTableMaxStrings(ScriptContext& script_context) {
  auto index = script_context.GetArgument<int>(0);
  auto table = globals::net_string_tables->GetTable(index);

  if (!table) {
    script_context.ThrowNativeError("Invalid string table index %d", index);
    return -1;
  }

  return table->GetMaxStrings();
}

const char* GetStringTableName(ScriptContext& script_context)
{
  auto index = script_context.GetArgument<int>(0);
  auto table = globals::net_string_tables->GetTable(index);

  if (!table) {
    script_context.ThrowNativeError("Invalid string table index %d", index);
    return nullptr;
  }

  return table->GetTableName();
}

int FindStringIndex(ScriptContext& script_context) {
  auto index = script_context.GetArgument<int>(0);
  auto lookup = script_context.GetArgument<const char*>(1);

  auto table = globals::net_string_tables->GetTable(index);

  if (!table) {
    script_context.ThrowNativeError("Invalid string table index %d", index);
    return -1;
  }

  return table->FindStringIndex(lookup);
}

const char* ReadStringTable(ScriptContext& script_context) {
  auto index = script_context.GetArgument<int>(0);
  auto string_id = script_context.GetArgument<int>(1);

  auto table = globals::net_string_tables->GetTable(index);

  if (!table) {
    script_context.ThrowNativeError("Invalid string table index %d", index);
    return nullptr;
  }

  auto found_value = table->GetString(string_id);

  if (!found_value) {
    script_context.ThrowNativeError(
        "Invalid string index for table (index %d) (table \"%s\")", string_id,
        table->GetTableName());
    return nullptr;
  }

  return found_value;
}

int GetStringTableDataLength(ScriptContext& script_context)
{
  auto index = script_context.GetArgument<int>(0);
  auto string_id = script_context.GetArgument<int>(1);

  auto table = globals::net_string_tables->GetTable(index);

  if (!table) {
    script_context.ThrowNativeError("Invalid string table index %d", index);
    return -1;
  }

  if (string_id < 0 || string_id >= table->GetNumStrings()) {
    script_context.ThrowNativeError(
        "Invalid string index specified for table (index %d) (table \"%s\")",
        script_context, table->GetTableName());
    return -1;
  }

  int datalen;
  auto userdata = table->GetStringUserData(string_id, &datalen);

  if (!userdata)
  {
    datalen = 0;
  }

  return datalen;  
}

const char* GetStringTableData(ScriptContext& script_context)
{
  auto index = script_context.GetArgument<int>(0);
  auto string_id = script_context.GetArgument<int>(1);

  auto table = globals::net_string_tables->GetTable(index);

  if (!table) {
    script_context.ThrowNativeError("Invalid string table index %d", index);
    return nullptr;
  }

  if (string_id < 0 || string_id >= table->GetNumStrings()) {
    script_context.ThrowNativeError(
        "Invalid string index specified for table (index %d) (table \"%s\")",
        script_context, table->GetTableName());
    return nullptr;
  }

  int datalen = 0;
  const char* userdata = (const char*)table->GetStringUserData(string_id, &datalen);

  return userdata;
}

void SetStringTableData(ScriptContext& script_context) {
  auto index = script_context.GetArgument<int>(0);
  auto string_id = script_context.GetArgument<int>(1);
  auto value = script_context.GetArgument<const char*>(2);

  auto table = globals::net_string_tables->GetTable(index);

  if (!table) {
    return script_context.ThrowNativeError("Invalid string table index %d", index);
  }

  if (string_id < 0 || string_id >= table->GetNumStrings()) {
    return script_context.ThrowNativeError(
        "Invalid string index specified for table (index %d) (table \"%s\")",
        index, table->GetTableName());
  }

  table->SetStringUserData(string_id, sizeof(value), value);
}


void AddToStringTable(ScriptContext& script_context)
{
  auto index = script_context.GetArgument<int>(0);
  auto value = script_context.GetArgument<const char*>(1);
  auto userdata = script_context.GetArgument<const char*>(2);

  auto table = globals::net_string_tables->GetTable(index);

  if (!table) {
    return script_context.ThrowNativeError("Invalid string table index %d", index);
  }

  table->AddString(true, value, sizeof(value), userdata);
}

REGISTER_NATIVES(stringtables, {
  ScriptEngine::RegisterNativeHandler("ADD_TO_STRING_TABLE", AddToStringTable);
  ScriptEngine::RegisterNativeHandler("FIND_STRING_INDEX", FindStringIndex);
  ScriptEngine::RegisterNativeHandler("FIND_STRING_TABLE", FindStringTable);
  ScriptEngine::RegisterNativeHandler("GET_NUM_STRING_TABLES", GetNumStringTables);
  ScriptEngine::RegisterNativeHandler("GET_STRING_TABLE_DATA", GetStringTableData);
  ScriptEngine::RegisterNativeHandler("GET_STRING_TABLE_DATA_LENGTH", GetStringTableDataLength);
  ScriptEngine::RegisterNativeHandler("GET_STRING_TABLE_MAX_STRINGS", GetStringTableMaxStrings);
  ScriptEngine::RegisterNativeHandler("GET_STRING_TABLE_NAME", GetStringTableName);
  ScriptEngine::RegisterNativeHandler("GET_STRING_TABLE_NUM_STRINGS", GetStringTableNumStrings);
  ScriptEngine::RegisterNativeHandler("LOCK_STRING_TABLES", LockStringTables);
  ScriptEngine::RegisterNativeHandler("READ_STRING_TABLE", ReadStringTable);
  ScriptEngine::RegisterNativeHandler("SET_STRING_TABLE_DATA", SetStringTableData);
})
}