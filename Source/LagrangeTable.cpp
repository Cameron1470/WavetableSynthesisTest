/*
  ==============================================================================

    LagrangeTable.cpp
    Created: 5 Apr 2021 10:48:32am
    Author:  csmit

  ==============================================================================
*/

#include "LagrangeTable.h"

LagrangeTable::LagrangeTable(int _lagrangeOrder, int _numberOfInterpolationPoints)
{
    lagrangeOrder = _lagrangeOrder;
    Q = _numberOfInterpolationPoints;

    lookup = new float* [lagrangeOrder];

    for (int i = 0; i < lagrangeOrder; i++)
    {
        lookup[i] = new float[Q];
    }

    setLagrangeTable();
}


void LagrangeTable::setLagrangeTable()
{
    float* aq = new float[Q];                       // create array for alpha_q values of Q length
    for (int i = 0; i < Q; i++)
    {
        aq[i] = (-Q / 2 + i) / Q;                   // fill array with alpha_q values between -1/2 and 1/2
    }

    float* al = new float[lagrangeOrder];           // create pointer to array of lagrangeOrder length
    for (int i = 0; i < lagrangeOrder; i++)
    {
        al[i] = (1 - lagrangeOrder + 2 * i) / 2;    // fill array with alpha values for interpolation order
    }

    // looping over the polynomials
    for (int i = 0; i < lagrangeOrder; i++)
    {
        std::vector<float> alP;                     // initialize a vector

        for (int n = 0; n < lagrangeOrder; n++)
        {
            alP.push_back(al[n]);                   // get values from pointer array ,al
        }

        float cur = alP[i];                         // extract the current polynomial factor
        alP.erase(alP.begin() + i);                 // remove the current polynomial factor from the vector

        float** facElements = new float* [Q - 1];   // creating an array of float pointers   
        for (int j = 0; j < lagrangeOrder; j++)
        {
            // for each pointer creat a float array
            facElements[i] = new float[lagrangeOrder];
        }

        for (int j = 0; j < Q; j++)
        {
            for (int k = 0; k < lagrangeOrder - 1; k++)
            {
                // adding elements to the fac elements matrix  --> facElements (j,k) = aq(j) - aLP(k)
                facElements[j][k] = aq[j] - alP[k];
            }
        }


        // calculate the denominator for current polynomial
        float denominator = cur - alP[0];
        for (int j = 1; j < lagrangeOrder - 1; j++)
        {
            denominator *= cur - alP[j];
        }

        // calculate the product of the polymnomial factors elemnts for each aq, vertical column
        float* facProd = new float[Q];
        for (int j = 0; j < Q; j++)
        {
            float product = facElements[j][0];

            for (int k = 1; k < lagrangeOrder - 1; k++)
            {
                product *= facElements[j][k];
            }

            facProd[j] = product / denominator;
        }


        //add this to the i column of the lookup variable
        for (int j = 0; j < Q; j++)
        {
            lookup[i][j] = facProd[j];
        }


    }
}


float* LagrangeTable::getTableValues(int rowIndex)
{
    float* weightings = new float[lagrangeOrder];

    for (int i = 0; i < lagrangeOrder; i++)
    {
        weightings[i] = lookup[i][rowIndex];
    }
}