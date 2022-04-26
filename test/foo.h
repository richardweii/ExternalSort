//
// Created by Administrator on 2022/4/23.
//

#ifndef EXTSORT_TEST_FOO_H
#define EXTSORT_TEST_FOO_H
#include <iostream>
struct Foo {
  int a;
  long b;
  float c;
  char *data;
  ~Foo() {
    delete data;
  }
  Foo(Foo &&f) noexcept {
    std::cout << "Foo&& called" << std::endl;
    a = f.a;
    b = f.b;
    c = f.c;
    data = f.data;
    f.data = nullptr;
  }

  Foo(const Foo &f) {
    std::cout << "cont Foo& called" << std::endl;
    a = f.a;
    b = f.b;
    c = f.c;
    data = new char;
    *data = *f.data + 1;
  }

  Foo(int a, int b, float c) : a(a), b(b), c(c), data(new char) {
    *data = a + b + c;
  }

  Foo &operator=(const Foo &f) {
    if (&f == this) {
      return *this;
    }

    std::cout << "operator= Foo& called" << std::endl;
    a = f.a;
    b = f.b;
    c = f.c;
    data = new char;
    *data = *f.data + 1;
    return *this;
  }

  Foo &operator=(Foo &&f) noexcept {
    std::cout << "operator= Foo&& called" << std::endl;
    a = f.a;
    b = f.b;
    c = f.c;
    data = f.data;
    f.data = nullptr;
    return *this;
  }

  bool operator==(const Foo &f) const {
    return a == f.a && b == f.b && c == f.c && data == f.data && *data == *f.data;
  }

};
#endif //EXTSORT_TEST_FOO_H
