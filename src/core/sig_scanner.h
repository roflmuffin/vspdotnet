#pragma once

namespace vspdotnet {

  class CBinaryFile
  {
  public:
  CBinaryFile(unsigned long module, unsigned long base, unsigned long size) : m_module(module), m_base(base), m_size(size)
    {

    }

    unsigned long FindSignature(unsigned char* sigBytes, int length);

  public:
    unsigned long m_module;
    unsigned long m_base;
    unsigned long m_size;
  };

  CBinaryFile* FindBinary(const char* szPath, bool bSrvCheck = true, bool bCheckExtension = true);

}
