//
// Created by Administrator on 2022/4/24.
//

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "foo.h"
#include "io/file_iterator.h"
#include "io/buffer_iterator.h"

using namespace extsort;
using namespace std;

TEST(FileIteratorTest, Basic) {
  vector<Foo> foos;
  for (int i = 0; i < 100; i++) {
    foos.emplace_back(i, i * 2, i + 3 + i / 2);
  }

  auto filePtr = std::make_shared<io::File>();
  auto ret = filePtr->Append(reinterpret_cast<const char *>(&foos[0]), foos.size() * sizeof(Foo));
  EXPECT_TRUE(ret.HasValue());
  EXPECT_EQ(ret.Value(), foos.size() * sizeof(Foo));

  int i = 0;
  // iterate the file
  for (auto iter = io::FileIterator(filePtr); iter.IsValid(); iter.Next()) {
    auto val = iter.Value();
    EXPECT_TRUE(val.HasValue());
    auto buf = val.Release();
    // iterate the buffer
    for (auto fooIter = io::BufferIterator<Foo>(buf); fooIter.IsValid(); fooIter.Next()) {
      auto fooOpt = fooIter.Value();
      EXPECT_TRUE(fooOpt.HasValue());
      auto &&foo = fooOpt.Release();
      EXPECT_EQ(foo, foos[i]);
      i++;
    }
  }
}

TEST(FileIteratorTest, Seek) {
  vector<Foo> foos;
  for (int i = 0; i < 100; i++) {
    foos.emplace_back(i, i * 2, i + 3 + i / 2);
  }

  auto filePtr = std::make_shared<io::File>();
  auto ret = filePtr->Append(reinterpret_cast<const char *>(&foos[0]), foos.size() * sizeof(Foo));
  EXPECT_TRUE(ret.HasValue());
  EXPECT_EQ(ret.Value(), foos.size() * sizeof(Foo));

  int i = 10;
  // iterate the file
  auto iter = io::FileIterator(filePtr);
  for (iter.Seek(10 * sizeof(Foo)); iter.IsValid(); iter.Next()) {
    auto val = iter.Value();
    EXPECT_TRUE(val.HasValue());
    auto buf = val.Release();
    // iterate the buffer
    for (auto fooIter = io::BufferIterator<Foo>(buf); fooIter.IsValid(); fooIter.Next()) {
      auto fooOpt = fooIter.Value();
      EXPECT_TRUE(fooOpt.HasValue());
      auto &&foo = fooOpt.Release();
      EXPECT_EQ(foo, foos[i]);
      i++;
    }
  }
}

TEST(FileIteratorTest, ConstructSeek) {
  vector<Foo> foos;
  for (int i = 0; i < 100; i++) {
    foos.emplace_back(i, i * 2, i + 3 + i / 2);
  }

  auto filePtr = std::make_shared<io::File>();
  auto ret = filePtr->Append(reinterpret_cast<const char *>(&foos[0]), foos.size() * sizeof(Foo));
  EXPECT_TRUE(ret.HasValue());
  EXPECT_EQ(ret.Value(), foos.size() * sizeof(Foo));

  int i = 10;
  // iterate the file
  for (auto iter = io::FileIterator(filePtr, 10 * sizeof(Foo)); iter.IsValid(); iter.Next()) {
    auto val = iter.Value();
    EXPECT_TRUE(val.HasValue());
    auto buf = val.Release();
    // iterate the buffer
    for (auto fooIter = io::BufferIterator<Foo>(buf); fooIter.IsValid(); fooIter.Next()) {
      auto fooOpt = fooIter.Value();
      EXPECT_TRUE(fooOpt.HasValue());
      auto &&foo = fooOpt.Release();
      EXPECT_EQ(foo, foos[i]);
      i++;
    }
  }
}