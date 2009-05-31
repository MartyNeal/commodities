/*!
  \file    commodities.h
  \brief   Includes prototype for tradeSystem function
  \author  Becky Engley and Martin Neal
  \date    May 2009
*/

#define UNIFORM 1
#define NORMAL 2
#define CONSTANT 3

double tradeSystem(char* szName, int iYear, int iEntryWindow,
                   int iTrailStopWindow, int iStopLossWindow, char* szEntryDate,
                   char* szNoEntryDate, char* szExitDate);

double tradeSystemData(char* szName, double dPercentData, int iEntryWindow,
                       int iTrailStopWindow, int iStopLossWindow,
                       char* szEntryDateMonthDay, char* szNoEntryDateMonthDay,
                       char* szExitDateMonthDay);

int SimulatedAnnealing(int iIterations,char* szCommodName, int iArgsToRandomize,
                       int distType, double dResults[], int iStepSize,
                       int iPopSize, int iOrgySize, double dMutationProb);

int GeneticAlgorithm(int iIterations, char* szCommodName, int iArgsToRandomize,
                     int distType, double dResults[], int iStepSize,
                     int iPopSize, int iOrgySize, double dMutationProb);

int RandomAlgorithm(int iIterations, char* szCommodName, int iArgsToRandomize,
                    int distType, double dResults[], int iStepSize,
                    int iPopSize, int iOrgySize, double dMutationProb);
