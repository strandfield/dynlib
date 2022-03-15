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

#else

static std::string add_lib_prefix(std::string libpath)
{
  size_t offset = libpath.rfind("/");

  if (offset == std::string::npos)
    return "lib" + libpath;

  return libpath.substr(0, offset + 1) + "lib" + libpath.substr(offset + 1);
}

static void replace_all(std::string& inout, const std::string& what, const std::string& with)
{
  for (std::string::size_type pos(0); inout.npos != (pos = inout.find(what, pos)); pos += with.length())
  {
    inout.replace(pos, what.length(), with.data(), with.length());
  }
}

#endif // defined(OS_WINDOWS)


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
    replace_all(libname, "//", "/");
#endif
  }
};

Library::Library(Library&& other)
  : d(std::move(other.d))
{
}

Library::~Library()
{
 
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

  if (!d->loaded)
  {
    DWORD error = GetLastError();
    char buf[256];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      buf, (sizeof(buf) / sizeof(char)), NULL);
    d->error_string = buf;
  }

#else

  d->system_library = dlopen(d->libname.c_str(), RTLD_LAZY);
  
  if (d->system_library == nullptr)
  {
    std::string libpath = add_lib_prefix(d->libname);
    d->system_library = dlopen(libpath.c_str(), RTLD_LAZY);
  }

  d->loaded = (d->system_library != nullptr);

  if(!d->loaded)
  {
      char* err = dlerror();

      if(err)
      {
          d->error_string = err;
      }
  }

#endif

  return d->loaded;
}

bool Library::isLoaded() const
{
  return d->loaded;
}

void Library::unload()
{
  if (!d)
    return;

#if defined(OS_WINDOWS)
  if (d->loaded)
  {
    FreeLibrary(d->system_library);
    d->system_library = nullptr;
  }
#else
  if (d->loaded)
  {
    dlclose(d->system_library);
    d->system_library = nullptr;
  }
#endif
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
