//
// Created by Administrator on 2022/4/23.
//
#include <gtest/gtest.h>
#include "optional.h"
#include "foo.h"

using namespace extsort;

TEST(Optional, OptionalMoveObject) {
  Optional<Foo> f;
  EXPECT_FALSE(f.HasValue());
  auto foo = Foo(1, 2, 3);
  auto addr = foo.data;

  Optional<Foo> f1(foo);
  EXPECT_TRUE(f1.HasValue());
  EXPECT_NE(f1.Value().data, addr);

  Optional<Foo> f2(std::move(foo));
  EXPECT_TRUE(f2.HasValue());
  EXPECT_EQ(f2.Value().data, addr);
  EXPECT_EQ(foo.data, nullptr); // moved
}

TEST(Optional, OptionalMoveConstruction) {
  Optional<Foo> opt1(1, 2, 3); // construction
  EXPECT_TRUE(opt1.HasValue());
  auto cnt = *opt1.Value().data;
  auto addr = opt1.Value().data;

  auto f = std::move(opt1); // moved construction
  EXPECT_TRUE(f.HasValue());
  EXPECT_FALSE(opt1.HasValue());  // moved
  EXPECT_EQ(f.Value().data, addr);
  EXPECT_EQ(*f.Value().data, cnt);
}

TEST(Optional, OptionalReturnMovedValue) {
  Optional<Foo> opt1(1, 2, 3); // construction
  EXPECT_TRUE(opt1.HasValue());
  auto cnt = *opt1.Value().data;
  auto addr = opt1.Value().data;

  auto val1 = opt1.Value(); // copy construction
  EXPECT_TRUE(opt1.HasValue());
  EXPECT_NE(val1.data, addr);
  EXPECT_NE(*val1.data, cnt);

  auto val2 = opt1.Release(); // move construction
  EXPECT_FALSE(opt1.HasValue());
  EXPECT_EQ(val2.data, addr);
  EXPECT_EQ(*val2.data, cnt);
}

TEST(Optional, OptionalInvalidValue) {
  Optional<Foo> opt1(1, 2, 3); // construction
  EXPECT_TRUE(opt1.HasValue());

  auto foo = opt1.Release(); // move construction
  EXPECT_FALSE(opt1.HasValue());
  EXPECT_THROW(opt1.Value(), std::runtime_error);
  EXPECT_THROW(opt1.Release(), std::runtime_error);

  Optional<Foo> empty;
  EXPECT_FALSE(empty.HasValue());
  EXPECT_THROW(empty.Value(), std::runtime_error);
  EXPECT_THROW(empty.Release(), std::runtime_error);
}