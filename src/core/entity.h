/**
 * =============================================================================
 * Source Python
 * Copyright (C) 2012-2016 Source Python Development Team.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, the Source Python Team gives you permission
 * to link the code of this program (as well as its derivative works) to
 * "Half-Life 2," the "Source Engine," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, the Source.Python
 * Development Team grants this exception to all derivative works.
 *
 * This file has been modified from its original form, under the GNU General
 * Public License, version 3.0.
 */

#pragma once

#include <datamap.h>
#include <dt_send.h>
#include <iserverentity.h>
#include <iservernetworkable.h>
#include <public/toolframework/itoolentity.h>
#include <public/iclient.h>

#include <cstdint>
#include <string>

#include "core/utilities/conversions.h"
#include "core/globals.h"
#include "core/props.h"
#include "core/log.h"

//#undef GetClassName

namespace vspdotnet {
struct SendPropInfo;

enum PropType { Send, Data };

enum PropEntType {
  PropEnt_Unknown,
  PropEnt_Handle,
  PropEnt_Entity,
  PropEnt_Edict,
  PropEnt_Variant,
};

enum PropFieldType {
  PropField_Unsupported, /**< The type is unsupported. */
  PropField_Integer,     /**< Valid for SendProp and Data fields */
  PropField_Float,       /**< Valid for SendProp and Data fields */
  PropField_Entity,   /**< Valid for Data fields only (SendProp shows as int) */
  PropField_Vector,   /**< Valid for SendProp and Data fields */
  PropField_String,   /**< Valid for SendProp and Data fields */
  PropField_String_T, /**< Valid for Data fields.  Read only! */
  PropField_Variant,  /**< Valid for variants/any. (User must know type) */
};

class CBaseEntityWrapper : public IServerEntity {
 protected:
  // Make sure that nobody can call the constructor/destructor
  CBaseEntityWrapper() {}
  ~CBaseEntityWrapper() {}

 public:
  // We need to keep the order of these methods up-to-date and maybe we need
  // to add new methods for other games.
  virtual ServerClass* GetServerClass() = 0;
  virtual int YouForgotToImplementOrDeclareServerClass() = 0;
  virtual datamap_t* GetDataDescMap() = 0;
  virtual bool IsMoving(void) = 0;
  int OffsetToInt(int offset, int bit_count, bool is_unsigned);
  void SetIntByOffset(int offset, int bit_count, bool is_unsigned, int value);
 private:  
  int MatchTypeDescAsInteger(_fieldtypes type, int flags);

  int GetFloatOffset(PropType type, const char* name);
  int GetStringOffset(PropType type, const char* name);
  int GetVectorOffset(PropType type, const char* name);
  int GetEntOffset(PropType type, const char* name, PropEntType& prop_ent);

 public:
  CBaseEntity* GetThis() const;
  int FindDatamapPropertyOffset(const char* name);
  typedescription_t* FindDatamapPropertyDescription(const char* name);

  template <class T>
  T GetPropertyByOffset(int offset) {
    return *(T*)(((unsigned long)this) + offset);
  }

  template <class T>
  T* GetPropertyPointerByOffset(int offset) {
    return (T*)(((unsigned long)this) + offset);
  }

  template <class T>
  void SetPropertyByOffset(int offset, T value) {
    *(T*)(((unsigned long)this) + offset) = value;
  }

  const char* GetPropertyStringArrayByOffset(int offset) {
    return (const char*)(((unsigned long)this) + offset);
  }

  void SetPropertyStringArrayByOffset(int offset, const char* value) {
    strcpy((char*)(((unsigned long)this) + offset), value);
  }

  int FindNetworkPropertyOffset(const char* name);
  SendPropInfo* FindNetworkPropertyDescription(const char* name);

