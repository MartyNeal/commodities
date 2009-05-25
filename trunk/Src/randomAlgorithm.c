#include <stdlib.h>
#include <string.h>
#include "tradeSystem.h"


double RandomAlgorithm(int iIterations, char* szCommodName, int iArgsToRandomize, int distType)
{
    int iCurEntryWindow;
    int iCurTrailStopWindow;
    int iCurStopLossWindow;
    char szCurEntryDate[5];
    char szCurNoEntryDate[5];
    char szCurExitDate[5];
    double dCurProfit;

    int iBestEntryWindow;
    int iBestTrailStopWindow;
    int iBestStopLossWindow;
    char szBestEntryDate[5];
    char szBestNoEntryDate[5];
    char szBestExitDate[5];
    double dBestProfit;

    int i;

    //set current and best to all uniformly random parameters
    Neighbor(&iCurEntryWindow, &iCurTrailStopWindow, &iCurStopLossWindow,
             szCurEntryDate, szCurNoEntryDate, szCurExitDate, 6,
             UNIFORM);
    dCurProfit = tradeSystemData(szCommodName, TRAIN_YEARS,
                                 iCurEntryWindow, iCurTrailStopWindow,
                                 iCurStopLossWindow, szCurEntryDate,
                                 szCurNoEntryDate, szCurExitDate);

    iBestEntryWindow = iCurEntryWindow;
    iBestTrailStopWindow = iCurTrailStopWindow;
    iBestStopLossWindow = iCurStopLossWindow;
    strcpy(szBestEntryDate, szCurEntryDate);
    strcpy(szBestNoEntryDate, szCurNoEntryDate);
    strcpy(szBestExitDate, szCurExitDate);
    dBestProfit = dCurProfit;

    for (i = 0; i < iIterations; i++)
    {
        Neighbor(&iCurEntryWindow,&iCurTrailStopWindow,&iCurStopLossWindow,
                 szCurEntryDate,szCurNoEntryDate,szCurExitDate,
                 iArgsToRandomize,distType);
        dCurProfit = tradeSystemData(szCommodName, TRAIN_YEARS,
                                     iCurEntryWindow, iCurTrailStopWindow, iCurStopLossWindow,
                                     szCurEntryDate, szCurNoEntryDate, szCurExitDate);
        if (dCurProfit > dBestProfit)
        {
            iBestEntryWindow = iCurEntryWindow;
            iBestTrailStopWindow = iCurTrailStopWindow;
            iBestStopLossWindow = iCurStopLossWindow;
            strcpy(szBestEntryDate,szCurEntryDate);
            strcpy(szBestNoEntryDate,szCurNoEntryDate);
            strcpy(szBestExitDate,szCurExitDate);
            dBestProfit = dCurProfit;
        }
    }
    dCurProfit = tradeSystemData(szCommodName,TEST_YEARS,
                                 iCurEntryWindow, iCurTrailStopWindow, iCurStopLossWindow,
                                 szCurEntryDate, szCurNoEntryDate, szCurExitDate);
    return dCurProfit;
}
