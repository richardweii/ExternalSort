//
// Created by Administrator on 2022/4/23.
//

#include "io/buffer_iterator.h"
#include "io/buffer.h"
#include <gtest/gtest.h>
#include "foo.h"

using namespace extsort;
TEST(BufferIter, Basic) {
  auto buffer_ptr = io::CreateBuffer(1024);
  Foo foos[] = {
      {1, 2, 3},
      {2, 3, 4},
      {3, 4, 10},
      {2, 2, 2},
      {42, 32, 23},
      {12, 22, 3},
      {12, 23, 4}
  };

  // write
  std::size_t pos = 0;
  for (auto &foo : foos) {
//    std::cout << &foo << std::endl;
    auto ret = buffer_ptr->Write(pos, foo);
    EXPECT_TRUE(ret.HasValue());
    pos = ret.Value();
  }

  io::BufferIterator<Foo> iter(buffer_ptr);

  // iterator
  int i = 0;
  while (iter.IsValid()) {
    auto val = iter.Value();
    EXPECT_TRUE(val.HasValue());
    EXPECT_EQ(val.Value(), foos[i]);
    iter.Next();
    i++;
  }
}