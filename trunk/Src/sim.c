#include <stdlib.h>
#include <string.h>

#define TRAIN_DATA_PERCENTAGE .66
#define MAX_WINDOW_SIZE 50



//use for hill-climbing
double StayCool(int iCurrentIteration, int iTotalIterations)
{
    return 0;
}

//does a random walk
double StayHot(int iCurrentIteration, int iTotalIterations)
{
    return 1;
}

double LinearCoolingFunction(int iCurrentIteration, int iTotalIterations)
{
    return 1 - (double)iCurrentIteration/iTotalIterations;
}

void RandomNeighbor(int* piCurEntryWindow, int* piCurTrailStopWindow,
                    int* piCurStopLossWindow, char* szCurEntryDate,
                    char* szCurNoEntryDate, char* szCurExitDate,
                    int iArgsToRandomize, int iAmountToRandomize)
{
    *piCurStopLossWindow = random()%MAX_WINDOW_SIZE;
    *piCurTrailStopWindow = random()%MAX_WINDOW_SIZE;
    *piCurStopLossWindow = random()%MAX_WINDOW_SIZE;
    sprintf(szCurEntryDate,"%d%d",(int)(random()%12),(int)(random()%31));
    sprintf(szCurNoEntryDate,"%d%d",(int)(random()%12),(int)(random()%31));
    sprintf(szCurExitDate,"%d%d",(int)(random()%12),(int)(random()%31));
}

void DateAdd(char* szDate, int iAddend)
{
    const int iDaysInMonths[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int iDate = atoi(szDate);
    int iDay = iDate % 100 + iAddend;
    int iMonth = iDate/100 - 1;

    while (iDay > iDaysInMonths[iMonth])
    {
        iDay -= iDaysInMonths[iMonth++];
        iMonth %= 12;
    }

    while (iDay < 1)
    {
        iMonth = (iMonth-1) % 12;
        iDay += iDaysInMonths[iMonth];
    }

    sprintf(szDate,"%d%d",iMonth+1,iDay);
}

int SimulatedAnnealing(int iIterations,char* szCommodName)
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

    int iBestEntryWindow;
    int iBestTrailStopWindow;
    int iBestStopLossWindow;
    char szBestEntryDate[5];
    char szBestNoEntryDate[5];
    char szBestExitDate[5];
    double dBestProfit;

    int i;
    double (*CoolingFunction)(int, int) = &LinearCoolingFunction;
    double dTemperature;

    RandomNeighbor(&iCurEntryWindow,&iCurTrailStopWindow,&iCurStopLossWindow,
                   szCurEntryDate,szCurNoEntryDate,szCurExitDate,0,0);
    dCurProfit = tradeSystemData(szCommodName, TRAIN_DATA_PERCENTAGE,
                                 iCurEntryWindow, iCurTrailStopWindow, iCurStopLossWindow,
                                 szCurEntryDate, szCurNoEntryDate, szCurExitDate);
    iBestEntryWindow = iCurEntryWindow;
    iBestTrailStopWindow = iCurTrailStopWindow;
    iBestStopLossWindow = iCurStopLossWindow;
    strcpy(szCurEntryDate,szBestEntryDate);
    strcpy(szCurNoEntryDate,szBestNoEntryDate);
    strcpy(szCurExitDate,szBestExitDate);
    dBestProfit = dCurProfit;

    for (i = 0; i < iIterations; i++)
    {
        dTemperature = CoolingFunction(i, iIterations);
        RandomNeighbor(&iNextEntryWindow,&iNextTrailStopWindow,&iNextStopLossWindow,
                       szNextEntryDate,szNextNoEntryDate,szNextExitDate,0,0);
        dNextProfit = tradeSystemData(szCommodName, TRAIN_DATA_PERCENTAGE,
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

    return dCurProfit;
}
