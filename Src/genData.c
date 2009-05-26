#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tradeSystem.h"
/*
  MAX_BUFFER is the amount to read in from an input file at a time.  Generally,
  this should be set as high as possible to achieve the best performance.
*/
#define MAX_BUFFER (1<<16) //64K at a time

int generateTradeData(commod comCommodity,
                      double **padLow,
                      double **padHigh,
                      double **padOpen,
                      double **padClose,
                      char ***paszDates,
                      int *piSize)
{
    char szPathname[256] = DATAPATH;
    char szReadBuf[MAX_BUFFER];
    char* szCurLine;
    int fdInFile;
    int iRet = 0;
    int cbLeftover = 0;
    int iIndex;
    int iVal;
    int i;
    int cbBytesReturned;

    if (!padLow)
    {
        fprintf(stderr,"padLow was null");
        iRet = ERRVAL;
        goto exit;
    }
    if (!padHigh)
    {
        fprintf(stderr,"padHigh was null");
        iRet = ERRVAL;
        goto exit;
    }
    if (!padOpen)
    {
        fprintf(stderr,"padOpen was null");
        iRet = ERRVAL;
        goto exit;
    }
    if (!padClose)
    {
        fprintf(stderr,"padClose was null");
        iRet = ERRVAL;
        goto exit;
    }
    if (!paszDates)
    {
        fprintf(stderr,"paszDates was null");
        iRet = ERRVAL;
        goto exit;
    }
    if (!piSize)
    {
        fprintf(stderr,"piSize was null");
        iRet = ERRVAL;
        goto exit;
    }

    *padOpen = NULL;
    *padHigh = NULL;
    *padLow = NULL;
    *padClose = NULL;
    *paszDates = NULL;

    //create the file name based on info in comCommodity
    if(!snprintf(szPathname,
                 sizeof(szPathname),
                 "%s%s%d.txt",
                 DATAPATH,
                 comCommodity.szName,
                 comCommodity.iYear))
    {
        fprintf(stderr,"snprintf failed");
        iRet = ERRVAL;
        goto exit;
    }

    fdInFile = open(szPathname, O_RDONLY);
    if(fdInFile == ERRVAL)
    {
        fprintf(stderr,"open failed");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padOpen = (double*)malloc(MAX_BUFFER*sizeof(double)))))
    {
        fprintf(stderr,"malloc *padOpen failed");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padHigh = (double*)malloc(MAX_BUFFER*sizeof(double)))))
    {
        fprintf(stderr,"malloc *padHigh failed");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padLow = (double*)malloc(MAX_BUFFER*sizeof(double)))))
    {
        fprintf(stderr,"malloc *padLow failed");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padClose = (double*)malloc(MAX_BUFFER*sizeof(double)))))
    {
        fprintf(stderr,"malloc *padClose failed");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*paszDates = (char**)malloc(MAX_BUFFER*sizeof(char*)))))
    {
        fprintf(stderr,"malloc *paszDates failed");
        iRet = ERRVAL;
        goto exit;
    }

    memset(*paszDates,0,MAX_BUFFER*sizeof(char*));

    *piSize = 0;
    cbLeftover = 0;
    //begin reading large chunks from the file
    cbBytesReturned = read(fdInFile,szReadBuf,MAX_BUFFER);

    //verify that the file contains this header
    if(sscanf(szReadBuf,"Date,Open,High,Low,Close") == EOF)
    {
        fprintf(stderr,"scanf didn't find any headers when it should have");
        iRet = ERRVAL;
        goto exit;
    }

    //before entering our main loop, consume the header
    for (cbLeftover = 0; cbLeftover < cbBytesReturned; cbLeftover++)
    {
            if (szReadBuf[cbLeftover] == '\n')
            {
                // Replace the newline with NULL
                szReadBuf[cbLeftover] = '\0';
                break;
            }
    }

    cbLeftover++;
    szCurLine = szReadBuf + cbLeftover;

    do
    {
        // For each character in the chunk:
        for (iIndex = cbLeftover; iIndex < cbBytesReturned; iIndex++)
        {
            // Scan through the chunk looking for a newline
            // For each newline we come to:
            if (szReadBuf[iIndex] == '\n')
            {
                // Replace the newline with NULL
                szReadBuf[iIndex] = '\0';

                // malloc space for the date
                (*paszDates)[*piSize] = (char*)malloc(8*sizeof(char));

                switch((iVal = sscanf(szCurLine,
                                      "%8s, %lf, %lf, %lf, %lf",
                                      (*paszDates)[*piSize],
                                      &((*padOpen)[*piSize]),
                                      &((*padHigh)[*piSize]),
                                      &((*padLow)[*piSize]),
                                      &((*padClose)[*piSize]))))
                {
                case 0: //expected at the end of the file

                    //resize the arrays to just the right size
                    *padOpen = (double*)realloc(*padOpen, *piSize*sizeof(double));
                    *padHigh = (double*)realloc(*padHigh, *piSize*sizeof(double));
                    *padLow = (double*)realloc(*padLow, *piSize*sizeof(double));
                    *padClose = (double*)realloc(*padClose, *piSize*sizeof(double));
                    *paszDates = (char**)realloc(*paszDates, *piSize*sizeof(char*));
                    goto exit;
                    break;
                case 5: //expected
                    //increment the index of the array
                    (*piSize)++;

                    //set the curLine to be the next line
                    szCurLine = szReadBuf+iIndex+1;
                    break;
                default:
                    fprintf(stderr,"szCurLine = %s\n",szCurLine);
                    fprintf(stderr,"*piSize = %d\n",*piSize);
                    fprintf(stderr,"iVal = %d\n",iVal);
                    fprintf(stderr,"sscanf returned an incorrect value");
                    iRet = ERRVAL;
                    goto exit;
                }
            }
        }

        // When we hit the end of the chunk, save the leftovers to the top of the buffer,
        cbLeftover = MAX_BUFFER - (szCurLine - szReadBuf);
        memmove(szReadBuf,szCurLine,cbLeftover);

        // And then refill the remainder with more from the file
        cbBytesReturned = read(fdInFile,szReadBuf + cbLeftover,szCurLine - szReadBuf);
        szCurLine = szReadBuf;
    }
    while (cbBytesReturned != 0);

 exit:
    if (iRet != 0)
    {
        if (*padOpen)
        {
            free(*padOpen);
        }
        if (*padHigh)
        {
            free(*padHigh);
        }
        if (*padLow)
        {
            free(*padLow);
        }
        if (*padClose)
        {
            free(*padClose);
        }
        for (i = 0; i < *piSize; i++)
        {
            if ((*paszDates)[i])
            {
                free((*paszDates)[i]);
            }
        }
        if (*paszDates)
        {
            free(*paszDates);
        }
    }

    close(fdInFile);

    return iRet;
}

