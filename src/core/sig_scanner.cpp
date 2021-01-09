#include "sig_scanner.h"

#include "globals.h"

#include <public/eiface.h>

#include <stdio.h>
#include <fcntl.h>

#ifdef _WIN32
	#include <windows.h>
#else
	#include <fcntl.h>
	#include <link.h>
	#include <sys/mman.h>
	extern int PAGE_SIZE;
	#define PAGE_ALIGN_UP(x) ((x + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#endif

// #include <sys/mman.h>

// #include <elf.h>
// #include <link.h>

#include <dynload.h>

#include <list>
#include <string>

namespace vspdotnet {

  std::list<CBinaryFile*> m_Binaries;

  bool str_ends_with(const char* szString, const char* szSuffix)
  {
    int stringlen = strlen(szString);
    int suffixlen = strlen(szSuffix);
    if (suffixlen > stringlen)
      return false;

    return strncmp(szString + stringlen - suffixlen, szSuffix, suffixlen) == 0;
  }

  unsigned long CBinaryFile::FindSignature(unsigned char* sigBytes, int length)
  {
    unsigned char* base = (unsigned char*)m_base_;
    unsigned char* end = (unsigned char*)(base + m_size_ - length);

    while(base < end)
    {
      int i = 0;
      for (; i < length; i++)
      {
        if (sigBytes[i] == '\x2A')
          continue;

        if (sigBytes[i] != base[i])
          break;
      }

      if (i == length)
      {
        return (unsigned long)base;
      }
      base++;
    }

    return -1;
  }

  CBinaryFile* FindBinary(const char* szPath, bool bSrvCheck, bool bCheckExtension)
  {
    std::string szBinaryPath = szPath;
#ifdef __linux__
    if (bCheckExtension) {
      if (bSrvCheck && !str_ends_with(szBinaryPath.data(), "_srv") && !str_ends_with(szBinaryPath.data(), ".so"))
        szBinaryPath += "_srv.so";
      else if (!str_ends_with(szBinaryPath.data(), ".so"))
        szBinaryPath += ".so";
    }
#endif

    unsigned long ulModule = (unsigned long)dlLoadLibrary(szBinaryPath.data());
    unsigned long ulBase = 0;
#ifdef __linux__
    if (!ulModule)
    {
      char szGameDir[1024];
      vspdotnet::globals::engine->GetGameDir(szGameDir, 1024);

      // If the previous path failed, try the "bin" folder of the game.
      // This will allow passing e.g. "server" to this function.
      szBinaryPath = std::string(szGameDir) + "/bin/" + szBinaryPath;
      ulModule = (unsigned long)dlLoadLibrary(szBinaryPath.data());
    }
#endif

    if (!ulModule)
    {
      szBinaryPath = "Unable to find " + szBinaryPath;
#ifdef _WIN32
      if (bCheckExtension && !str_ends_with(szBinaryPath.data(), ".dll"))
        szBinaryPath += ".dll";
      Msg("%s", szBinaryPath.data());
#endif
    }

    // Search for an existing BinaryFile object
    for (std::list<CBinaryFile*>::iterator iter = m_Binaries.begin(); iter != m_Binaries.end(); ++iter)
    {
      CBinaryFile* binary = *iter;
      if (binary->m_module_ == ulModule)
      {
        // We don't need to open it several times
        dlFreeLibrary((DLLib*)ulModule);
        return binary;
      }
    }

    unsigned long ulSize;

#ifdef _WIN32
    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)ulModule;
    IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)((BYTE*)dos + dos->e_lfanew);
    ulSize = nt->OptionalHeader.SizeOfImage;
    ulBase = ulModule;

#elif defined(__linux__)
    // Copied from here. Thanks!
    // https://github.com/alliedmodders/sourcemod/blob/237db0504c7a59e394828446af3e8ca3d53ef647/core/logic/MemoryUtils.cpp#L486

    Elf32_Ehdr* file;
    Elf32_Phdr* phdr;
    uint16_t phdrCount;

    struct link_map* lm = (struct link_map*)ulModule;
    ulBase = reinterpret_cast<uintptr_t>(lm->l_addr);
    file = reinterpret_cast<Elf32_Ehdr*>(ulBase);

    /* Check ELF magic */
    if (memcmp(ELFMAG, file->e_ident, SELFMAG) != 0)
    {
      Msg("ELF magic check failed.");
      return nullptr;
    }

    /* Check ELF version */
    if (file->e_ident[EI_VERSION] != EV_CURRENT)
    {
      Msg("ELF version check failed.");
      return nullptr;
    }

    /* Check ELF architecture, which is 32-bit/x86 right now
     * Should change this for 64-bit if Valve gets their act together
     */
    if (file->e_ident[EI_CLASS] != ELFCLASS32 || file->e_machine != EM_386 || file->e_ident[EI_DATA] != ELFDATA2LSB)
    {
      Msg("ELF architecture check failed.");
      return nullptr;
    }

    /* For our purposes, this must be a dynamic library/shared object */
    if (file->e_type != ET_DYN)
    {
      Msg("Library is not a dynamic or shared object.");
      return nullptr;
    }

    phdrCount = file->e_phnum;
    phdr = reinterpret_cast<Elf32_Phdr*>(ulBase + file->e_phoff);

    for (uint16_t i = 0; i < phdrCount; i++)
    {
      Elf32_Phdr& hdr = phdr[i];

      /* We only really care about the segment with executable code */
      if (hdr.p_type == PT_LOAD && hdr.p_flags == (PF_X | PF_R))
      {
        /* From glibc, elf/dl-load.c:
         * c->mapend = ((ph->p_vaddr + ph->p_filesz + GLRO(dl_pagesize) - 1)
         *             & ~(GLRO(dl_pagesize) - 1));
         *
         * In glibc, the segment file size is aligned up to the nearest page size and
         * added to the virtual address of the segment. We just want the size here.
         */
        ulSize = PAGE_ALIGN_UP(hdr.p_filesz);
        break;
      }
    }
#else
#error "BinaryManager::FindBinary() is not implemented on this OS"
#endif

    // Create a new Binary object and add it to the list
    CBinaryFile* binary = new CBinaryFile(ulModule, ulBase, ulSize);
    m_Binaries.push_front(binary);
    return binary;
  }

}
