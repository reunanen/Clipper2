#include <gtest/gtest.h>
#include "clipper2/clipper.h"
#include "ClipFileLoad.h"
#include <windows.h>
#include <random>
#include <numeric>

inline Clipper2Lib::PathD MakeRandomPath(int width, int height, unsigned vertCnt)
{
  Clipper2Lib::PathD result;
  result.reserve(vertCnt);
  for (unsigned i = 0; i < vertCnt; ++i)
    result.push_back(Clipper2Lib::PointD(double(rand()) / RAND_MAX * width, double(rand()) / RAND_MAX * height));
  return result;
}

template <size_t N>
inline bool IsInList(int num, const int (&intArray)[N])
{  
  const int* list = &intArray[0];
  for (int cnt = N; cnt; --cnt)
    if (num == *list++) return true;
  return false;
}

int filterException(int code, PEXCEPTION_POINTERS ex) {
    std::cout << "Filtering " << std::hex << code << std::endl;
    return EXCEPTION_EXECUTE_HANDLER;
}

TEST(Clipper2Tests, TestMultiplePolygons)
{
  std::ifstream ifs("Polygons.txt");


  ASSERT_TRUE(ifs);
  ASSERT_TRUE(ifs.good());

  int test_number = 1;
  while (true)
  {
    Clipper2Lib::Paths64 subject, subject_open, clip;
    Clipper2Lib::ClipType ct;
    Clipper2Lib::FillRule fr;
    int64_t stored_area, stored_count;

    if (!LoadTestNum(ifs, test_number, 
      subject, subject_open, clip, stored_area, stored_count, ct, fr)) break;

    bool any_changes = false;
    int round = 0;

    const auto test = [&subject, ct, fr](const std::set<size_t>& indexes) {
        Clipper2Lib::Paths64 cc;
        Clipper2Lib::Paths64 solution, solution_open;

        for (auto ii : indexes) {
            cc.push_back(subject[ii]);
        }

        Clipper2Lib::Clipper64 c;
        c.AddSubject(cc);

        try {
            c.Execute(ct, fr, solution, solution_open);
            //std::cout << ":";
            return true;
        }
        catch (std::exception&) {
            //std::cout << ".";
            return false;
        }
    };

    std::set<size_t> remaining_indexes;
    for (size_t i = 0; i < subject.size(); ++i) {
        remaining_indexes.insert(i);
    }

    do {
        any_changes = false;

        std::cout << "ROUND: " << round++ << ", remaining: " << remaining_indexes.size() << std::endl;

        std::cout << "Erasing:";

        std::vector<size_t> rr(remaining_indexes.rbegin(), remaining_indexes.rend());

        for (size_t k : rr) {

            std::set<size_t> candidate;

            for (auto cc : remaining_indexes) {
                if (cc != k) {
                    candidate.insert(cc);
                }
            }

            if (!test(candidate)) {
                std::cout << " " << k;
                remaining_indexes.erase(k);
                any_changes = true;
            }
        }

        std::cout << std::endl;

#if 0
            const int64_t measured_area = static_cast<int64_t>(Area(solution));
            const int64_t measured_count = static_cast<int64_t>(solution.size() + solution_open.size());
            const int64_t count_diff = stored_count <= 0 ? 0 : std::abs(measured_count - stored_count);
            const int64_t area_diff = stored_area <= 0 ? 0 : std::abs(measured_area - stored_area);
            double area_diff_ratio = (area_diff == 0) ? 0 : std::fabs((double)(area_diff) / measured_area);

            // check the polytree variant too
            Clipper2Lib::PolyTree64 solution_polytree;
            Clipper2Lib::Paths64 solution_polytree_open;
            Clipper2Lib::Clipper64 clipper_polytree;
            clipper_polytree.AddSubject(subject);
            clipper_polytree.AddOpenSubject(subject_open);
            clipper_polytree.AddClip(clip);
            //clipper_polytree.Execute(ct, fr, solution_polytree, solution_polytree_open);

            const int64_t measured_area_polytree =
                static_cast<int64_t>(solution_polytree.Area());
            const auto solution_polytree_paths = PolyTreeToPaths64(solution_polytree);
            const int64_t measured_count_polytree =
                static_cast<int64_t>(solution_polytree_paths.size());

            // check polygon counts
            if (stored_count <= 0)
                ; // skip count
            else if (IsInList(test_number, { 120, 138, 140, 165, 166, 167, 168, 172, 175, 178, 180 }))
                EXPECT_LE(count_diff, 5) << " in test " << test_number;
            else if (IsInList(test_number, { 27, 126, 145, 163, 173, 176, 177, 179, 181 }))
                EXPECT_LE(count_diff, 2) << " in test " << test_number;
            else if (test_number > 119 && test_number < 184)
                EXPECT_LE(count_diff, 1) << " in test " << test_number;
            else if (IsInList(test_number, { 23, 87, 102, 111, 113, 191 }))
                EXPECT_LE(count_diff, 1) << " in test " << test_number;
            else
                EXPECT_EQ(count_diff, 0) << " in test " << test_number;

            // check polygon areas
            if (stored_area <= 0)
                ; // skip area
            else if (IsInList(test_number, { 19, 22, 23, 24 }))
                EXPECT_LE((double)area_diff_ratio, 0.5) << " in test " << test_number;
            else if (test_number == 193)
                EXPECT_LE((double)area_diff_ratio, 0.2) << " in test " << test_number;
            else if (test_number == 63)
                EXPECT_LE((double)area_diff_ratio, 0.1) << " in test " << test_number;
            else if (test_number == 16)
                EXPECT_LE((double)area_diff_ratio, 0.075) << " in test " << test_number;
            else if (test_number == 26)
                EXPECT_LE((double)area_diff_ratio, 0.05) << " in test " << test_number;
            else if (IsInList(test_number, { 15, 52, 53, 54, 59, 60, 64, 117, 119, 184 }))
                EXPECT_LE((double)area_diff_ratio, 0.02) << " in test " << test_number;
            else
                EXPECT_LE((double)area_diff_ratio, 0.01) << " in test " << test_number;

            EXPECT_EQ(measured_count, measured_count_polytree);
            EXPECT_EQ(measured_area, measured_area_polytree);
#endif
        //std::shuffle(remaining_indexes.begin(), remaining_indexes.end(), g);
    } while (any_changes);

    for (auto i : remaining_indexes) {
        const auto& path = subject[i];
        bool first = true;
        for (const auto& pp : path) {
            if (first) {
                first = false;
            }
            else {
                std::cout << " ";
            }
            std::cout << pp.x << "," << pp.y;
        }
        std::cout << std::endl;
    }

    ++test_number;
  }

#if 0
  EXPECT_GE(test_number, 188);

  Clipper2Lib::PathsD subjd, clipd, solutiond;
  Clipper2Lib::FillRule frd = Clipper2Lib::FillRule::NonZero;

  subjd.push_back(MakeRandomPath(800, 600, 100));
  clipd.push_back(MakeRandomPath(800, 600, 100));
  solutiond = Clipper2Lib::Intersect(subjd, clipd, Clipper2Lib::FillRule::NonZero);
  EXPECT_GE(solutiond.size(), 1);
#endif
}