int generateChannels(commod comCommodity,
                     double* adLow,
                     double* adHigh,
                     int iEntryWindow,
                     int iTrailStopWindow,
                     int iStopLossWindow,
                     int iSize,
                     double** padEntryChannel,
                     double** padTrailStopChannel,
                     double** padStopLossChannel)
{
    int iRet = 0;
    int i;
    int j;
    double min;
    double max;

    if (!padEntryChannel)
    {
        fprintf(stderr,"padEntryChannel was null");
        iRet = ERRVAL;
        goto exit;
    }

    if (!padTrailStopChannel)
    {
        fprintf(stderr,"padTrailStopChannel was null");
        iRet = ERRVAL;
        goto exit;
    }

    if (!padStopLossChannel)
    {
        fprintf(stderr,"padStopLossChannel was null");
        iRet = ERRVAL;
        goto exit;
    }

    if (!adLow)
    {
        fprintf(stderr,"adLow was null");
        iRet = ERRVAL;
        goto exit;
    }

    if (!adHigh)
    {
        fprintf(stderr,"adHigh was null");
        iRet = ERRVAL;
        goto exit;
    }

    *padEntryChannel = NULL;
    *padTrailStopChannel = NULL;
    *padStopLossChannel = NULL;

    if(!((*padEntryChannel = (double*)malloc(iSize*sizeof(double)))))
    {
        fprintf(stderr,"malloc *padEntryChannel failed");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padTrailStopChannel = (double*)malloc(iSize*sizeof(double)))))
    {
        fprintf(stderr,"malloc *padTrailStopChannel failed");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padStopLossChannel = (double*)malloc(iSize*sizeof(double)))))
    {
        fprintf(stderr,"malloc *padStopLossChannel failed");
        iRet = ERRVAL;
        goto exit;
    }

    if (comCommodity.iType == SHORT)
    {
        for(i = 0; i < iSize; i++)
        {
            if(i < (iEntryWindow - 1))
            {
                (*padEntryChannel)[i] = ERRVAL;
            }
            else
            {
                min = adLow[i];
                for(j = i - (iEntryWindow - 1); j < i; j++)
                {
                    if(adLow[j] < min)
                    {
                        min = adLow[j];
                    }
                }
                (*padEntryChannel)[i] = min;
            }
            if(i < (iTrailStopWindow - 1))
            {
                (*padTrailStopChannel)[i] = ERRVAL;
            }
            else
            {
                max = adHigh[i];
                for(j = i - (iTrailStopWindow - 1); j < i; j++)
                {
                    if(adHigh[j] > max)
                    {
                        max = adHigh[j];
                    }
                }
                (*padTrailStopChannel)[i] = max;
            }
            if(i < (iStopLossWindow - 1))
            {
                (*padStopLossChannel)[i] = ERRVAL;
            }
            else
            {
                max = adHigh[i];
                for(j = i - (iStopLossWindow - 1); j < i; j++)
                {
                    if(adHigh[j] > max)
                    {
                        max = adHigh[j];
                    }
                }
                (*padStopLossChannel)[i] = max;
            }
        }
    }
    else
    {
        for(i = 0; i < iSize; i++)
        {
            if(i < (iEntryWindow - 1))
            {
                (*padEntryChannel)[i] = ERRVAL;
            }
            else
            {
                max = adHigh[i];
                for(j = i - (iEntryWindow - 1); j < i; j++)
                {
                    if(adHigh[j] > max)
                    {
                        max = adHigh[j];
                    }
                }
                (*padEntryChannel)[i] = max;
            }
            if(i < (iTrailStopWindow - 1))
            {
                (*padTrailStopChannel)[i] = ERRVAL;
            }
            else
            {
                min = adLow[i];
                for(j = i - (iTrailStopWindow - 1); j < i; j++)
                {
                    if(adLow[j] < min)
                    {
                        min = adLow[j];
                    }
                }
                (*padTrailStopChannel)[i] = min;
            }
            if(i < (iStopLossWindow - 1))
            {
                (*padStopLossChannel)[i] = ERRVAL;
            }
            else
            {
                min = adLow[i];
                for(j = i - (iStopLossWindow - 1); j < i; j++)
                {
                    if(adLow[j] < min)
                    {
                        min = adLow[j];
                    }
                }
                (*padStopLossChannel)[i] = min;
            }
        }
    }

 exit:
    if (iRet != 0)
    {
        if (*padEntryChannel)
        {
            free(*padEntryChannel);
        }
        if (*padTrailStopChannel)
        {
            free(*padTrailStopChannel);
        }
        if (*padStopLossChannel)
        {
            free(*padStopLossChannel);
        }
    }

    return iRet;
}