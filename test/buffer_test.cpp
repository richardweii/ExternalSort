//
// Created by Administrator on 2022/4/22.
//

#include "io/buffer.h"
#include <gtest/gtest.h>
#include "foo.h"

using namespace extsort;

TEST(IOBUFFER, Write) {
  Foo a = {1, 5, 10.2};
  auto buffer = io::CreateBuffer(1024);
  ASSERT_EQ(buffer->BufferSize(), 1024);
  ASSERT_TRUE(buffer->Write(0, a).HasValue());
  auto rd = buffer->Read<Foo>(0);
  ASSERT_TRUE(rd.HasValue());
  auto val = rd.Value();
  EXPECT_EQ(val.a, a.a);
  EXPECT_EQ(val.b, a.b);
  EXPECT_EQ(val.c, a.c);
}

TEST(IOBUFFER, SeqentialRead) {
  auto buffer = io::CreateBuffer(1024);
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
    auto ret = buffer->Write(pos, foo);
    EXPECT_TRUE(ret.HasValue());
    pos = ret.Value();
  }

  // read
  int num = 0;
  std::size_t start = 0;
  while (start < pos) {
    auto rd = buffer->Read<Foo>(start);
    EXPECT_TRUE(rd.HasValue());
    EXPECT_EQ(foos[num], rd.Value());
    start += sizeof(rd.Value());
    num++;
  }
  EXPECT_EQ(num, sizeof(foos) / sizeof(Foo));
}

TEST(IOBUFFER, MultiRead) {
  auto buffer = io::CreateBuffer(1024);
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
    auto ret = buffer->Write(pos, foo);
    EXPECT_TRUE(ret.HasValue());
    pos = ret.Value();
  }

  // read
  int num = sizeof(foos) / sizeof(Foo);
  auto rd = buffer->Read<Foo>(0, num);
  EXPECT_TRUE(rd.HasValue());
  auto &res = rd.Value();
  for (int i = 0; i < num; i++) {
    EXPECT_EQ(res[i], foos[i]);
  }
}