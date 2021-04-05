/*
  ==============================================================================

    LagrangeTable.h
    Created: 24 Mar 2021 11:36:25am
    Author:  csmit

    Description: A class for storing a Lagrange interpolation table which will 
    be used in the wavetable oscillator class to improve upon the linear 
    interpolation methods.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class LagrangeTable
{
public:
    LagrangeTable(int _lagrangeOrder, int _numberOfInterpolationPoints);

    /// function for setting up lagrange table using the private variables Q and lagrangeOrder
    void setLagrangeTable();

    /// function for getting values from the lagrange table matrix
    float* getTableValues(int rowIndex);

private:
    /// order of Lagrange interpolation
    int lagrangeOrder;

    /// number of values between alpha = -1/2 and 1/2
    int Q;

    /// double float pointer for storing Lagrange table
    float** lookup;

};