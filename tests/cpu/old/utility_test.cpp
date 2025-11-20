#include "gtest/gtest.h"
#include "linalg/ldlt_solver.h"
#include "linalg/shift_registers.h"

TEST(ShiftRegisterTest, BasicShift)
{
    ShiftRegister<int, 3> sr;
    // Initially, the register is {0, 0, 0}
    EXPECT_EQ(sr.ShiftDown(10), 0);  // Returns 0, state becomes {0, 0, 10}
    EXPECT_EQ(sr.ShiftDown(20), 0);  // Returns 0, state becomes {0, 10, 20}
    EXPECT_EQ(sr.ShiftDown(30), 0);  // Returns 0, state becomes {10, 20, 30}
    EXPECT_EQ(sr.ShiftDown(40), 10); // Returns 10, state becomes {20, 30, 40}
    EXPECT_EQ(sr.ShiftDown(50), 20); // Returns 20, state becomes {30, 40, 50}
}

TEST(ShiftMat2Test, WindowGeneration)
{
    const int width = 5;
    ShiftMat2<int, width> sm;

    // Create a dummy image stream
    int stream_len = 2 * width;
    int image_stream[stream_len];
    for (int i = 0; i < stream_len; ++i)
    {
        image_stream[i] = i + 1;
    }

    // Feed 1*width + 2 pixels to get the first 2x2 window
    int num_pixels_to_feed = 1 * width + 2;
    for (int i = 0; i < num_pixels_to_feed; ++i)
    {
        sm.ShiftDown(image_stream[i]);
    }

    linalg::Mat2<int> mat = sm.GetMat();

    // Expected matrix (window ending at pixel 7)
    // 1 2
    // 6 7
    EXPECT_EQ(mat(0, 0), 1);
    EXPECT_EQ(mat(0, 1), 2);
    EXPECT_EQ(mat(1, 0), 6);
    EXPECT_EQ(mat(1, 1), 7);
}

TEST(ShiftMat3Test, WindowGeneration)
{
    const int width = 5;
    ShiftMat3<int, width> sm;

    // Create a dummy image stream
    int stream_len = 3 * width;
    int image_stream[stream_len];
    for (int i = 0; i < stream_len; ++i)
    {
        image_stream[i] = i + 1;
    }

    // Feed 2*width + 3 pixels to get the first full 3x3 window
    int num_pixels_to_feed = 2 * width + 3;
    for (int i = 0; i < num_pixels_to_feed; ++i)
    {
        sm.ShiftDown(image_stream[i]);
    }

    linalg::Mat3<int> mat = sm.GetMat();

    // Expected matrix (window ending at pixel 13)
    // 1  2  3
    // 6  7  8
    // 11 12 13
    EXPECT_EQ(mat(0, 0), 1);
    EXPECT_EQ(mat(0, 1), 2);
    EXPECT_EQ(mat(0, 2), 3);
    EXPECT_EQ(mat(1, 0), 6);
    EXPECT_EQ(mat(1, 1), 7);
    EXPECT_EQ(mat(1, 2), 8);
    EXPECT_EQ(mat(2, 0), 11);
    EXPECT_EQ(mat(2, 1), 12);
    EXPECT_EQ(mat(2, 2), 13);
}

TEST(LDLTSolverTest, Solve3x3)
{
    using Type = double;
    const int size = 3;

    linalg::LDLT<Type, size> solver;
    linalg::Mat<Type, size, size> A;
    A(0, 0) = 4;
    A(0, 1) = 12;
    A(0, 2) = -16;
    A(1, 0) = 12;
    A(1, 1) = 37;
    A(1, 2) = -43;
    A(2, 0) = -16;
    A(2, 1) = -43;
    A(2, 2) = 98;

    linalg::Mat<Type, size, 1> b;
    b(0, 0) = 0;
    b(1, 0) = 6;
    b(2, 0) = 39;

    solver.compute(A);
    linalg::Mat<Type, size, 1> x = solver.solve(b);

    EXPECT_NEAR(x(0, 0), 1.0, 1e-9);
    EXPECT_NEAR(x(1, 0), 1.0, 1e-9);
    EXPECT_NEAR(x(2, 0), 1.0, 1e-9);
}