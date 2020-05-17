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

TEST(ClockReplacerTest, MyConcurrencyTest1) {
  int num_pages = 500;
  // frame_id范围为[0,num_pages);

  ClockReplacer clock_replacer(num_pages);

  // invariable broken case 1: 在clock_replacer.Size()计算Size()执行过程中，
  // 可能其size已经增加，从而导致Size()返回一个不恰当的值
  int myExist=0;
  // 保护myExist的mtx
  std::mutex mtx;
  std::vector<std::thread> vs (num_pages/2);
  int vsi = 0;
  int n1 = 100;
  for (auto i=0; i<n1; i++) {
    if (i % 2 == 0) {
      EXPECT_EQ(myExist, clock_replacer.Size());
    } else {
      vs[vsi++] = std::thread([&](int x) {
        std::lock_guard<std::mutex> g (mtx);
        clock_replacer.Unpin(x);
        myExist ++;
      }, i);
    }
  }

  for (unsigned i=0; i<vs.size(); i++) {
    if (vs[i].joinable()) {
      vs[i].join();
    }
  }

}

TEST(ClockReplacerTest, MyConcurrencyTest2) {
  /* 无用，测不出来
  int num_pages = 500;
  // frame_id范围为[0,num_pages);

  ClockReplacer clock_replacer(num_pages);

  // invariable broken case 2: 在victim找到可迫害的frame后，在真正迫害这个frame之前，
  // 这个frame可能被Pin了。
  int n1 = 5000000;
  frame_id_t f_id;
  frame_id_t unpin_id = num_pages / 2;
  std::mutex mtx;
  for (auto i=0; i<n1; i++) {
    clock_replacer.Unpin(unpin_id);
    bool victim_start = false;
    bool victim_end = false;
    f_id = -1;
    std::thread t1([&]{
      victim_start = true;
      clock_replacer.Victim(&f_id);
      victim_end = true;
    });
    std::thread t2([&]{
      while (!victim_start);
      if (f_id != unpin_id) {
        clock_replacer.Pin(unpin_id);
        while (!victim_end);
        EXPECT_EQ(f_id != unpin_id, true);
      }
    });
    t1.join(); t2.join();
  }
   */
}

}  // namespace bustub
