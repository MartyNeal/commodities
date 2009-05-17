#include <stdio.h>
#include <string.h>
#include <error.h>
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
    int i;
    int cbBytesReturned;

    if (!padLow)
    {
        error(0,0,"padLow was null");
        iRet = ERRVAL;
        goto exit;
    }
    if (!padHigh)
    {
        error(0,0,"padHigh was null");
        iRet = ERRVAL;
        goto exit;
    }
    if (!padOpen)
    {
        error(0,0,"padOpen was null");
        iRet = ERRVAL;
        goto exit;
    }
    if (!padClose)
    {
        error(0,0,"padClose was null");
        iRet = ERRVAL;
        goto exit;
    }
    if (!paszDates)
    {
        error(0,0,"paszDates was null");
        iRet = ERRVAL;
        goto exit;
    }
    if (!piSize)
    {
        error(0,0,"piSize was null");
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
        error(0,0,"snprintf failed");
        iRet = ERRVAL;
        goto exit;
    }

    fdInFile = open(szPathname, O_RDONLY);
    if(fdInFile == ERRVAL)
    {
        error(0,0,"open failed");
        iRet = ERRVAL;
        goto exit;
    }

    //begin reading large chunks from the file
    cbBytesReturned = read(fdInFile,szReadBuf,MAX_BUFFER);
    if(sscanf(szReadBuf,"Date,Open,High,Low,Close") == EOF)
    {
        error(0,0,"scanf didn't find any headers when it should have");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padOpen = (double*)malloc(MAX_BUFFER*sizeof(double)))))
    {
        error(0,0,"malloc *padOpen failed");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padHigh = (double*)malloc(MAX_BUFFER*sizeof(double)))))
    {
        error(0,0,"malloc *padHigh failed");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padLow = (double*)malloc(MAX_BUFFER*sizeof(double)))))
    {
        error(0,0,"malloc *padLow failed");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padClose = (double*)malloc(MAX_BUFFER*sizeof(double)))))
    {
        error(0,0,"malloc *padClose failed");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*paszDates = (char**)malloc(MAX_BUFFER*sizeof(char*)))))
    {
        error(0,0,"malloc *paszDates failed");
        iRet = ERRVAL;
        goto exit;
    }

    memset(*paszDates,0,MAX_BUFFER*sizeof(char*));

    *piSize = 0;
    cbLeftover = 0;
    do
    {
        szCurLine = szReadBuf;

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
                *paszDates[*piSize] = (char*)malloc(8*sizeof(char));

                switch(sscanf(szReadBuf,
                              "%8s, %f, %f, %f, %f",
                              (*paszDates)[*piSize],
                              (float*)&((*padOpen)[*piSize]),
                              (float*)&((*padHigh)[*piSize]),
                              (float*)&((*padLow)[*piSize]),
                              (float*)&((*padClose)[*piSize])))
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
                    error(0,0,"sscanf returned an incorrect value");
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
    int min;
    int max;

    if (!padEntryChannel)
    {
        error(0,0,"padEntryChannel was null");
        iRet = ERRVAL;
        goto exit;
    }

    if (!padTrailStopChannel)
    {
        error(0,0,"padTrailStopChannel was null");
        iRet = ERRVAL;
        goto exit;
    }

    if (!padStopLossChannel)
    {
        error(0,0,"padStopLossChannel was null");
        iRet = ERRVAL;
        goto exit;
    }

    if (!adLow)
    {
        error(0,0,"adLow was null");
        iRet = ERRVAL;
        goto exit;
    }

    if (!adHigh)
    {
        error(0,0,"adHigh was null");
        iRet = ERRVAL;
        goto exit;
    }

    *padEntryChannel = NULL;
    *padTrailStopChannel = NULL;
    *padStopLossChannel = NULL;

    if(!((*padEntryChannel = (double*)malloc(iSize*sizeof(double*)))))
    {
        error(0,0,"malloc *padEntryChannel failed");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padTrailStopChannel = (double*)malloc(iSize*sizeof(double*)))))
    {
        error(0,0,"malloc *padTrailStopChannel failed");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padStopLossChannel = (double*)malloc(iSize*sizeof(double*)))))
    {
        error(0,0,"malloc *padStopLossChannel failed");
        iRet = ERRVAL;
        goto exit;
    }

    if (comCommodity.iType == SHORT)
    {
        for(i = 0; i < iSize; i++)
        {
            if(i < iEntryWindow)
            {
                *padEntryChannel[i] = ERRVAL;
            }
            else
            {
                min = adLow[i];
                for(j = i - iEntryWindow; j < i; j++)
                {
                    if(adLow[j] < adLow[i])
                    {
                        min = adLow[j];
                    }
                }
                *padEntryChannel[i] = min;
            }
            if(i < iTrailStopWindow)
            {
                *padTrailStopChannel[i] = ERRVAL;
            }
            else
            {
                max = adHigh[i];
                for(j = i - iTrailStopWindow; j < i; j++)
                {
                    if(adHigh[j] > adHigh[i])
                    {
                        max = adHigh[j];
                    }
                }
                *padTrailStopChannel[i] = max;
            }
            if(i < iStopLossWindow)
            {
                *padStopLossChannel[i] = ERRVAL;
            }
            else
            {
                max = adHigh[i];
                for(j = i - iStopLossWindow; j < i; j++)
                {
                    if(adHigh[j] > adHigh[i])
                    {
                        max = adHigh[j];
                    }
                }
                *padStopLossChannel[i] = max;
            }
        }
    }
    else
    {
        for(i = 0; i < iSize; i++)
        {
            if(i < iEntryWindow)
            {
                *padEntryChannel[i] = ERRVAL;
            }
            else
            {
                max = adHigh[i];
                for(j = i - iEntryWindow; j < i; j++)
                {
                    if(adHigh[j] > adHigh[i])
                    {
                        max = adHigh[j];
                    }
                }
                *padEntryChannel[i] = max;
            }
            if(i < iTrailStopWindow)
            {
                *padTrailStopChannel[i] = ERRVAL;
            }
            else
            {
                min = adLow[i];
                for(j = i - iTrailStopWindow; j < i; j++)
                {
                    if(adLow[j] < adLow[i])
                    {
                        min = adLow[j];
                    }
                }
                *padTrailStopChannel[i] = min;
            }
            if(i < iStopLossWindow)
            {
                *padStopLossChannel[i] = ERRVAL;
            }
            else
            {
                min = adLow[i];
                for(j = i - iStopLossWindow; j < i; j++)
                {
                    if(adLow[j] < adLow[i])
                    {
                        min = adLow[j];
                    }
                }
                *padStopLossChannel[i] = min;
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
