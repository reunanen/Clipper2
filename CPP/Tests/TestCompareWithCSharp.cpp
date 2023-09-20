#include <gtest/gtest.h>
#include "clipper2/clipper.h"
#include "ClipFileLoad.h"

TEST(Clipper2Tests, TestCompareWithCSharp)
{
  std::ifstream ifs("CPP-vs-CS.txt");

  ASSERT_TRUE(ifs);
  ASSERT_TRUE(ifs.good());

  Clipper2Lib::Paths64 subject, subject_open, clip;
  Clipper2Lib::Paths64 solution, solution_open;
  Clipper2Lib::ClipType ct;
  Clipper2Lib::FillRule fr;
  int64_t stored_area, stored_count;

  ASSERT_TRUE(LoadTestNum(ifs, 1, subject, subject_open, clip, stored_area, stored_count, ct, fr));

  // check Paths64 solutions
  Clipper2Lib::Clipper64 c;
  c.AddSubject(subject);
  c.AddOpenSubject(subject_open);
  c.AddClip(clip);
  c.Execute(ct, fr, solution, solution_open);

  const auto measured_area = Area(solution);
  EXPECT_EQ(measured_area, 107099);
}