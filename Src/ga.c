/*!
  \file    ga.c
  \brief   Algorithm for learning via genetic algorithms
  \author  Becky Engley and Martin Neal
  \date    May 2009
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tradeSystem.h"


void Quicksort(individual indArray[], int iSize, int iLeftToSort);
void Reproduce(individual indParents[], int iParentSize,
               individual indChildren[], int iChildrenSize);

int GeneticAlgorithm(int iIterations, char* szCommodName, int iArgsToRandomize,
                     int distType, double dResults[], int iStepSize,
                     int iPopSize, int iOrgySize, double dMutationProb)
{
    int i,t;
    //create an initial population
    individual* indPopulation;
    individual* indChildren = indPopulation+iPopSize;

    if (iStepSize == 0)
    {
        FPRINTE(stderr,"iStepSize can't be 0\n");
        return ERRVAL;
    }

    if (iStepSize > iIterations)
    {
        FPRINTE(stderr,"iStepSize can't be greater than iIterations\n");
        return ERRVAL;
    }

    if (iIterations % iStepSize != 0)
    {
        FPRINTE(stderr,"iStepSize incompatible with iIterations\n");
        return ERRVAL;
    }

    indPopulation = (individual*)malloc(iPopSize*2*sizeof(individual));

    //fill it with random values
    for(i = 0; i < iPopSize; i++)
    {
        Neighbor(&indPopulation[i].iEntryWindow,
                 &indPopulation[i].iTrailStopWindow,
                 &indPopulation[i].iStopLossWindow,
                 indPopulation[i].szEntryDate,
                 indPopulation[i].szNoEntryDate,
                 indPopulation[i].szExitDate,
                 6, UNIFORM);

        indPopulation[i].dProfit =
            tradeSystemData(szCommodName,
                            TRAIN_YEARS,
                            indPopulation[i].iEntryWindow,
                            indPopulation[i].iTrailStopWindow,
                            indPopulation[i].iStopLossWindow,
                            indPopulation[i].szEntryDate,
                            indPopulation[i].szNoEntryDate,
                            indPopulation[i].szExitDate);
    }

    Quicksort(indPopulation,iPopSize,iPopSize);

    for (t = 0; t < iIterations; ++t)
    {
        if (t % iStepSize == 0)
        {
            dResults[t/iStepSize] =
                tradeSystemData(szCommodName,
                                TEST_YEARS,
                                indPopulation[0].iEntryWindow,
                                indPopulation[0].iTrailStopWindow,
                                indPopulation[0].iStopLossWindow,
                                indPopulation[0].szEntryDate,
                                indPopulation[0].szNoEntryDate,
                                indPopulation[0].szExitDate);
        }

        //reproduce
        for (i = 0; i < iPopSize; i += iOrgySize)
        {
            Reproduce(indPopulation+i,iOrgySize,indChildren+i,iOrgySize);
        }

        //mutate
        for (i = 0; i < iPopSize; ++i)
        {
            if ((double)random()/RAND_MAX < dMutationProb)
            {
                Neighbor(&indChildren[i].iEntryWindow,
                         &indChildren[i].iTrailStopWindow,
                         &indChildren[i].iStopLossWindow,
                         indChildren[i].szEntryDate,
                         indChildren[i].szNoEntryDate,
                         indChildren[i].szExitDate,
                         iArgsToRandomize, distType);
            }
        }

        //evaluate children
        for (i = 0; i < iPopSize; ++i)
        {
            indChildren[i].dProfit =
                tradeSystemData(szCommodName,
                                TRAIN_YEARS,
                                indPopulation[i].iEntryWindow,
                                indPopulation[i].iTrailStopWindow,
                                indPopulation[i].iStopLossWindow,
                                indPopulation[i].szEntryDate,
                                indPopulation[i].szNoEntryDate,
                                indPopulation[i].szExitDate);
        }
        //cull
        Quicksort(indPopulation,iPopSize*2,iPopSize);
    }

    dResults[t/iStepSize] =
        tradeSystemData(szCommodName,
                        TEST_YEARS,
                        indPopulation[0].iEntryWindow,
                        indPopulation[0].iTrailStopWindow,
                        indPopulation[0].iStopLossWindow,
                        indPopulation[0].szEntryDate,
                        indPopulation[0].szNoEntryDate,
                        indPopulation[0].szExitDate);

    printf("%d\t%d\t%d\t%s\t%s\t%s\t%.2lf\n",
           indPopulation[0].iEntryWindow,
           indPopulation[0].iTrailStopWindow,
           indPopulation[0].iStopLossWindow,
           indPopulation[0].szEntryDate,
           indPopulation[0].szNoEntryDate,
           indPopulation[0].szExitDate,
           dResults[t/iStepSize]);

    free(indPopulation);

    return 0;
}


void Reproduce(individual indParents[], int iParentSize,
               individual indChildren[], int iChildrenSize)
{
    int i,r;

    r = random();
    for (i = 0; i < iChildrenSize; ++i)
    {
        indChildren[i].iEntryWindow = indParents[(r+i)%iParentSize].iEntryWindow;
    }

    r = random();
    for (i = 0; i < iChildrenSize; ++i)
    {
        indChildren[i].iTrailStopWindow = indParents[(r+i)%iParentSize].iTrailStopWindow;
    }

    r = random();
    for (i = 0; i < iChildrenSize; ++i)
    {
        indChildren[i].iStopLossWindow = indParents[(r+i)%iParentSize].iStopLossWindow;
    }

    r = random();
    for (i = 0; i < iChildrenSize; ++i)
    {
        strcpy(indChildren[i].szEntryDate,indParents[(r+i)%iParentSize].szEntryDate);
    }

    r = random();
    for (i = 0; i < iChildrenSize; ++i)
    {
        strcpy(indChildren[i].szNoEntryDate,indParents[(r+i)%iParentSize].szNoEntryDate);
    }

    r = random();
    for (i = 0; i < iChildrenSize; ++i)
    {
        strcpy(indChildren[i].szExitDate,indParents[(r+i)%iParentSize].szExitDate);
    }
}


void Quicksort(individual indArray[], int iSize, int iLeftToSort)
{
    int i;
    int iPiv = --iSize;
    individual indTemp;

    if (iSize <= 1) return;

    for (i = 0; i < iPiv;)
    {
        if (indArray[i].dProfit >= indArray[iPiv].dProfit)
        {
            i++;
            continue;
        }
        else
        {
            memcpy(&indTemp,&indArray[i],sizeof(individual));
            memcpy(&indArray[i],&indArray[iPiv-1],sizeof(individual));
            memcpy(&indArray[iPiv-1],&indArray[iPiv],sizeof(individual));
            memcpy(&indArray[iPiv],&indTemp,sizeof(individual));
            iPiv--;
        }
    }

    Quicksort(indArray,i,iLeftToSort);
    iLeftToSort -= i + 1;
    if (iLeftToSort > 0)
    {
        Quicksort(indArray+i+1,iSize-i,iLeftToSort);
    }
}
