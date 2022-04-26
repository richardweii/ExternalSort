//
// Created by Administrator on 2022/4/25.
//

#include "io/two_level_iterator.h"
#include "io/buffer.h"
#include "io/file.h"
#include "foo.h"
#include <gtest/gtest.h>

using namespace std;
using namespace extsort;

TEST(TwoLevelIteratorTest, Basic) {
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
  auto iter = io::TwoLevelIterator<Foo>(filePtr);
  for (; iter.IsValid(); iter.Next()) {
    auto fooOpt = iter.Value();
    EXPECT_TRUE(fooOpt.HasValue());
    auto &&foo = fooOpt.Release();
    EXPECT_EQ(foo, foos[i]);
    i++;
  }
  EXPECT_EQ(iter.Count(), 100);
}

TEST(TwoLevelIteratorTest, Seek) {
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
  auto iter = io::TwoLevelIterator<Foo>(filePtr, 10 * sizeof(Foo));
  for (; iter.IsValid(); iter.Next()) {
    auto fooOpt = iter.Value();
    EXPECT_TRUE(fooOpt.HasValue());
    auto &&foo = fooOpt.Release();
    EXPECT_EQ(foo, foos[i]);
    i++;
  }
  EXPECT_EQ(iter.Count(), 90);

}