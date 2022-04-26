//
// Created by Administrator on 2022/4/25.
//

#include "sort.h"
#include <random>
#include <algorithm>
#include <gtest/gtest.h>

struct Entry {
  int a;
  float b;
  double c;
  Entry(int a, float b, double c) : a(a), b(b), c(c) {}
  Entry(const Entry &e) = default;
  Entry(Entry &&e) = default;
  Entry &operator=(Entry &&e) = default;
  Entry &operator=(const Entry &e) = default;
  bool operator==(const Entry &e) const { return a == e.a && b == e.b && c == e.c; }
  void print() const {
    spdlog::info("Entry [a: {}, b:{}, c:{}]", a, b, c);
  }
};

using namespace std;
using namespace extsort;

TEST(ExternalSort, Basic) {
  vector<Entry> entries;
  for (int i = 0; i < 100; i++) {
    entries.emplace_back(i, i * 2, i + 3 + i / 2);
  }

  // shuffle
  std::random_device rd;
  std::mt19937 rng(rd());
  std::shuffle(entries.begin(), entries.end(), rng);
  spdlog::info("sizeof(Entry) is {}", sizeof(Entry));
  // create a file
  auto filePtr = std::make_shared<io::File>();
  auto retApd = filePtr->Append(reinterpret_cast<const char *>(&entries[0]), entries.size() * sizeof(Entry));
  EXPECT_TRUE(retApd.HasValue());
  EXPECT_EQ(retApd.Value(), entries.size() * sizeof(Entry));

  // external sort
  ExternalSort<Entry> sort(filePtr);
  auto cmp = [](const Entry &e1, const Entry &e2) {
    return (e1.a < e2.a) || (e1.a == e2.a && e1.b < e2.b) || (e1.a == e2.a && e1.b == e2.b && e1.c < e2.c);
  };
  auto retSort = sort(cmp);
  EXPECT_TRUE(retSort.HasValue());

  // sort
  std::sort(entries.begin(), entries.end(), cmp);

  // validate
  int i = 0;
  auto iter = io::TwoLevelIterator<Entry>(filePtr);
  for (; iter.IsValid(); iter.Next()) {
    auto entryOpt = iter.Value();
    EXPECT_TRUE(entryOpt.HasValue());
    auto &&entry = entryOpt.Release();
    entry.print();
    EXPECT_EQ(entry, entries[i]);
    i++;
  }
  EXPECT_EQ(iter.Count(), 100);
}