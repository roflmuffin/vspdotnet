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

//#undef GetClassName

namespace vspdotnet {

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

 private:
  int OffsetToInt(int offset, int bit_count, bool is_unsigned);
  void SetIntByOffset(int offset, int bit_count, bool is_unsigned, int value);
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
  SendProp* FindNetworkPropertyDescription(const char* name);

  template <class T>
  T GetProperty(PropType type, const char* name) {
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
        SendProp* info = FindNetworkPropertyDescription(name);

        if (!info) {
          //SetPendingException("Could not find network property %s", name);
          return -1;
        }

        bit_count = info->m_nBits;
        offset = info->GetOffset();
        is_unsigned = ((info->GetFlags() & SPROP_UNSIGNED) == SPROP_UNSIGNED);

        if (info->GetFlags() & SPROP_VARINT) {
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
        SendProp* info = FindNetworkPropertyDescription(name);

        if (!info) {
          //SetPendingException("Could not find network property %s", name);
          return;
        }

        bit_count = info->m_nBits;
        offset = info->GetOffset();
        is_unsigned = ((info->GetFlags() & SPROP_UNSIGNED) == SPROP_UNSIGNED);

        if (info->GetFlags() & SPROP_VARINT) {
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

  bool IsValid() { return ExcIndexFromBaseEntity(GetThis()) > -1; }

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