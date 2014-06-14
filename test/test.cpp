#include <iostream>
#include "gtest/gtest.h"

TEST(SquareRootTest, PositiveNos) { 
        EXPECT_EQ (18.0, 18.0);
}

TEST (SquareRootTest, ZeroAndNegativeNos) { 
        ASSERT_EQ (0.0, 0);
}


GTEST_API_ int main(int argc, char **argv) {
      std::cout << "Running main() from testmain.cc\n";
       
        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}
