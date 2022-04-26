//
// Created by Administrator on 2022/4/24.
//
#include <gtest/gtest.h>
#include "io/file.h"
#include "io/buffer_iterator.h"
#include "io/buffer.h"
#include "foo.h"

using namespace extsort;

TEST(FileTest, WriteAndRead) {
  auto file = io::File();
  EXPECT_EQ(file.Size(), 0);
  Foo foos[] = {
      {1, 2, 3},
      {2, 3, 4},
      {3, 4, 10},
      {2, 2, 2},
      {42, 32, 23},
      {12, 22, 3},
      {12, 23, 4}
  };
  file.Append(reinterpret_cast<const char *>(&foos), sizeof(foos));
  EXPECT_EQ(file.Size(), sizeof(foos));

  auto buf = io::CreateBuffer(kBufferSize * 2);
  auto ret = file.Read(0, buf, buf->BufferSize());
  EXPECT_TRUE(ret.HasValue());
  EXPECT_EQ(ret.Value(), sizeof(foos));
  int i = 0;
  for (auto iter = io::BufferIterator<Foo>(std::move(buf)); iter.IsValid(); iter.Next()) {
    auto val = iter.Value();
    EXPECT_TRUE(val.HasValue());
    auto &&got = val.Release();
    EXPECT_EQ(got, foos[i]);
    i++;
  }
}

TEST(FileTest, WriteBuffer) {
  Foo foos[] = {
      {1, 2, 3},
      {2, 3, 4},
      {3, 4, 10},
      {2, 2, 2},
      {42, 32, 23},
      {12, 22, 3},
      {12, 23, 4}
  };

  // fill a buffer
  auto writeBuf = io::CreateBuffer(kBufferSize * 2);
  auto ret = writeBuf->Write(0, reinterpret_cast<const char *>(&foos), sizeof(foos));
  EXPECT_TRUE(ret.HasValue());
  EXPECT_EQ(ret.Value(), sizeof(foos));

  // write the buffer to file
  auto file = io::File();
  EXPECT_EQ(file.Size(), 0);
  ret = file.AppendBuffer(writeBuf, 0, writeBuf->Size());
  EXPECT_TRUE(ret.HasValue());
  EXPECT_EQ(ret.Value(), sizeof(foos));

  // read from file
  auto readBuf = io::CreateBuffer(kBufferSize * 2);
  ret = file.Read(0, readBuf, sizeof(Foo) * 5);
  EXPECT_TRUE(ret.HasValue());
  EXPECT_EQ(ret.Value(), sizeof(Foo) * 5);

  // validate
  int i = 0;
  for (auto iter = io::BufferIterator<Foo>(std::move(readBuf)); iter.IsValid(); iter.Next()) {
    auto val = iter.Value();
    EXPECT_TRUE(val.HasValue());
    auto &&got = val.Release();
    EXPECT_EQ(got, foos[i]);
    i++;
  }
}