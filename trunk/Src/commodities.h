/*
 * header file commodities.h
 *
 * includes prototype for tradeSystem function
 *
 * Becky Engley and Martin Neal
 * May 2009
 */


double tradeSystem(char* szName, int iYear, int iEntryWindow, int iTrailStopWindow, int iStopLossWindow,
                   char* szEntryDate, char* szNoEntryDate, char* szExitDate);

double tradeSystemData(char* szName, double dPercentData, int iEntryWindow, int iTrailStopWindow,
                       int iStopLossWindow, char* szEntryDateMonthDay, char* szNoEntryDateMonthDay, char* szExitDateMonthDay);
