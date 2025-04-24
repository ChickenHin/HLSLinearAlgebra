#pragma once

#include "linalgHLS.h"

template <typename type, int size>
class ShiftRegister
{
public:
    ShiftRegister()
    {
    }

    type ShiftDown(type val_in)
    {
        // #pragma HLS INLINE

        type val_out = data[0];

    shift_down_loop:
        for (int i = 0; i < size - 1; i++)
        {
            // #pragma HLS UNROLL

            data[i] = data[i + 1];
        }

        data[size - 1] = val_in;

        return val_out;
    }

private:
    type data[size];
};

template <typename type, int size>
class ShiftMat3
{
public:
    ShiftMat3()
    {
    }

    void ShiftDown(type val)
    {
        type m20 = matrix(2, 0);
        matrix(2, 0) = matrix(2, 1);
        matrix(2, 1) = matrix(2, 2);
        matrix(2, 2) = val;

        type line2_val = line2.ShiftDown(m20);

        type m10 = matrix(1, 0);
        matrix(1, 0) = matrix(1, 1);
        matrix(1, 1) = matrix(1, 2);
        matrix(1, 2) = line2_val;

        type line1_val = line1.ShiftDown(m10);

        // type m00 = matrix(0, 0);
        matrix(0, 0) = matrix(0, 1);
        matrix(0, 1) = matrix(0, 2);
        matrix(0, 2) = line1_val;
    }

    linalgHLS::Mat3<type> GetMat()
    {
        return matrix;
    }

private:
    ShiftRegister<type, size - 3> line1;
    ShiftRegister<type, size - 3> line2;
    linalgHLS::Mat3<type> matrix;
};

template <typename type, int size>
class ShiftMat2
{
public:
    ShiftMat2()
    {
    }

    void ShiftDown(type val)
    {
        type m10 = matrix(1, 0);
        matrix(1, 0) = matrix(1, 1);
        matrix(1, 1) = val;

        type line_val = line.ShiftDown(m10);

        // type m10 = matrix(1, 0);
        matrix(0, 0) = matrix(0, 1);
        matrix(0, 1) = line_val;
    }

    linalgHLS::Mat2<type> GetMat()
    {
        return matrix;
    }

private:
    ShiftRegister<type, size - 2> line;
    linalgHLS::Mat2<type> matrix;
};