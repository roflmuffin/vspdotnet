#include "core/datamaps.h"

#include <unordered_map>
#include <string>

#include <datamap.h>

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
}  // namespace vspdotnet