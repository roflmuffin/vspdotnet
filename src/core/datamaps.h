#pragma once

struct datamap_t;
struct typedescription_t;

namespace vspdotnet {
class DataMapSharedExt {
 public:
  static typedescription_t* Find(datamap_t* pDataMap, const char* szName);
  static int FindOffset(datamap_t* pDataMap, const char* name);
  static typedescription_t* FindDescription(datamap_t* pDataMap,
                                            const char* name);
};

class TypeDescriptionSharedExt {
 public:
  static void* GetFunction(typedescription_t& pTypeDesc);
};

}  // namespace vspdotnet