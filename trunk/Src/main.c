#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "commodities.h"
#include "tradeSystem.h"
#define NUMARGS 9

//#define USE_MPI //comment out when not using MPI

typedef struct _experiment {
    double (*LearningFunction)(int iIterations, char* szCommodName, int iArgsToRandomize, int distType);
    int distType;
    int iArgsToChange;
    char szName[MAX_NAME_LEN];
    int iNumIterations;
    int iStepSize;
    int iAverageAccuracy;
    char szFileName[256];
} experiment;

int main(int argc, char** argv)
{
    int i;
    int j;
    int sum;
    time_t tTime;
    int fd;
    int iMyRank = 0;
    char szWriteBuf[256] = {0};

#ifdef USE_MPI
#include <mpi.h>
    int ipes = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ipes);
    MPI_Comm_rank(MPI_COMM_WORLD, &iMyRank);
#endif

    srand(time(&tTime));

    experiment aeExperiments[] = {
        {&SimulatedAnnealing,UNIFORM,1,"Jun_CL",200,10,100,"SimulatedAnnealing_UNIFORM_1_Jun_CL_200_10_100.dat"},
        {&SimulatedAnnealing,UNIFORM,3,"Jun_CL",200,10,100,"SimulatedAnnealing_UNIFORM_3_Jun_CL_200_10_100.dat"},
        {&SimulatedAnnealing,UNIFORM,6,"Jun_CL",200,10,100,"SimulatedAnnealing_UNIFORM_6_Jun_CL_200_10_100.dat"},
        {&SimulatedAnnealing,NORMAL,1,"Jun_CL",200,10,100,"SimulatedAnnealing_NORMAL_1_Jun_CL_200_10_100.dat"},
        {&SimulatedAnnealing,NORMAL,3,"Jun_CL",200,10,100,"SimulatedAnnealing_NORMAL_3_Jun_CL_200_10_100.dat"},
        {&SimulatedAnnealing,NORMAL,6,"Jun_CL",200,10,100,"SimulatedAnnealing_NORMAL_6_Jun_CL_200_10_100.dat"},
        {&SimulatedAnnealing,CONSTANT,1,"Jun_CL",200,10,100,"SimulatedAnnealing_CONSTANT_1_Jun_CL_200_10_100.dat"},
        {&SimulatedAnnealing,CONSTANT,3,"Jun_CL",200,10,100,"SimulatedAnnealing_CONSTANT_3_Jun_CL_200_10_100.dat"},
        {&SimulatedAnnealing,CONSTANT,6,"Jun_CL",200,10,100,"SimulatedAnnealing_CONSTANT_6_Jun_CL_200_10_100.dat"},
        {&RandomAlgorithm,UNIFORM,1,"Jun_CL",200,10,100,"RandomAlgorithm_UNIFORM_1_Jun_CL_200_10_100.dat"},
        {&RandomAlgorithm,UNIFORM,3,"Jun_CL",200,10,100,"RandomAlgorithm_UNIFORM_3_Jun_CL_200_10_100.dat"},
        {&RandomAlgorithm,UNIFORM,6,"Jun_CL",200,10,100,"RandomAlgorithm_UNIFORM_6_Jun_CL_200_10_100.dat"},
        {&RandomAlgorithm,NORMAL,1,"Jun_CL",200,10,100,"RandomAlgorithm_NORMAL_1_Jun_CL_200_10_100.dat"},
        {&RandomAlgorithm,NORMAL,3,"Jun_CL",200,10,100,"RandomAlgorithm_NORMAL_3_Jun_CL_200_10_100.dat"},
        {&RandomAlgorithm,NORMAL,6,"Jun_CL",200,10,100,"RandomAlgorithm_NORMAL_6_Jun_CL_200_10_100.dat"},
        {&RandomAlgorithm,CONSTANT,1,"Jun_CL",200,10,100,"RandomAlgorithm_CONSTANT_1_Jun_CL_200_10_100.dat"},
        {&RandomAlgorithm,CONSTANT,3,"Jun_CL",200,10,100,"RandomAlgorithm_CONSTANT_3_Jun_CL_200_10_100.dat"},
        {&RandomAlgorithm,CONSTANT,6,"Jun_CL",200,10,100,"RandomAlgorithm_CONSTANT_6_Jun_CL_200_10_100.dat"}
    };

    experiment* peMyExpirment = &aeExperiments[iMyRank];

    fd = creat(peMyExpirment->szFileName,0644);
    if (fd == -1)
    {
        fprintf(stderr,"failed to open %s\n",peMyExpirment->szFileName);
        return -1;
    }

    j = strlen(strcpy(szWriteBuf,"i\tprofit\n"));
    write(fd,szWriteBuf,j);
    for(i = 0; i <= peMyExpirment->iNumIterations; i += peMyExpirment->iStepSize)
    {
        sum = 0;
        for(j = 0; j < peMyExpirment->iAverageAccuracy; j++)
        {
            sum += peMyExpirment->LearningFunction(i,peMyExpirment->szName,
                                                    peMyExpirment->iArgsToChange,
                                                    peMyExpirment->distType);
        }
        j = snprintf(szWriteBuf,256,"%d\t%d\n",i,sum/j);
        write(fd,szWriteBuf,j);
    }
    close (fd);
    return 0;
}
