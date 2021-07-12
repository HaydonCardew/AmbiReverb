/*
  =============================================================================

    Matrix.h
    Created: 18 Jun 2021 12:15:12
    Author:  Haydon Cardew

  =============================================================================
*/

#pragma once

#include <vector>

using namespace std;

namespace HC
{

class Matrix : public vector<vector<float>>
{
public:
    using vector<vector<float>>::vector;
    typedef vector<float> Row;

    Matrix (int rows, int columns);

    int numRows() const;
    int numColumns() const;

    Row getRow(int index) const;
    float getDeterminant() const;
    Matrix getTranspose() const;
    Matrix getAdjoint() const;
    Matrix getInverse() const;
    Matrix getPseudoInverse() const;
    Matrix triangularise() const;

    Matrix subtract(const Matrix& input) const;
    Matrix multiply(float scalar) const;
    void multiply(const Matrix& input, Matrix& output) const;
    Matrix multiply(const Matrix& input) const;

    void addToRow(Row row, int index);
    void addRow(Row row);
    
    static Matrix getIdentity(unsigned size);
    
private:
    float getCofactor (int row, int column) const;
    Matrix getCofactorMatrix() const;
    Row getMultipliedRow(int index, float multiple);
};

}
