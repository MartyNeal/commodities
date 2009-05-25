/*
 * header file tradeSystem.h
 *
 * Algorithm for a channel breakout system
 *
 * Becky Engley and Martin Neal
 * May 2009
 *
 */

#ifndef __TRADE_SYSTEM__
#define __TRADE_SYSTEM__

#define DATAPATH "../Data/"
#define SHORT 0
#define LONG 1
#define MAX_NAME_LEN 6
#define DATE_LEN 8
#define FORCEDEXIT 2
#define ERRVAL -1
#define TRAIN_YEARS 2
#define TEST_YEARS 3
#define TRAIN_YEARS_PERCENTAGE .67
#define TEST_YEARS_PERCENTAGE (1 - TRAIN_YEARS_PERCENTAGE)
#define MAX_WINDOW_SIZE 50
#define MAX_DATE_SIZE 365
#define STDDEV 4 // 1/4th

#define UNIFORM 1
#define NORMAL 2
#define CONSTANT 3

typedef struct commodity
{
    char* szName;
    int iType;
    int iYear;
    int iNumYears;
    double dTickVal;
    double dTickSize;
} commod;

double tradeSystem(char* szName, int iYear, int iEntryWindow, int iTrailStopWindow, int iStopLossWindow,
                   char* szEntryDate, char* szNoEntryDate, char* szExitDate);

double tradeSystemData(char* szName, double dPercentData, int iEntryWindow, int iTrailStopWindow,
                       int iStopLossWindow, char* szEntryDateMonthDay, char* szNoEntryDateMonthDay, char* szExitDateMonthDay);

void Neighbor(int* piCurEntryWindow, int* piCurTrailStopWindow,
              int* piCurStopLossWindow, char* szCurEntryDate,
              char* szCurNoEntryDate, char* szCurExitDate,
              int iArgsToRandomize, int distType);

int generateTradeData(commod comCommodity, double** padLow, double** padHigh, double** padOpen,
                      double** padClose, char*** paszDates, int* piSize);

int generateChannels(commod comCommodity, double* adLow, double* adHigh, int iEntryWindow,
                     int iTrailStopWindow, int iStopLossWindow, int iSize, double** padEntryChannel, double** padTrailStopChannel,
                     double** padStopLossChannel);

double rnorm();
double pnorm(double x, double mu, double sigma);

#endif
