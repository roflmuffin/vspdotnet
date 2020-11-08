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

#include <string>
#include <unordered_map>

#include "core/props.h"

namespace vspdotnet {


typedef std::unordered_map<std::string, SendPropInfo*> OffsetsMap;
typedef std::unordered_map<std::string, OffsetsMap> SendTableMap;

SendTableMap g_SendTableCache;

SendTable* GetNextSendTable(SendTable* pTable) {
  for (int i = 0; i < pTable->GetNumProps(); ++i) {
    SendProp* pProp = pTable->GetProp(i);
    if (strcmp(pProp->GetName(), "baseclass") != 0) continue;

    return pProp->GetDataTable();
  }
  return NULL;
}

void AddSendTable(SendTable* pTable, OffsetsMap& offsets, int offset = 0,
                  const char* baseName = NULL) {
  for (int i = 0; i < pTable->GetNumProps(); ++i) {
    SendProp* pProp = pTable->GetProp(i);
    if (strcmp(pProp->GetName(), "baseclass") == 0 || pProp->IsExcludeProp() ||
        pProp->GetFlags() & SPROP_COLLAPSIBLE)
      continue;

    int currentOffset = offset + pProp->GetOffset();

    char* currentName = (char*)pProp->GetName();
    /*char* currentName = NULL;
    if (baseName == NULL) {
      currentName = (char*)pProp->GetName();
    } else {
      char tempName[256];
      sprintf(tempName, "%s.%s", baseName, pProp->GetName());
      currentName = strdup(tempName);
    }*/

    if (pProp->GetType() == DPT_DataTable) {
      AddSendTable(pProp->GetDataTable(), offsets, currentOffset, currentName);
    } else {
      auto prop_info = new SendPropInfo{pProp, currentOffset};
      offsets.insert(std::make_pair(currentName, prop_info));
    }
  }
}

int SendTableSharedExt::FindOffset(SendTable* pTable, const char* name) {
  while (pTable) {
    SendTableMap::iterator offsets = g_SendTableCache.find(pTable->GetName());
    if (offsets == g_SendTableCache.end()) {
      offsets = g_SendTableCache
                    .insert(std::make_pair(pTable->GetName(), OffsetsMap()))
                    .first;
      AddSendTable(pTable, offsets->second);
    }

    OffsetsMap::iterator result = offsets->second.find(name);
    if (result == offsets->second.end()) {
      pTable = GetNextSendTable(pTable);
    } else {
      return result->second->actual_offset;
    }
  }
  return -1;
}

SendPropInfo* SendTableSharedExt::FindDescription(SendTable* pTable,
                                              const char* name) {
  while (pTable) {
    SendTableMap::iterator offsets = g_SendTableCache.find(pTable->GetName());
    if (offsets == g_SendTableCache.end()) {
      offsets = g_SendTableCache
                    .insert(std::make_pair(pTable->GetName(), OffsetsMap()))
                    .first;
      AddSendTable(pTable, offsets->second);
    }

    OffsetsMap::iterator result = offsets->second.find(name);
    if (result == offsets->second.end()) {
      pTable = GetNextSendTable(pTable);
    } else {
      return result->second;
    }
  }

  return nullptr;
}

}  // namespace vspdotnet