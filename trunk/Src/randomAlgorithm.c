#include <stdlib.h>
#include <string.h>
#include "tradeSystem.h"


int RandomAlgorithm(int iIterations, char* szCommodName, int iArgsToRandomize, int distType)
{
    int iCurEntryWindow;
    int iCurTrailStopWindow;
    int iCurStopLossWindow;
    char szCurEntryDate[5];
    char szCurNoEntryDate[5];
    char szCurExitDate[5];
    double dCurProfit;

    int iNextEntryWindow;
    int iNextTrailStopWindow;
    int iNextStopLossWindow;
    char szNextEntryDate[5];
    char szNextNoEntryDate[5];
    char szNextExitDate[5];
    double dNextProfit;

    int i;

    Neighbor(&iCurEntryWindow,&iCurTrailStopWindow,&iCurStopLossWindow,
             szCurEntryDate,szCurNoEntryDate,szCurExitDate,6,UNIFORM);
    dCurProfit = tradeSystemData(szCommodName, TRAIN_YEARS,
                                 iCurEntryWindow, iCurTrailStopWindow, iCurStopLossWindow,
                                 szCurEntryDate, szCurNoEntryDate, szCurExitDate);
    for (i = 0; i < iIterations; i++)
    {
        Neighbor(&iNextEntryWindow,&iNextTrailStopWindow,&iNextStopLossWindow,
                 szNextEntryDate,szNextNoEntryDate,szNextExitDate,
                 iArgsToRandomize,distType);
        dNextProfit = tradeSystemData(szCommodName, TRAIN_YEARS,
                                     iNextEntryWindow, iNextTrailStopWindow, iNextStopLossWindow,
                                     szNextEntryDate, szNextNoEntryDate, szNextExitDate);
        if (dNextProfit > dCurProfit)
        {
            iCurEntryWindow = iNextEntryWindow;
            iCurTrailStopWindow = iNextTrailStopWindow;
            iCurStopLossWindow = iNextStopLossWindow;
            strcpy(szNextEntryDate,szCurEntryDate);
            strcpy(szNextNoEntryDate,szCurNoEntryDate);
            strcpy(szNextExitDate,szCurExitDate);
            dCurProfit = dNextProfit;
        }
    }
    dCurProfit = tradeSystemData(szCommodName,TEST_YEARS,
                                 iCurEntryWindow, iCurTrailStopWindow, iCurStopLossWindow,
                                 szCurEntryDate, szCurNoEntryDate, szCurExitDate);
    return dCurProfit;
}
