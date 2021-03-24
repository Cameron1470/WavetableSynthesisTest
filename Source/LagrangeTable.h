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
    LagrangeTable(int _lagrangeOrder, int _numberOfInterpolationPoints)
    {
        lagrangeOrder = _lagrangeOrder;
        Q = _numberOfInterpolationPoints;
        
        setLagrangeTable();
    }

    /// function for setting up lagrange table using the private variables Q and lagrangeOrder
    void setLagrangeTable()
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

            // creating matrix for polynomial factor elements
            juce::dsp::Matrix<float> facElements{ size_t(Q) - 1 , size_t(lagrangeOrder) };

            for (int j = 0; j < Q; j++)
            {
                for (int k = 0; k < lagrangeOrder - 1; k++)
                {
                    // elements to the fac elements matrix  --> facElements (j,k) = aq(j) - aLP(k)
                }
            }

            //TODO: calculate the product of the polymnomial factors elemnts for each aq, vertical column

            //TODO: calculate and apply denominators

            //TODO: add this to the i column of the lagrangeTable matrix


        }

    }

    /// function for getting values from the lagrange table matrix
    float getTableValue()
    {

    }

private:
    /// order of Lagrange interpolation
    int lagrangeOrder;

    /// number of values between alpha = -1/2 and 1/2
    int Q;

    /// Matrix for storing Lagrange table
    //juce::dsp::Matrix<float> lagrangeTableMatrix{ size_t(Q), size_t(lagrangeOrder) };

};