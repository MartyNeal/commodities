/*!
  \file    genData.c
  \brief   Functions for reading commodity data and generating the channels
  \author  Becky Engley and Martin Neal
  \date    May 2009
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "tradeSystem.h"

/*!
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
        FPRINTE(stderr,"padLow was null\n");
        iRet = ERRVAL;
        goto exit;
    }
    if (!padHigh)
    {
        FPRINTE(stderr,"padHigh was null\n");
        iRet = ERRVAL;
        goto exit;
    }
    if (!padOpen)
    {
        FPRINTE(stderr,"padOpen was null\n");
        iRet = ERRVAL;
        goto exit;
    }
    if (!padClose)
    {
        FPRINTE(stderr,"padClose was null\n");
        iRet = ERRVAL;
        goto exit;
    }
    if (!paszDates)
    {
        FPRINTE(stderr,"paszDates was null\n");
        iRet = ERRVAL;
        goto exit;
    }
    if (!piSize)
    {
        FPRINTE(stderr,"piSize was null\n");
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
        FPRINTE(stderr,"snprintf failed\n");
        iRet = ERRVAL;
        goto exit;
    }

    fdInFile = open(szPathname, O_RDONLY);
    if(fdInFile == ERRVAL)
    {
        FPRINTE(stderr,"ERROR:  failed to open %s\n",szPathname);
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padOpen = (double*)malloc(MAX_BUFFER*sizeof(double)))))
    {
        FPRINTE(stderr,"malloc *padOpen failed\n");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padHigh = (double*)malloc(MAX_BUFFER*sizeof(double)))))
    {
        FPRINTE(stderr,"malloc *padHigh failed\n");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padLow = (double*)malloc(MAX_BUFFER*sizeof(double)))))
    {
        FPRINTE(stderr,"malloc *padLow failed\n");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padClose = (double*)malloc(MAX_BUFFER*sizeof(double)))))
    {
        FPRINTE(stderr,"malloc *padClose failed\n");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*paszDates = (char**)malloc(MAX_BUFFER*sizeof(char*)))))
    {
        FPRINTE(stderr,"malloc *paszDates failed\n");
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
        FPRINTE(stderr,"scanf didn't find any headers when it should have\n");
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
                    FPRINTE(stderr,"sscanf returned an incorrect value\n\
                                   szPathname = %s\n\
                                   szCurLine = %s\n\
                                   *piSize = %d\n\
                                   iVal = %d\n",
                            szPathname, szCurLine, *piSize, iVal);
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
        FPRINTE(stderr,"padEntryChannel was null\n");
        iRet = ERRVAL;
        goto exit;
    }

    if (!padTrailStopChannel)
    {
        FPRINTE(stderr,"padTrailStopChannel was null\n");
        iRet = ERRVAL;
        goto exit;
    }

    if (!padStopLossChannel)
    {
        FPRINTE(stderr,"padStopLossChannel was null\n");
        iRet = ERRVAL;
        goto exit;
    }

    if (!adLow)
    {
        FPRINTE(stderr,"adLow was null\n");
        iRet = ERRVAL;
        goto exit;
    }

    if (!adHigh)
    {
        FPRINTE(stderr,"adHigh was null\n");
        iRet = ERRVAL;
        goto exit;
    }

    *padEntryChannel = NULL;
    *padTrailStopChannel = NULL;
    *padStopLossChannel = NULL;

    if(!((*padEntryChannel = (double*)malloc(iSize*sizeof(double)))))
    {
        FPRINTE(stderr,"malloc *padEntryChannel failed\n");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padTrailStopChannel = (double*)malloc(iSize*sizeof(double)))))
    {
        FPRINTE(stderr,"malloc *padTrailStopChannel failed\n");
        iRet = ERRVAL;
        goto exit;
    }

    if(!((*padStopLossChannel = (double*)malloc(iSize*sizeof(double)))))
    {
        FPRINTE(stderr,"malloc *padStopLossChannel failed\n");
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
