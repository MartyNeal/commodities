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


typedef struct commodity
{
    char* szName;
    int iType;
    int iYear;
    double dTickVal;
    double dTickSize;
} commod;

int generateTradeData(commod comCommodity, double** padLow, double** padHigh, double** padOpen,
    double** padClose, char*** paszDates, int* piSize);

int generateChannels(commod comCommodity, double* adLow, double* adHigh, int iEntryWindow,
    int iTrailStopWindow, int iStopLossWindow, double** padEntryChannel, double** padTrailStopChannel,
    double** padStopLossChannel);

#endif
