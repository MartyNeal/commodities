/*!
  \file    sim.c
  \brief   Algorithm for a learning via simmulated annealing
  \author  Becky Engley and Martin Neal
  \date    May 2009
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tradeSystem.h"


/* ------------------------------- utility prototypes ----------------------- */

double StayCool(int iCurrentIteration, int iTotalIterations);

double StayHot(int iCurrentIteration, int iTotalIterations);

double LinearCoolingFunction(int iCurrentIteration, int iTotalIterations);


/* ----------------------------- Simulated Annealing ------------------------ */

int SimulatedAnnealing(int iIterations,char* szCommodName, int iArgsToRandomize,
                       int distType, double dResults[], int iResultSize)
{
    node nodeCur, nodeNext, nodeBest;

    int i, iStepSize;
    double (*CoolingFunction)(int, int) = &LinearCoolingFunction;
    double dDeltaE, dDeltaM, mean, var, M2, p, r;

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
             nodeCur.szExitDate, 6,
             UNIFORM);

    nodeCur.dProfit = tradeSystemData(szCommodName, TRAIN_YEARS,
                                      nodeCur.iEntryWindow,
                                      nodeCur.iTrailStopWindow,
                                      nodeCur.iStopLossWindow,
                                      nodeCur.szEntryDate,
                                      nodeCur.szNoEntryDate,
                                      nodeCur.szExitDate);

    memcpy(&nodeBest, &nodeCur, sizeof(node));

    mean = 0;
    M2 = 0;
    //    printf("first dProfit=%d\n",(int)dCurProfit);

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

        //calculate a successor for current
        memcpy(&nodeNext, &nodeCur, sizeof(node));
        Neighbor(&nodeNext.iEntryWindow,
                 &nodeNext.iTrailStopWindow,
                 &nodeNext.iStopLossWindow,
                 nodeNext.szEntryDate,
                 nodeNext.szNoEntryDate,
                 nodeNext.szExitDate,
                 iArgsToRandomize, distType);
        nodeNext.dProfit = tradeSystemData(szCommodName, TRAIN_YEARS,
                                           nodeNext.iEntryWindow,
                                           nodeNext.iTrailStopWindow,
                                           nodeNext.iStopLossWindow,
                                           nodeNext.szEntryDate,
                                           nodeNext.szNoEntryDate,
                                           nodeNext.szExitDate);

        //update the mean and variance using newly sampled instance
        dDeltaE = nodeNext.dProfit - nodeCur.dProfit;
        dDeltaM = dDeltaE - mean;
        mean = mean + dDeltaM/(i+1);
        M2 = M2 + dDeltaM*(dDeltaE - mean);
        var = M2/(i+1);
        p = pnorm(dDeltaE,mean,sqrt(var));

        //check to see if the successor is good enough
        r = (double)random()/RAND_MAX;

        if (dDeltaE >= 0 ||
            CoolingFunction(i,iIterations)*pnorm(dDeltaE,mean,sqrt(var)) > r)
        {
            memcpy(&nodeCur,&nodeNext,sizeof(node));
        }

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


/* ----------------------------- utility functions -------------------------- */

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
