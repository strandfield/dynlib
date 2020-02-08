// Copyright (C) 2020 Vincent Chambrin
// This file is part of the dynlib library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dynlib/dynlib.h"

#if (defined(WIN32) || defined(_WIN32))
#define OS_WINDOWS
#else
#define OS_LINUX
#endif

#if defined(OS_WINDOWS)
#include <windows.h>
#else
#include <dlfcn.h> 
#endif // defined(OS_WINDOWS)

namespace dynlib
{

static void replace_slashes(std::string& str)
{
  for (char& c : str)
  {
    if (c == '/')
      c = '\\';
  }
}

struct LibraryImpl
{
  std::string libname;
  bool loaded = false;

#if defined(OS_WINDOWS)
  HMODULE system_library = nullptr;
#endif // defined(OS_WINDOWS)

public:
  LibraryImpl(std::string filename)
    : libname(std::move(filename))
  {

  }
};

Library::~Library()
{
#if defined(OS_WINDOWS)
  if (d->loaded)
  {
    FreeLibrary(d->system_library);
  }
#else

#endif
}

Library::Library(const std::string& libname)
  : d(new LibraryImpl(libname))
{

}

bool Library::load()
{
  if (d->loaded)
    return true;

#if defined(OS_WINDOWS)
  d->system_library = LoadLibrary(d->libname.c_str());
  d->loaded = (d->system_library != nullptr);
#else
 
#endif

  return d->loaded;
}

bool Library::isLoaded() const
{
  return d->loaded;
}

FunctionPointer Library::resolve(const char* sym)
{
  if (!load())
    return nullptr;

#if defined(OS_WINDOWS)
  std::string str_sym = sym;
  replace_slashes(str_sym);

  return (FunctionPointer)GetProcAddress(d->system_library, str_sym.c_str());
#else
  return nullptr;
#endif
}

} // namespace dynlib
