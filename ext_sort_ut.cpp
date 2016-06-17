#include "gtest/gtest.h"
#include "sorter.h"

using namespace std;

TEST(Basic, ArithmeticInMem) {
    vector<int> canon;
    ext_sort::Sorter<int, ext_sort::ArithmeticTraits<int>> sorter(1024, "./ut");

    for (size_t i = 0; i < 100; ++i) {
        canon.push_back(i % 13);
        sorter.Add(i % 13);
    }

    size_t idx = 0;
    sort(canon.begin(), canon.end());
    for (auto val: sorter) {
        //~ cerr << val << endl;
        EXPECT_EQ(val, canon[idx]);
        ++idx;
    }

    EXPECT_EQ(idx, canon.size());
}

TEST(Basic, ArithmeticOffMem) {
    vector<int> canon;
    ext_sort::Sorter<int, ext_sort::ArithmeticTraits<int>> sorter(1024, "./ut");

    for (size_t i = 0; i < 100000; ++i) {
        canon.push_back(i % 13);
        sorter.Add(i % 13);
    }
    sort(canon.begin(), canon.end());

    size_t idx = 0;
    for (auto val: sorter) {
        //~ cerr << val << endl;
        EXPECT_EQ(val, canon[idx]);
        ++idx;
    }

    EXPECT_EQ(idx, canon.size());
}

TEST(Basic, RestartOffMem) {
    vector<int> canon;
    ext_sort::Sorter<int, ext_sort::ArithmeticTraits<int>> sorter(1024, "./ut");

    for (size_t i = 0; i < 100000; ++i) {
        canon.push_back(i % 13);
        sorter.Add(i % 13);
    }
    sort(canon.begin(), canon.end());

    size_t idx = 0;
    for (auto val: sorter) {
        //~ cerr << val << endl;
        EXPECT_EQ(val, canon[idx]);
        ++idx;
    }

    EXPECT_EQ(idx, canon.size());

    idx = 0;
    for (auto val: sorter) {
        //~ cerr << val << endl;
        EXPECT_EQ(val, canon[idx]);
        ++idx;
    }

    EXPECT_EQ(idx, canon.size());
}
