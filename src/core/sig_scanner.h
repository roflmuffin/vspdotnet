#pragma once

namespace vspdotnet {

  class CBinaryFile
  {
  public:
  CBinaryFile(unsigned long module, unsigned long base, unsigned long size) : m_module_(module), m_base_(base), m_size_(size)
    {

    }

    unsigned long FindSignature(unsigned char* sigBytes, int length);

  public:
    unsigned long m_module_;
    unsigned long m_base_;
    unsigned long m_size_;
  };

  CBinaryFile* FindBinary(const char* szPath, bool bSrvCheck = true, bool bCheckExtension = true);

}
