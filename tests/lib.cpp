// Copyright (C) 2020 Vincent Chambrin
// This file is part of the dynlib library
// For conditions of distribution and use, see copyright notice in LICENSE

#if (defined(WIN32) || defined(_WIN32))
#  define DYNLIB_API __declspec(dllexport)
#else
#define DYNLIB_API
#endif

#include <iostream>

void cpp_hello()
{
  std::cout << "Hello World!" << std::endl;
}

extern "C"
{

  DYNLIB_API void say_hello()
  {
    cpp_hello();
  }

  DYNLIB_API int double_it(int n)
  {
    return 2 * n;
  }

} // extern "C"
