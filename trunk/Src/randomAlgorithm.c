/*!
  \file    randomAlgorithm.c
  \brief   Algorithm for learning via a random walk
  \author  Becky Engley and Martin Neal
  \date    May 2009
*/

#include <stdio.h>
#include <string.h>
#include "tradeSystem.h"


int RandomAlgorithm(int iIterations, char* szCommodName, int iArgsToRandomize,
                    int distType, double dResults[], int iResultSize)
{
    node nodeCur, nodeBest;

    int i, iStepSize;

    if (iResultSize == 1)
    {
        FPRINTE(stderr,"iResultSize can't be 1\n");
        return ERRVAL;
    }

    if (iResultSize > iIterations + 1)
    {
        FPRINTE(stderr,"iResultSize can't be greater than iIterations\n");
        return ERRVAL;
    }

    iStepSize = iIterations / (iResultSize - 1);
    if (iIterations % (iResultSize - 1) != 0)
    {
        FPRINTE(stderr,"iResultSize is not compatible with iIterations\n");
        return ERRVAL;
    }

    //set current and best to all uniformly random parameters
    Neighbor(&nodeCur.iEntryWindow,
             &nodeCur.iTrailStopWindow,
             &nodeCur.iStopLossWindow,
             nodeCur.szEntryDate,
             nodeCur.szNoEntryDate,
             nodeCur.szExitDate,
             6, UNIFORM);

    nodeCur.dProfit = tradeSystemData(szCommodName, TRAIN_YEARS,
                                      nodeCur.iEntryWindow,
                                      nodeCur.iTrailStopWindow,
                                      nodeCur.iStopLossWindow,
                                      nodeCur.szEntryDate,
                                      nodeCur.szNoEntryDate,
                                      nodeCur.szExitDate);

    memcpy(&nodeBest, &nodeCur, sizeof(node));

    for (i = 0; i < iIterations; i++)
    {
        if (i % iStepSize == 0)
        {
            dResults[i/iStepSize] =
                tradeSystemData(szCommodName,
                                TEST_YEARS,
                                nodeBest.iEntryWindow,
                                nodeBest.iTrailStopWindow,
                                nodeBest.iStopLossWindow,
                                nodeBest.szEntryDate,
                                nodeBest.szNoEntryDate,
                                nodeBest.szExitDate);
        }

        Neighbor(&nodeCur.iEntryWindow,
                 &nodeCur.iTrailStopWindow,
                 &nodeCur.iStopLossWindow,
                 nodeCur.szEntryDate,
                 nodeCur.szNoEntryDate,
                 nodeCur.szExitDate,
                 6, UNIFORM);

        nodeCur.dProfit = tradeSystemData(szCommodName, TRAIN_YEARS,
                                          nodeCur.iEntryWindow,
                                          nodeCur.iTrailStopWindow,
                                          nodeCur.iStopLossWindow,
                                          nodeCur.szEntryDate,
                                          nodeCur.szNoEntryDate,
                                          nodeCur.szExitDate);
        //if its the best so far, save it
        if (nodeCur.dProfit > nodeBest.dProfit)
        {
            memcpy(&nodeBest,&nodeCur,sizeof(node));
        }
    }

    dResults[i/iStepSize] =
        tradeSystemData(szCommodName,
                        TEST_YEARS,
                        nodeBest.iEntryWindow,
                        nodeBest.iTrailStopWindow,
                        nodeBest.iStopLossWindow,
                        nodeBest.szEntryDate,
                        nodeBest.szNoEntryDate,
                        nodeBest.szExitDate);

    printf("%d\t%d\t%d\t%s\t%s\t%s\t%.2lf\n",
           nodeBest.iEntryWindow,
           nodeBest.iTrailStopWindow,
           nodeBest.iStopLossWindow,
           nodeBest.szEntryDate,
           nodeBest.szNoEntryDate,
           nodeBest.szExitDate,
           dResults[i/iStepSize]);

    return 0;
}
