#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tradeSystem.h"

#define POP_SIZE 30
#define ORGY_SIZE 6
#define MUTATE_PROB .05

typedef struct _individual
{
    int iEntryWindow;
    int iTrailStopWindow;
    int iStopLossWindow;
    char szEntryDate[5];
    char szNoEntryDate[5];
    char szExitDate[5];
    double dProfit;
} individual;

void Quicksort(individual indArray[], int iSize, int iLeftToSort);
void Reproduce(individual indParents[], int iParentSize, individual indChildren[], int iChildrenSize);

double GeneticAlgorithm(int iIterations, char* szCommodName, int iArgsToRandomize, int distType)
{
    int i,t;
    //create an initial population
    individual indPopulation[POP_SIZE*2];

    individual* indChildren = indPopulation+POP_SIZE;

    //fill it with random values
    for(i = 0; i < POP_SIZE; i++)
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

    for (t = 0; t < iIterations; ++t)
    {
        //reproduce
        for (i = 0; i < POP_SIZE; i += ORGY_SIZE)
        {
            Reproduce(indPopulation+i,ORGY_SIZE,indChildren+i,ORGY_SIZE);
        }

        //mutate
        for (i = 0; i < POP_SIZE; ++i)
        {
            if ((double)random()/RAND_MAX < MUTATE_PROB)
            {
                Neighbor(&indChildren[i].iEntryWindow,
                         &indChildren[i].iTrailStopWindow,
                         &indChildren[i].iStopLossWindow,
                         indChildren[i].szEntryDate,
                         indChildren[i].szNoEntryDate,
                         indChildren[i].szExitDate,
                         1, UNIFORM);
            }
        }

        //evaluate children
        for (i = 0; i < POP_SIZE; ++i)
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
        Quicksort(indPopulation,POP_SIZE*2,POP_SIZE);
    }
    return indPopulation[0].dProfit;
}


void Reproduce(individual indParents[], int iParentSize, individual indChildren[], int iChildrenSize)
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
        Quicksort(indArray+i+1,iSize-i,iLeftToSort);
}
