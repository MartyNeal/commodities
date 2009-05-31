/*!
  \file    tradeSystem.h
  \brief   Algorithm for a channel breakout system
  \author  Becky Engley and Martin Neal
  \date    May 2009
*/


#ifndef __TRADE_SYSTEM__
#define __TRADE_SYSTEM__

#define DATAPATH "../Data/"
#define SHORT 0
#define LONG 1
#define MAX_NAME_LEN 7
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

/*!
  \brief POSITION_STAMP is a helper macro to FPRINTE
  \param _1
  A string that is the name of the file this macro is being called from.
  \param _2
  A string that is the name of the function this macro is being called from.
  \param _3
  An integer that is the line number this macro is being called from.
*/
#define POSITION_STAMP(_1,_2,_3) \
fprintf(stderr, "ERROR:  FILE:%s FUNCTION:%s LINE:%d\n\t",_1,_2,_3)

/*!
  \brief PRINTE is a wrapper for printf that will print using an error message
  prefix
*/
#define FPRINTE POSITION_STAMP(__FILE__,__FUNCTION__,__LINE__); fprintf

/* ------------------------------- structure defines ------------------------ */

typedef struct commodity
{
    char* szName;
    int iType;
    int iYear;
    int iNumYears;
    double dTickVal;
    double dTickSize;
} commod;


typedef struct _node
{
    int iEntryWindow;
    int iTrailStopWindow;
    int iStopLossWindow;
    char szEntryDate[5];
    char szNoEntryDate[5];
    char szExitDate[5];
    double dProfit;
} node, individual;


typedef struct _experiment {
    int (*LearningFunction)(int iIterations, char* szCommodName,
                            int iArgsToRandomize, int distType,
                            double dResults[], int iResultSize,
                            int iPopSize, int iOrgySize, double dMutationProb);
    int distType;
    int iArgsToChange;
    char szName[MAX_NAME_LEN];
    int iPopSize;
    int iOrgySize;
    double dMutationProb;
    char szFileName[256];
} experiment;

/* ------------------------------- function prototypes ---------------------- */

double tradeSystem(char* szName, int iYear, int iEntryWindow,
                   int iTrailStopWindow, int iStopLossWindow,
                   char* szEntryDate, char* szNoEntryDate, char* szExitDate);

double tradeSystemData(char* szName, double dPercentData, int iEntryWindow,
                       int iTrailStopWindow, int iStopLossWindow,
                       char* szEntryDateMonthDay, char* szNoEntryDateMonthDay,
                       char* szExitDateMonthDay);

void Neighbor(int* piCurEntryWindow, int* piCurTrailStopWindow,
              int* piCurStopLossWindow, char* szCurEntryDate,
              char* szCurNoEntryDate, char* szCurExitDate,
              int iArgsToRandomize, int distType);

int generateTradeData(commod comCommodity, double** padLow, double** padHigh,
                      double** padOpen, double** padClose, char*** paszDates,
                      int* piSize);

int generateChannels(commod comCommodity, double* adLow, double* adHigh,
                     int iEntryWindow, int iTrailStopWindow,
                     int iStopLossWindow, int iSize, double** padEntryChannel,
                     double** padTrailStopChannel, double** padStopLossChannel);

double rnorm();
double pnorm(double x, double mu, double sigma);

#endif
