// Copyright (C) 2020 Vincent Chambrin
// This file is part of the dynlib library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dynlib/dynlib.h"

#include <iostream>

int main()
{
  dynlib::Library lib{ "testlib" };

  if (!lib.load())
  {
      std::cout << lib.errorString() << std::endl;
      return 1;
  }

  dynlib::FunctionPointer say_hello = lib.resolve("say_hello");

  if (!say_hello)
    return 2;

  say_hello();

  using Fn = int(*)(int);

  Fn double_it = (Fn)lib.resolve("double_it");

  if (!double_it)
    return 3;

  int n = double_it(33);

  if (n != 66)
    return 4;

  return 0;
}
