//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer_test.cpp
//
// Identification: test/buffer/clock_replacer_test.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cstdio>
#include <thread>  // NOLINT
#include <vector>

#include "buffer/clock_replacer.h"
#include "gtest/gtest.h"

namespace bustub {
// DISABLED_SampleTest
TEST(ClockReplacerTest, SampleTest) {
  ClockReplacer clock_replacer(7);

  // Scenario: unpin six elements, i.e. add them to the replacer.
  clock_replacer.Unpin(1);
  clock_replacer.Unpin(2);
  clock_replacer.Unpin(3);
  clock_replacer.Unpin(4);
  clock_replacer.Unpin(5);
  clock_replacer.Unpin(6);
  clock_replacer.Unpin(1);
  EXPECT_EQ(6, clock_replacer.Size());

  // Scenario: get three victims from the clock.
  int value;
  clock_replacer.Victim(&value);
  EXPECT_EQ(1, value);
  clock_replacer.Victim(&value);
  EXPECT_EQ(2, value);
  clock_replacer.Victim(&value);
  EXPECT_EQ(3, value);

  // Scenario: pin elements in the replacer.
  // Note that 3 has already been victimized, so pinning 3 should have no effect.
  clock_replacer.Pin(3);
  clock_replacer.Pin(4);
  EXPECT_EQ(2, clock_replacer.Size());

  // Scenario: unpin 4. We expect that the reference bit of 4 will be set to 1.
  clock_replacer.Unpin(4);

  // Scenario: continue looking for victims. We expect these victims.
  clock_replacer.Victim(&value);
  EXPECT_EQ(5, value);
  clock_replacer.Victim(&value);
  EXPECT_EQ(6, value);
  clock_replacer.Victim(&value);
  EXPECT_EQ(4, value);
}

TEST(ClockReplacerTest, MyConcurrencyTest) {
  int num_pages = 500;
  // frame_id范围为[0,num_pages);

  ClockReplacer clock_replacer(num_pages);

  // invariable broken case 1: 在clock_replacer.Size()计算Size()执行过程中，
  // 可能其size已经增加，从而导致Size()返回一个不恰当的值
  int myExist=0;
  // 保护myExist的mtx
  std::mutex mtx;
  for (auto i=0; i<num_pages; i++) {
    if (i % 2 == 0) {
      EXPECT_EQ(myExist, clock_replacer.Size());
    } else {
      std::thread t1([&](int x) {
        std::lock_guard<std::mutex> g (mtx);
        clock_replacer.Unpin(x);
        myExist ++;
      }, i);
      t1.detach();
    }
  }

}
}  // namespace bustub
