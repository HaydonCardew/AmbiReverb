/*
  ==============================================================================

    Matrix.cpp
    Created: 18 Jun 2021 12:15:12pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "Matrix.h"
#include <iostream>

HC::Matrix::Matrix (int rows, int columns)
{
    assert(rows > 0 && columns > 0);
    resize(rows, vector<float>(columns, 0));
}

HC::Matrix HC::Matrix::getTranspose() const
{
    Matrix transpose(numColumns(), numRows());
    for (int row = 0; row < numRows(); ++row)
    {
        for (int column = 0; column < numColumns(); ++column)
        {
            transpose[column][row] = (*this)[row][column];
        }
    }
    return transpose;
}

float HC::Matrix::getCofactor(int row, int column) const
{
    Matrix cofactor(*this);
    cofactor.erase(cofactor.begin() + row);
    for (auto & col : cofactor)
    {
        col.erase(col.begin() + column);
    }
    return cofactor.getDeterminant();
}

HC::Matrix HC::Matrix::getCofactorMatrix() const
{
    Matrix cofactor(numRows(), numColumns());
    for (int row = 0; row < numRows(); ++row)
    {
        for (int column = 0; column < numColumns(); ++column)
        {
            cofactor[row][column] = getCofactor(row, column);
            if ( (row+column) % 2 == 1)
            {
                cofactor[row][column] *= -1;
            }
        }
    }
    return cofactor;
}

HC::Matrix HC::Matrix::getAdjoint() const
{
    return getCofactorMatrix().getTranspose();
}

HC::Matrix HC::Matrix::getInverse() const
{
    return getAdjoint().multiply(1/(*this).getDeterminant());
}

HC::Matrix HC::Matrix::multiply(float scalar) const
{
    Matrix output(*this);
    for (int row = 0; row < output.numRows(); ++row)
    {
        for (int column = 0; column < output.numColumns(); ++column)
        {
            output[row][column] *= scalar;
        }
    }
    return output;
}

void HC::Matrix::multiply(const Matrix& input, Matrix& output) const
{
    assert(numRows() == input.numColumns());
    //assert(numColumns() == input.numRows());
    assert(output.numRows() >= numRows() && output.numColumns() >= input.numColumns());
    for (int row = 0; row < numRows(); ++row)
    {
        for (int column = 0; column < input.numColumns(); ++column)
        {
            for (int i = 0; i < input.numRows(); ++i)
            {
                output[row][column] += (*this)[row][i] * input[i][column];
            }
        }
    }
}

HC::Matrix HC::Matrix::multiply(const Matrix& input) const
{
    Matrix output(numRows(), input.numColumns());
    multiply(input, output);
    return output;
}

HC::Matrix HC::Matrix::getPseudoInverse() const
{
    if (numRows() == numColumns())
    {
        return getInverse();
    }
    Matrix square(multiply(getTranspose()));
    return getTranspose().multiply(square.getInverse());
};

HC::Matrix::Row HC::Matrix::getRow(int index) const
{
    assert(numRows() > index);
    return (*this)[index];
}

HC::Matrix::Row HC::Matrix::getMultipliedRow(int index, float multiple)
{
    Row row = getRow(index);
    for (auto & r : row)
    {
        r *= multiple;
    }
    return row;
}

float HC::Matrix::getDeterminant() const
{
    assert(numRows() == numColumns());
    if (numRows() == 2)
    {
        return ((*this)[0][0]*(*this)[1][1]) - ((*this)[0][1]*(*this)[1][0]);
    }
    Matrix triangular = triangularise();
    float determinant = triangular[0][0];
    for (int i = 1; i < triangular.numRows(); ++i)
    {
        determinant *= triangular[i][i];
    }
    return determinant;
}

void HC::Matrix::addToRow(Row row, int index)
{
    assert(numRows() > index);
    assert(numColumns() == row.size());
    for (int r = 0; r < row.size(); ++r)
    {
        (*this)[index][r] += row[r];
    }
}

HC::Matrix HC::Matrix::triangularise() const
{
    assert(numRows() == numColumns());
    Matrix triangular(*this);
    for (int i = 1; i < triangular.numRows(); ++i)
    {
        for (int j = i; j < triangular.numRows(); ++j)
        {
            assert(triangular[i-1][i-1] != 0);
            Row row = triangular.getMultipliedRow(i-1, -triangular[j][i-1] / triangular[i-1][i-1]);
            triangular.addToRow(row, j);
        }
    }
    return triangular;
}

void HC::Matrix::addRow(Row row)
{
    this->push_back(row);
}

int HC::Matrix::numRows() const
{
    return static_cast<int>(size());
}

int HC::Matrix::numColumns() const
{
    return static_cast<int>((*this)[0].size());
}

HC::Matrix HC::Matrix::subtract(const Matrix& input) const
{
    assert(numColumns() == input.numColumns() && numRows() == input.numRows());
    Matrix output(numRows(), numColumns());
    for (int row = 0; row < numRows(); ++row)
    {
        for (int column = 0; column < numColumns(); ++column)
        {
            output[row][column] = (*this)[row][column] - input[row][column];
        }
    }
    return output;
}

HC::Matrix HC::Matrix::getIdentity(unsigned size)
{
    Matrix identity(size, size);
    for (int i = 0; i < size; ++i)
    {
        identity[i][i] = 1.0;
    }
    return identity;
}
