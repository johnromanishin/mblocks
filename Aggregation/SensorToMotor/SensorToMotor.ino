#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator> // for back_inserter 
#include <random>
#include "algorithm.h"

// Definitions: Hard-coded.
const int nContext = 18;
const int nSensor  = 4;

// 0 = no neighbors 
// 1 = Light only
// 2 = Neighbor in contact

const std::vector< std::vector<int> > SensorList = { {1,1,1,1}, 
                                                     {1,1,1,0}, 
                                                     {1,1,1,2}, 
                                                     {2,1,2,1}, 
                                                     {2,1,0,1}, 
                                                     {0,1,0,1}, 
                                                     {1,1,2,2}, 
                                                     {1,1,0,2}, 
                                                     {1,1,2,0}, 
                                                     {1,1,0,0}, 
                                                     {1,2,2,2}, 
                                                     {1,0,2,2}, 
                                                     {1,2,0,2}, 
                                                     {1,2,2,0}, 
                                                     {1,0,0,2}, 
                                                     {1,0,2,0}, 
                                                     {1,2,0,0}, 
                                                     {1,0,0,0} };
         
const std::vector< std::vector<int> > ControllerDict = { {0, 1, 2, 3},
                                                         {4, 5, 6},
                                                         {7, 8, 9},
                                                         {10, 11},
                                                         {12, 13},
                                                         {14, 15},
                                                         {16, 17},
                                                         {18, 19},
                                                         {20, 21},
                                                         {22, 23},
                                                         {24},
                                                         {25},
                                                         {26},
                                                         {27},
                                                         {28},
                                                         {29},
                                                         {30},
                                                         {31} };

const std::vector<float> ControlParameters =  {2.14227699e-01, 1.05419841e-01, 1.84067061e-01, 4.16797019e-02,
                                               2.91384338e-02, 9.98197117e-01, 3.51535127e-02, 1.39068161e-03,
                                               3.01063835e-02, 2.03673939e-02, 6.75807063e-01, 5.57445512e-02,
                                               5.28726365e-01, 9.93379583e-01, 5.48178745e-01, 7.05043896e-01,
                                               1.67147769e-02, 2.15627617e-02, 8.24514833e-04, 4.76639205e-02,
                                               3.79914571e-01, 9.72522112e-01, 9.86571294e-01, 9.99585010e-01,
                                               9.73944577e-01, 9.97738802e-01, 3.10794687e-04, 1.21327340e-03,
                                               9.94012392e-01, 4.78038385e-01, 9.37732719e-01, 3.09412028e-01};


int main(int argc, char* argv[])
{
    // The code based on sensor reading includes at least one 1.
    // If the sensor reading made of only 0s and 2s, that means stop only!    
    // This is the container for the sensor readings. A vector of nSensor = 4. The order is N, W, S, E.
    
    std::vector<int> SR = {2,1,1,1};  // Example Sensor Reading.
    // Sanity check. If less than 4 sensor readigs or if any sensor reading is outside {0,1,2}; terminate.
        bool ConditionToProceed = true;
        if (SR.size() != nSensor)
            ConditionToProceed = false;
        int i = 0;
        do 
        {
            if (SR[i] < 0 || SR[i] > 2)
                ConditionToProceed = false;
            i++;
        }
        while(i < nSensor && ConditionToProceed);
        if (!ConditionToProceed)
        {
            printf("Wrong Sensor Format!\n");
        }

    // Algorithm Starts: 
    // Find SR_P (permutated) in AllSensorList
        int iContext = -1; // Has to start from -1, so that inside the while loop it will be 0.
        int iPermute; 
        bool sensorFound = false;
        while(!sensorFound && iContext < nContext)
        {
            iContext++;
            std::vector<int> SR_P;
            std::copy(SensorList[iContext].begin(), SensorList[iContext].end(), std::back_inserter(SR_P));
            iPermute = -1;
            while(!sensorFound && iPermute < nSensor)
            {
                iPermute++;
                if (SR == SR_P) // If they are the same, then there is no need to rotate. We found it.
                {
                    sensorFound = true;   
                }
                else // We rotate one-by-one until it matches. 
                {
                    std::rotate(SR_P.begin(), SR_P.begin() + (nSensor - 1), SR_P.end()); // 2nd arg for right rotation: +(nSensor - 1), left rotation: +1.
                }
              }
        }
    
    // Find the indices in Parameter List
        std::vector<int> iParameters; // List of indices for the parameters.
        std::copy(ControllerDict[iContext].begin(), ControllerDict[iContext].end(), std::back_inserter(iParameters)); // Copy the content.

        // Populate the Parameters.
        std::vector<float> Parameters = {0.0, 0.0, 0.0, 0.0}; // The parameters {p_1, p_2, p_3, p_4}. It will be a vector of 5 later on, but now p_0 is not included.
        float sumParameters = 0.0; // To be used in calc. of p_0.s
        int j = 0; // Temp. counter for the iParameters index.
        int k;
        int nZeros = nSensor - iParameters.size();
        for (int i = 0; i < nSensor; i++)
        {
            if (SensorList[iContext][i] == 1) // If there is light, assign the control parameter for that index.
            {
                Parameters[i] = ControlParameters[iParameters[j]]/iParameters.size(); // Normalize 
                sumParameters += Parameters[i]; // Sum all the parameter values.
                j++;
            }
        }   
    // Now we know, how many times the parameters should be permutated ("iPermute" times).
        std::rotate(Parameters.begin(), Parameters.begin() + (nSensor - iPermute), Parameters.end()); // 2nd arg for right rotation: +(nSensor - iPermute), left rotation: +1.
        Parameters.insert(Parameters.begin(), 1 - sumParameters); // Calc. and insert p_0 to the beginning.
        // Now we have a Parameter vector of 5 elements. This is the Prob. Distribution Weights.
        std::random_device rd;
        std::mt19937 generator(rd()); // Random number generator.
        std::discrete_distribution<int> ProbDist(Parameters.begin(),Parameters.end());
        int Action = ProbDist(generator); // Action. Order; Stop, N, W, S, E.
    return 0;
}
