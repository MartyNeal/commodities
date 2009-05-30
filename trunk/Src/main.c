/*!
  \file    main.c
  \brief   control file for running experiments
  \author  Becky Engley and Martin Neal
  \date    May 2009
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "commodities.h"
#include "tradeSystem.h"

//#define USE_MPI //comment out when not using MPI
#define ITERATIONS 100
#define STEP_SIZE 100
#define RESULT_SIZE (ITERATIONS/STEP_SIZE+1)
#define AVERAGE_ACCURACY 1

int main(int argc, char** argv)
{
    int i;
    int j;
    time_t tTime;
    int fd;
    int iMyRank = 0;
    char szWriteBuf[256] = {0};
    double dResults[RESULT_SIZE], dSumResults[RESULT_SIZE];

#ifdef USE_MPI
#include <mpi.h>
    int ipes = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ipes);
    MPI_Comm_rank(MPI_COMM_WORLD, &iMyRank);
#endif

    srand(time(&tTime));

    experiment aeExperiments[] = {
        {&SimulatedAnnealing,UNIFORM,1,"Jun_CL","Sim_UNIFORM_1_Jun_CL.dat"},
        {&SimulatedAnnealing,UNIFORM,3,"Jun_CL","Sim_UNIFORM_3_Jun_CL.dat"},
        {&SimulatedAnnealing,UNIFORM,6,"Jun_CL","Sim_UNIFORM_6_Jun_CL.dat"},
        {&SimulatedAnnealing,NORMAL,1,"Jun_CL","Sim_NORMAL_1_Jun_CL.dat"},
        {&SimulatedAnnealing,NORMAL,3,"Jun_CL","Sim_NORMAL_3_Jun_CL.dat"},
        {&SimulatedAnnealing,NORMAL,6,"Jun_CL","Sim_NORMAL_6_Jun_CL.dat"},
        {&SimulatedAnnealing,CONSTANT,1,"Jun_CL","Sim_CONSTANT_1_Jun_CL.dat"},
        {&SimulatedAnnealing,CONSTANT,3,"Jun_CL","Sim_CONSTANT_3_Jun_CL.dat"},
        {&SimulatedAnnealing,CONSTANT,6,"Jun_CL","Sim_CONSTANT_6_Jun_CL.dat"},
        {&GeneticAlgorithm,UNIFORM,1,"Jun_CL","Gen_UNIFORM_1_Jun_CL.dat"},
        {&GeneticAlgorithm,UNIFORM,3,"Jun_CL","Gen_UNIFORM_3_Jun_CL.dat"},
        {&GeneticAlgorithm,UNIFORM,6,"Jun_CL","Gen_UNIFORM_6_Jun_CL.dat"},
        {&GeneticAlgorithm,NORMAL,1,"Jun_CL","Gen_NORMAL_1_Jun_CL.dat"},
        {&GeneticAlgorithm,NORMAL,3,"Jun_CL","Gen_NORMAL_3_Jun_CL.dat"},
        {&GeneticAlgorithm,NORMAL,6,"Jun_CL","Gen_NORMAL_6_Jun_CL.dat"},
        {&GeneticAlgorithm,CONSTANT,1,"Jun_CL","Gen_CONSTANT_1_Jun_CL.dat"},
        {&GeneticAlgorithm,CONSTANT,3,"Jun_CL","Gen_CONSTANT_3_Jun_CL.dat"},
        {&GeneticAlgorithm,CONSTANT,6,"Jun_CL","Gen_CONSTANT_6_Jun_CL.dat"},
        {&RandomAlgorithm,UNIFORM,1,"Jun_CL","Ran_UNIFORM_1_Jun_CL.dat"},
        {&RandomAlgorithm,UNIFORM,3,"Jun_CL","Ran_UNIFORM_3_Jun_CL.dat"},
        {&RandomAlgorithm,UNIFORM,6,"Jun_CL","Ran_UNIFORM_6_Jun_CL.dat"},
        {&RandomAlgorithm,NORMAL,1,"Jun_CL","Ran_NORMAL_1_Jun_CL.dat"},
        {&RandomAlgorithm,NORMAL,3,"Jun_CL","Ran_NORMAL_3_Jun_CL.dat"},
        {&RandomAlgorithm,NORMAL,6,"Jun_CL","Ran_NORMAL_6_Jun_CL.dat"},
        {&RandomAlgorithm,CONSTANT,1,"Jun_CL","Ran_CONSTANT_1_Jun_CL.dat"},
        {&RandomAlgorithm,CONSTANT,3,"Jun_CL","Ran_CONSTANT_3_Jun_CL.dat"},
        {&RandomAlgorithm,CONSTANT,6,"Jun_CL","Ran_CONSTANT_6_Jun_CL.dat"}
    };

    experiment* peMyExpirment = &aeExperiments[iMyRank];

    memset(dSumResults,0,sizeof(dSumResults));

    for(j = 0; j < AVERAGE_ACCURACY; j++)
    {
        peMyExpirment->LearningFunction(ITERATIONS,
                                        peMyExpirment->szName,
                                        peMyExpirment->iArgsToChange,
                                        peMyExpirment->distType,
                                        dResults,RESULT_SIZE);

        for (i = 0; i < RESULT_SIZE; ++i)
        {
            dSumResults[i] += dResults[i];
        }
    }


    fd = creat(peMyExpirment->szFileName,0644);
    if (fd == -1)
    {
        FPRINTE(stderr,"Failed to open %s\n",peMyExpirment->szFileName);
        return -1;
    }
    j = strlen(strcpy(szWriteBuf,"i\tprofit\n"));
    for (i = 0; i < RESULT_SIZE; ++i)
    {
        j = snprintf(szWriteBuf,256,"%d\t%.2lf\n",i,dSumResults[i]/AVERAGE_ACCURACY);
        write(fd,szWriteBuf,j);
    }
    close (fd);

    return 0;
}
