/*
 * header file commodities.h
 *
 * includes prototype for tradeSystem function
 *
 * Becky Engley and Martin Neal
 * May 2009
 */

#define UNIFORM 1
#define NORMAL 2
#define CONSTANT 3

double tradeSystem(char* szName, int iYear, int iEntryWindow, int iTrailStopWindow, int iStopLossWindow,
                   char* szEntryDate, char* szNoEntryDate, char* szExitDate);

double tradeSystemData(char* szName, double dPercentData, int iEntryWindow, int iTrailStopWindow,
                       int iStopLossWindow, char* szEntryDateMonthDay, char* szNoEntryDateMonthDay, char* szExitDateMonthDay);

double SimulatedAnnealing(int iIterations, char* szCommodName, int iArgsToRandomize, int distType);

double RandomAlgorithm(int iIterations, char* szCommodName, int iArgsToRandomize, int distType);
