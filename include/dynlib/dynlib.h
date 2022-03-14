// Copyright (C) 2020 Vincent Chambrin
// This file is part of the dynlib library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DYNLIB_DYNLIB_H
#define DYNLIB_DYNLIB_H

#include <memory>
#include <string>

#if (defined(WIN32) || defined(_WIN32)) && !defined(DYNLIB_STATIC_LINKING)
#if defined(DYNLIB_COMPILE_LIBRARY)
#  define DYNLIB_API __declspec(dllexport)
#else
#  define DYNLIB_API __declspec(dllimport)
#endif
#else
#define DYNLIB_API
#endif

namespace dynlib
{

struct LibraryImpl;

using FunctionPointer = void(*)();

class DYNLIB_API Library
{
public:
  Library() = delete;
  Library(const Library&) = delete;
  Library(Library&&);
  ~Library();

  explicit Library(const std::string& libname);

  bool load();
  bool isLoaded() const;
  void unload();

  const std::string& errorString() const;

  FunctionPointer resolve(const char* sym);

  Library& operator=(const Library&) = delete;
  Library& operator=(Library&&);

private:
  std::unique_ptr<LibraryImpl> d;
};

} // namespace dynlib
  
#endif // DYNLIB_DYNLIB_H