  template <class T>
  T GetProperty(PropType type, const char* name)
  {
    int offset;
    int bit_count;
    bool is_unsigned = false;

    auto pointer = ((unsigned long)this);

    switch (type) {
      case Data: {
        typedescription_t* td = FindDatamapPropertyDescription(name);

        if (!td) {
          //SetPendingException("Could not find d property %s", name);
          return -1;
        }

        bit_count = MatchTypeDescAsInteger(td->fieldType, td->flags);
        offset = td->fieldOffset;

        if (bit_count == 0) {
          //SetPendingException("Data field %s is not an integer (%d)", name,
                              //td->fieldType);
          return -1;
        }

        break;
      }
      case Send: {
        SendPropInfo* info = FindNetworkPropertyDescription(name);

        if (!info) {
          //SetPendingException("Could not find network property %s", name);
          return -1;
        }

        bit_count = info->sendprop->m_nBits;
        offset = info->actual_offset;
        is_unsigned = ((info->sendprop->GetFlags() & SPROP_UNSIGNED) == SPROP_UNSIGNED);

        if (info->sendprop->GetFlags() & SPROP_VARINT) {
          bit_count = sizeof(int) * 8;
        }

        break;
      }
      default: {
        //SetPendingException("Invalid Property type %d", type);
        return -1;
      }
    }

    return OffsetToInt(offset, bit_count, is_unsigned);
  }

  template <class T>
  void SetProperty(PropType type, const char* name, T value) {
    int offset;
    int bit_count;
    bool is_unsigned = false;

    switch (type) {
      case Data: {
        typedescription_t* td = FindDatamapPropertyDescription(name);

        if (!td) {
          //SetPendingException("Could not find d property %s", name);
          return;
        }

        bit_count = MatchTypeDescAsInteger(td->fieldType, td->flags);
        offset = td->fieldOffset;

        if (bit_count == 0) {
          //SetPendingException("Data field %s is not an integer (%d)", name,
                              //td->fieldType);
          return;
        }

        break;
      }
      case Send: {
        SendPropInfo* info = FindNetworkPropertyDescription(name);

        if (!info) {
          //SetPendingException("Could not find network property %s", name);
          return;
        }

        bit_count = info->sendprop->m_nBits;
        offset = info->actual_offset;
        is_unsigned = ((info->sendprop->GetFlags() & SPROP_UNSIGNED) == SPROP_UNSIGNED);

        if (info->sendprop->GetFlags() & SPROP_VARINT) {
          bit_count = sizeof(int) * 8;
        }

        break;
      }
      default: {
        //SetPendingException("Invalid Property type %d", type);
        return;
      }
    }

    SetIntByOffset(offset, bit_count, is_unsigned, value);

    if (type == Send && GetEdict()) {
      GetEdict()->StateChanged(offset);
    }
  }

 public:
  std::string GetKeyValueStringRaw(const char* szName);

  template <class T>
  void SetKeyValue(const char* szName, T value) {
    globals::server_tools->SetKeyValue(GetThis(), szName, value);
  }

  void SetKeyValue(const char* name, const char* value) {
    SetKeyValue<const char*>(name, value);
  }

  edict_t* GetEdict() { return ExcEdictFromBaseEntity(GetThis()); }

  int GetIndex() { return ExcIndexFromBaseEntity(GetThis()); }

  bool IsValid()
  {
    auto index = ExcIndexFromBaseEntity(GetThis());
    VSPDN_CORE_INFO("Is Valid Check on Entity {0} is {1}", (void*)GetThis(),
                    index);
    return index > -1;
  }

  const char* GetClassname() { return GetNetworkable()->GetClassName(); }

  int GetProp(PropType type, const char* name);
  void SetProp(PropType type, const char* name, int value);
  float GetPropFloat(PropType type, const char* name);
  void SetPropFloat(PropType type, const char* name, float value);
  const char* GetPropString(PropType type, const char* name);
  void SetPropString(PropType type, const char* name, const char* value);
  Vector* GetPropVector(PropType type, const char* name);
  void SetPropVector(PropType type, const char* name, Vector& value);
  int GetPropEnt(PropType type, const char* name);
  void SetPropEnt(PropType type, const char* name, int value);

  int FindSendPropInfo(const char* name, PropFieldType& prop_field_type,
                       int& num_bits, int& offset);

  void Spawn();

  bool IsPlayer();
  bool IsWeapon();

  IClient* GetIClient();
};
}  // namespace vspdotnet