#include "core/entity.h"

#include <iserver.h>


#include "core/props.h"
#include "core/utilities/conversions.h"
#include "core/datamaps.h"
#include "server_class.h"
#include "eiface.h"

//-----------------------------------------------------------------------------

namespace vspdotnet {
class variant_t {
 public:
  union {
    bool bVal;
    string_t iszVal;
    int iVal;
    float flVal;
    float vecVal[3];
    color32 rgbaVal;
  };

  CBaseHandle eVal;
  fieldtype_t fieldType;
};

int CBaseEntityWrapper::OffsetToInt(int offset, int bit_count,
                                    bool is_unsigned) {
  if (bit_count >= 17) {
    return *(int32_t*)((uint8_t*)this + offset);
  } else if (bit_count >= 9) {
    if (is_unsigned) {
      return *(uint16_t*)((uint8_t*)this + offset);
    } else {
      return *(int16_t*)((uint8_t*)this + offset);
    }
  } else if (bit_count >= 2) {
    if (is_unsigned) {
      return *(uint8_t*)((uint8_t*)this + offset);
    } else {
      return *(int8_t*)((uint8_t*)this + offset);
    }
  } else {
    return *(bool*)((uint8_t*)this + offset) ? 1 : 0;
  }
}

void CBaseEntityWrapper::SetIntByOffset(int offset, int bit_count,
                                        bool is_unsigned, int value) {
  if (bit_count >= 17) {
    *(int32_t*)((uint8_t*)this + offset) = value;
  } else if (bit_count >= 9) {
    *(int16_t*)((uint8_t*)this + offset) = (int16_t)value;
  } else if (bit_count >= 2) {
    *(int8_t*)((uint8_t*)this + offset) = (int8_t)value;
  } else {
    *(bool*)((uint8_t*)this + offset) = value ? true : false;
  }
}

CBaseEntity* CBaseEntityWrapper::GetThis() const { return (CBaseEntity*)this; }

int CBaseEntityWrapper::FindDatamapPropertyOffset(const char* name) {
  datamap_t* datamap = GetDataDescMap();

  int offset = DataMapSharedExt::FindOffset(datamap, name);

  return offset;
}

typedescription_t* CBaseEntityWrapper::FindDatamapPropertyDescription(
    const char* name) {
  datamap_t* datamap = GetDataDescMap();

  return DataMapSharedExt::FindDescription(datamap, name);
}

SendPropInfo* CBaseEntityWrapper::FindNetworkPropertyDescription(const char* name) {
  ServerClass* pServerClass = GetServerClass();
  if (!pServerClass) return nullptr;

  SendPropInfo* offset =
      SendTableSharedExt::FindDescription(pServerClass->m_pTable, name);

  return offset;
}

int CBaseEntityWrapper::FindNetworkPropertyOffset(const char* name) {
  ServerClass* pServerClass = GetServerClass();
  if (!pServerClass) return -1;

  int offset;
  offset = SendTableSharedExt::FindOffset(pServerClass->m_pTable, name);
  if (offset == -1) return -1;

  // TODO: Proxied RecvTables/Arrays
  if (offset == 0) offset = FindDatamapPropertyOffset(name);

  return offset;
}

int CBaseEntityWrapper::MatchTypeDescAsInteger(_fieldtypes type, int flags) {
  switch (type) {
    case FIELD_TICK:
    case FIELD_MODELINDEX:
    case FIELD_MATERIALINDEX:
    case FIELD_INTEGER:
    case FIELD_COLOR32:
      return 32;
    case FIELD_CUSTOM:
      if ((flags & FTYPEDESC_OUTPUT) == FTYPEDESC_OUTPUT) {
        // Variant, read as int32.
        return 32;
      }
      break;
    case FIELD_SHORT:
      return 16;
    case FIELD_CHARACTER:
      return 8;
    case FIELD_BOOLEAN:
      return 1;
    default:
      return 0;
  }

  return 0;
}

std::string CBaseEntityWrapper::GetKeyValueStringRaw(const char* szName) {
  char out[2048];

  globals::server_tools->GetKeyValue(GetThis(), szName, out, 2048);

  auto result = std::string(out);
  result.shrink_to_fit();
  return result;
}

int CBaseEntityWrapper::GetProp(PropType type, const char* name) {
  return GetProperty<int>(type, name);
}

void CBaseEntityWrapper::SetProp(PropType type, const char* name, int value) {
  return SetProperty<int>(type, name, value);
}

int CBaseEntityWrapper::GetFloatOffset(PropType type, const char* name) {
  int offset = 0;
  if (type == Data) {
    auto td = FindDatamapPropertyDescription(name);
    if (!td) {
      // SetPendingException("Could not find data property %s", name);
      return -1;
    }

    if (td->fieldType != FIELD_FLOAT && td->fieldType != FIELD_TIME) {
      // SetPendingException("Data field %s is not a float (%d != [%d,%d])",
      // name,
      //                    td->fieldType, FIELD_FLOAT, FIELD_TIME);
      return -1;
    }

    offset = td->fieldOffset;
  } else if (type == Send) {
    auto td = FindNetworkPropertyDescription(name);
    if (!td) {
      // SetPendingException("Could not find network property %s", name);
      return -1;
    }

    if (td->sendprop->m_Type != DPT_Float) {
      // SetPendingException("Network field %s is not a float (%d != [%d])",
      // name,
                          //td->m_Type, DPT_Float);
                          return -1;
    }

    offset = td->actual_offset;
  }

  if (offset == 0) {
    // SetPendingException("Could not find property %s", name);
    return -1;
  }

  return offset;
}

int CBaseEntityWrapper::GetStringOffset(PropType type, const char* name) {
  int offset = 0;
  if (type == Data) {
    auto td = FindDatamapPropertyDescription(name);
    if (!td) {
      // SetPendingException("Could not find data property %s", name);
      return -1;
    }

    if ((td->fieldType != FIELD_CHARACTER && td->fieldType != FIELD_STRING &&
         td->fieldType != FIELD_MODELNAME &&
         td->fieldType != FIELD_SOUNDNAME) ||
        (td->fieldType == FIELD_CUSTOM &&
         (td->flags & FTYPEDESC_OUTPUT) != FTYPEDESC_OUTPUT)) {
      // SetPendingException("Data field %s is not a string (%d != %d)", name,
      //                   td->fieldType, FIELD_CHARACTER);
      return -1;
    }

    offset = td->fieldOffset;
  } else if (type == Send) {
    auto td = FindNetworkPropertyDescription(name);
    if (!td) {
      // SetPendingException("Could not find network property %s", name);
      return -1;
    }

    if (td->sendprop->m_Type != DPT_Float) {
      // SetPendingException("Network field %s is not a string (%d != [%d])",
      // name,
                          //td->m_Type, DPT_String);
                          return -1;
    }

    offset = td->actual_offset;
  }

  if (offset == 0) {
    // SetPendingException("Could not find property %s", name);
    return -1;
  }

  return offset;
}

int CBaseEntityWrapper::GetVectorOffset(PropType type, const char* name) {
  int offset = 0;
  if (type == Data) {
    auto td = FindDatamapPropertyDescription(name);
    if (!td) {
      // SetPendingException("Could not find data property %s", name);
      return -1;
    }

    if (td->fieldType != FIELD_VECTOR &&
        td->fieldType != FIELD_POSITION_VECTOR) {
      // SetPendingException("Data field %s is not a vector (%d != [%d,%d])",
      // name,
      //                    td->fieldType, FIELD_VECTOR, FIELD_POSITION_VECTOR);
      return -1;
    }

    offset = td->fieldOffset;
  } else if (type == Send) {
    auto td = FindNetworkPropertyDescription(name);
    if (!td) {
      // SetPendingException("Could not find network property %s", name);
      return -1;
    }

    if (td->sendprop->m_Type != DPT_Vector) {
      // SetPendingException("Network field %s is not a vector (%d != [%d])",
      // name,
                          //td->m_Type, DPT_Vector);
                          return -1;
    }

    offset = td->actual_offset;
  }

  if (offset == 0) {
    // SetPendingException("Could not find property %s", name);
    return -1;
  }

  return offset;
}

int CBaseEntityWrapper::GetEntOffset(PropType type, const char* name,
                                     PropEntType& prop_ent_type) {
  int offset = 0;

  if (type == Data) {
    auto td = FindDatamapPropertyDescription(name);
    if (!td) {
      // SetPendingException("Could not find data property %s", name);
      return -1;
    }

    switch (td->fieldType) {
      case FIELD_EHANDLE:
        prop_ent_type = PropEnt_Handle;
        break;
      case FIELD_CLASSPTR:
        prop_ent_type = PropEnt_Entity;
        break;
      case FIELD_EDICT:
        prop_ent_type = PropEnt_Edict;
        break;
      case FIELD_CUSTOM:
        if ((td->flags & FTYPEDESC_OUTPUT) == FTYPEDESC_OUTPUT) {
          prop_ent_type = PropEnt_Variant;
        }
        break;
    }

    if (prop_ent_type == PropEnt_Unknown) {
      // SetPendingException("Data field %s is not an entity or edict (%d)",
      // name,
                          //td->fieldType);
                          return -1;
    }

    offset = td->fieldOffset;
  } else if (type == Send) {
    auto td = FindNetworkPropertyDescription(name);
    if (!td) {
      // SetPendingException("Could not find network property %s", name);
      return -1;
    }

    if (td->sendprop->m_Type != DPT_Int) {
      // SetPendingException(
      //     "Network field %s is not an entity or edict (%d != [%d])", name,
      //     td->m_Type, DPT_Int);
      return -1;
    }

    prop_ent_type = PropEnt_Handle;

    offset = td->actual_offset;
  }

  if (offset == 0) {
    // SetPendingException("Could not find property %s", name);
    return -1;
  }

  return offset;
}

float CBaseEntityWrapper::GetPropFloat(PropType type, const char* name) {
  int offset = GetFloatOffset(type, name);

  if (offset > 0) {
    return GetPropertyByOffset<float>(offset);
  }

  return -1;
}

void CBaseEntityWrapper::SetPropFloat(PropType type, const char* name,
                                      float value) {
  int offset = GetFloatOffset(type, name);
  if (offset > 0) {
    return SetPropertyByOffset<float>(offset, value);
  }
}

const char* CBaseEntityWrapper::GetPropString(PropType type, const char* name) {
  int offset = GetStringOffset(type, name);
  if (offset > 0) {
    return GetPropertyStringArrayByOffset(offset);
  }

  return nullptr;
}

void CBaseEntityWrapper::SetPropString(PropType type, const char* name,
                                       const char* value) {
  int offset = GetStringOffset(type, name);
  if (offset > 0) {
    SetPropertyStringArrayByOffset(offset, value);
  }
}

Vector* CBaseEntityWrapper::GetPropVector(PropType type, const char* name) {
  int offset = GetVectorOffset(type, name);
  if (offset > 0) {
    return GetPropertyPointerByOffset<Vector>(offset);
  }

  return nullptr;
}

void CBaseEntityWrapper::SetPropVector(PropType type, const char* name,
                                       Vector& value) {
  int offset = GetVectorOffset(type, name);
  if (offset > 0) {
    Vector* p_vector = GetPropertyPointerByOffset<Vector>(offset);
    p_vector->x = value.x;
    p_vector->y = value.y;
    p_vector->z = value.z;
  }
}

int CBaseEntityWrapper::GetPropEnt(PropType type, const char* name) {
  PropEntType ent_type = PropEnt_Unknown;
  int offset = GetEntOffset(type, name, ent_type);

  if (offset <= 0) return -1;

  switch (ent_type) {
    case PropEnt_Handle:
    case PropEnt_Variant: {
      CBaseHandle* hndl;
      if (ent_type == PropEnt_Handle) {
        hndl = (CBaseHandle*)((uint8_t*)this + offset);
      } else  // PropEnt_Variant
      {
        auto* pVariant = (variant_t*)((intptr_t)this + offset);
        hndl = &pVariant->eVal;
      }

      int index = -1;
      IndexFromBaseHandle(*hndl, index);

      auto edict = ExcEdictFromIndex(index);

      CBaseEntity* pHandleEntity;
      if (!BaseEntityFromEdict(edict, pHandleEntity)) return -1;

      if (!pHandleEntity) return -1;

      return ((CBaseEntityWrapper*)pHandleEntity)->GetIndex();
    }
    case PropEnt_Entity: {
      CBaseEntity* pPropEntity = *(CBaseEntity**)((uint8_t*)this + offset);
      return ((CBaseEntityWrapper*)pPropEntity)->GetIndex();
    }
    case PropEnt_Edict: {
      edict_t* pEdict = *(edict_t**)((uint8_t*)this + offset);
      if (!pEdict || pEdict->IsFree()) return -1;

      return ExcIndexFromEdict(pEdict);
    }
  }

  return -1;
}

void CBaseEntityWrapper::SetPropEnt(PropType type, const char* name,
                                    int value) {
  PropEntType ent_type = PropEnt_Unknown;
  int offset = GetEntOffset(type, name, ent_type);

  if (offset <= 0) return;

  CBaseEntity* pOther = ExcBaseEntityFromIndex(value);
  if (!pOther) {
    //SetPendingException(
   //     "Could not find entity to set as property with index %d", value);
  }

  switch (ent_type) {
    case PropEnt_Handle:
    case PropEnt_Variant: {
      CBaseHandle* hndl;
      if (ent_type == PropEnt_Handle) {
        hndl = (CBaseHandle*)((uint8_t*)this + offset);
      } else  // PropEnt_Variant
      {
        auto* pVariant = (variant_t*)((intptr_t)this + offset);
        hndl = &pVariant->eVal;
      }

      hndl->Set((IHandleEntity*)pOther);
    }

    break;

    case PropEnt_Entity: {
      *(CBaseEntity**)((uint8_t*)this + offset) = pOther;
      break;
    }

    case PropEnt_Edict: {
      edict_t* pOtherEdict = NULL;
      if (pOther) {
        IServerNetworkable* pNetworkable =
            ((IServerUnknown*)pOther)->GetNetworkable();
        if (!pNetworkable) {
          //SetPendingException("Entity %d does not have a valid edict", value);
          return;
        }

        pOtherEdict = pNetworkable->GetEdict();
        if (!pOtherEdict || pOtherEdict->IsFree()) {
          //SetPendingException("Entity %d does not have a valid edict", value);
          return;
        }
      }

      *(edict_t**)((uint8_t*)this + offset) = pOtherEdict;
      break;
    }
  }
}

int CBaseEntityWrapper::FindSendPropInfo(const char* name,
                                         PropFieldType& prop_field_type,
                                         int& num_bits, int& offset) {
  auto td = FindNetworkPropertyDescription(name);
  if (!td) {
    //SetPendingException("Could not find network property %s", name);
    return -1;
  }

  switch (td->sendprop->GetType()) {
    case DPT_Int: {
      prop_field_type = PropField_Integer;
      break;
    }
    case DPT_Float: {
      prop_field_type = PropField_Float;
      break;
    }
    case DPT_String: {
      prop_field_type = PropField_String;
      break;
    }
    case DPT_Vector: {
      prop_field_type = PropField_Vector;
      break;
    }
    default: {
      prop_field_type = PropField_Unsupported;
      break;
    }
  }

  num_bits = td->sendprop->m_nBits;
  offset = td->actual_offset;

  return offset;
}

void CBaseEntityWrapper::Spawn() { globals::server_tools->DispatchSpawn(GetThis()); }

bool CBaseEntityWrapper::IsPlayer() {
  int iEntityIndex;
  if (!IndexFromBaseEntity(GetThis(), iEntityIndex)) return false;

  return iEntityIndex > WORLD_ENTITY_INDEX &&
         iEntityIndex <= (unsigned int)vspdotnet::globals::gpGlobals->maxClients;
}

bool CBaseEntityWrapper::IsWeapon() {
  datamap_t* pDatamap = GetDataDescMap();
  while (pDatamap) {
    if (strcmp(pDatamap->dataClassName, "CBaseCombatWeapon") == 0) return true;
    pDatamap = pDatamap->baseMap;
  }

  return false;
}

IClient* CBaseEntityWrapper::GetIClient() {
  if (!IsPlayer()) return nullptr;

  IClient* client = globals::server->GetClient(GetIndex());
  return client;
}


}  // namespace vspdotnet