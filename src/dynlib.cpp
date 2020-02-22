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

#if defined(OS_WINDOWS)
static void replace_slashes(std::string& str)
{
  for (char& c : str)
  {
    if (c == '/')
      c = '\\';
  }
}
#endif

struct LibraryImpl
{
  std::string libname;
  bool loaded = false;
  std::string error_string;

#if defined(OS_WINDOWS)
  HMODULE system_library = nullptr;
#else
  void* system_library = nullptr;
#endif // defined(OS_WINDOWS)

public:
  LibraryImpl(std::string filename)
    : libname(std::move(filename))
  {
#if defined(OS_WINDOWS)
    replace_slashes(libname);
#else
    libname.append(".so");
#endif
  }
};

Library::Library(Library&& other)
  : d(std::move(other.d))
{
}

Library::~Library()
{
  if (!d)
    return;

#if defined(OS_WINDOWS)
  if (d->loaded)
  {
    FreeLibrary(d->system_library);
  }
#else
  if (d->loaded)
  {
    dlclose(d->system_library);
    d->system_library = nullptr;
  }
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
  d->system_library = dlopen(d->libname.c_str(), RTLD_LAZY);
  d->loaded = (d->system_library != nullptr);

  if(!d->loaded)
  {
      char* err = dlerror();

      if(err)
      {
          d->error_string = err;
          delete err;
      }
  }
#endif

  return d->loaded;
}

bool Library::isLoaded() const
{
  return d->loaded;
}

const std::string& Library::errorString() const
{
  return d->error_string;
}

FunctionPointer Library::resolve(const char* sym)
{
  if (!load())
    return nullptr;

#if defined(OS_WINDOWS)
  return (FunctionPointer)GetProcAddress(d->system_library, sym);
#else
  return (FunctionPointer)dlsym(d->system_library, sym);
#endif
}

Library& Library::operator=(Library&& other)
{
  d = std::move(other.d);

  return *this;
}

} // namespace dynlib
