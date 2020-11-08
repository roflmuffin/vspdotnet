/**
 * =============================================================================
 * Source Python
 * Copyright (C) 2012-2015 Source Python Development Team.  All rights reserved.
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

#include "core/datamaps.h"

#include <public/datamap.h>

#include <string>
#include <unordered_map>

namespace vspdotnet {

typedef std::unordered_map<std::string, typedescription_t*> OffsetsMap;
typedef std::unordered_map<std::string, OffsetsMap> DataMapsMap;

DataMapsMap g_DataMapsCache;

void AddDataMap(datamap_t* pDataMap, OffsetsMap& offsets, int offset = 0,
                const char* baseName = NULL);

void AddDataDesc(typedescription_t& dataDesc, OffsetsMap& offsets, int offset,
                 const char* baseName) {
  if (dataDesc.fieldName == NULL) return;

  int currentOffset = offset + dataDesc.fieldOffset;

  char* currentName = NULL;
  if (baseName == NULL) {
    currentName = (char*)dataDesc.fieldName;
  } else {
    char tempName[256];
    sprintf(tempName, "%s.%s", baseName, dataDesc.fieldName);
    currentName = strdup(tempName);
  }

  if (dataDesc.fieldType == FIELD_EMBEDDED) {
    AddDataMap(dataDesc.td, offsets, currentOffset, currentName);
  } else {
    offsets.insert(std::make_pair(currentName, &dataDesc));
  }
}

void AddDataMap(datamap_t* pDataMap, OffsetsMap& offsets, int offset,
                const char* baseName) {
  for (int i = 0; i < pDataMap->dataNumFields; i++) {
    AddDataDesc(pDataMap->dataDesc[i], offsets, offset, baseName);
  }
}

typedescription_t* DataMapSharedExt::Find(datamap_t* pDataMap,
                                          const char* szName) {
  while (pDataMap) {
    for (int iCurrentIndex = 0; iCurrentIndex < pDataMap->dataNumFields;
         iCurrentIndex++) {
      typedescription_t& pCurrentDataDesc = pDataMap->dataDesc[iCurrentIndex];
      if ((pCurrentDataDesc.fieldName &&
           strcmp(szName, pCurrentDataDesc.fieldName) == 0) ||
          (pCurrentDataDesc.externalName &&
           strcmp(szName, pCurrentDataDesc.externalName) == 0)) {
        return &(pDataMap->dataDesc[iCurrentIndex]);
      } else if (pCurrentDataDesc.fieldType == FIELD_EMBEDDED) {
        typedescription_t* pReturnValue = Find(pCurrentDataDesc.td, szName);
        if (pReturnValue) {
          return pReturnValue;
        }
      }
    }
    pDataMap = pDataMap->baseMap;
  }
  return NULL;
}

int DataMapSharedExt::FindOffset(datamap_t* pDataMap, const char* name) {
  while (pDataMap) {
    DataMapsMap::iterator offsets =
        g_DataMapsCache.find(pDataMap->dataClassName);
    if (offsets == g_DataMapsCache.end()) {
      offsets =
          g_DataMapsCache
              .insert(std::make_pair(pDataMap->dataClassName, OffsetsMap()))
              .first;
      AddDataMap(pDataMap, offsets->second);
    }

    OffsetsMap::iterator result = offsets->second.find(name);
    if (result == offsets->second.end()) {
      pDataMap = pDataMap->baseMap;
    } else {
      return result->second->fieldOffset;
    }
  }
  return -1;
}

typedescription_t* DataMapSharedExt::FindDescription(datamap_t* pDataMap,
                                                     const char* name) {
  while (pDataMap) {
    DataMapsMap::iterator offsets =
        g_DataMapsCache.find(pDataMap->dataClassName);
    if (offsets == g_DataMapsCache.end()) {
      offsets =
          g_DataMapsCache
              .insert(std::make_pair(pDataMap->dataClassName, OffsetsMap()))
              .first;
      AddDataMap(pDataMap, offsets->second);
    }

    OffsetsMap::iterator result = offsets->second.find(name);
    if (result == offsets->second.end()) {
      pDataMap = pDataMap->baseMap;
    } else {
      return result->second;
    }
  }

  return nullptr;
}

void* TypeDescriptionSharedExt::GetFunction(
    typedescription_t& pTypeDesc) {
  return (void*&)pTypeDesc.inputFunc;
}

}  // namespace vspdotnet