/*
 * header file tradeSystem.h
 *
 * Algorithm for a channel breakout system
 *
 * Becky Engley and Martin Neal
 * May 2009
 *
 */

#define DATAPATH "../Data"
#define SHORT 0
#define LONG 1


typedef struct commodity
{   
    char *szName;
    int iType;         
    int iYear;
    double dTickVal;
    double dTickSize;
} commod;


int generateTradeData(commod comCommodity, double **ppdLow, double **ppdHigh, double **ppdOpen, 
    double **ppdClose, char ***ppszDates, int *piSize);

int generateChannels(commod comCommodity, double *pdLow, double *pdHigh, int iEntryWindow, 
    int iTrailStopWindow, int iStopLossWindow, double **ppdEntryChannel, double **ppdTrailStopChannel,
    double **ppdStopLossChannel);









