#include <stdio.h>
#include <string.h>
#include <error.h>

/*
MAX_BUFFER is the amount to read in from an input file at a time.  Generally,
this should be set as high as possible to achieve the best performance.
*/
#define MAX_BUFFER ~(1<<16) //64K at a time

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
    int iRet = 0;
    int cbLeftover = 0;
    int iIndex;
    char* szDate[8]; //get rid of this magic number
    int open;
    int high;
    int low;
    int close;

    if (!padLow)
    {
        error("padLow was null");
        iRet = -1;
        goto exit;
    }
    if (!padHigh)
    {
        error("padHigh was null");
        iRet = -1;
        goto exit;
    }
    if (!padOpen)
    {
        error("padOpen was null");
        iRet = -1;
        goto exit;
    }
    if (!padClose)
    {
        error("padClose was null");
        iRet = -1;
        goto exit;
    }
    if (!paszDates)
    {
        error("paszDates was null");
        iRet = -1;
        goto exit;
    }
    if (!piSize)
    {
        error("piSize was null");
        iRet = -1;
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
                 comCommodity.year))
    {
        error("snprintf failed");
        iRet = -1;
        goto exit;
    }

    fdInFile = open(szPathname, O_RDONLY);
    if(fdFile == -1)
    {
        error("open failed");
        iRet = -1;
        goto exit;
    }

    //begin reading large chunks from the file
    cbBytesReturned = read(fdInFile,szReadBuf,MAX_BUFFER);
    if(sscanf(szReadBuf,"Date,Open,High,Low,Close") == EOF)
    {
        error("scanf didn't find any headers when it should have");
        iRet = -1;
        goto exit;
    }

    if(!((*padOpen = (double*)malloc(MAX_BUFFER*sizeof(double*)))))
    {
        error("malloc *padOpen failed");
        iRet = -1;
        goto exit;
    }

    if(!((*padHigh = (double*)malloc(MAX_BUFFER*sizeof(double*)))))
    {
        error("malloc *padHigh failed");
        iRet = -1;
        goto exit;
    }

    if(!((*padLow = (double*)malloc(MAX_BUFFER*sizeof(double*)))))
    {
        error("malloc *padLow failed");
        iRet = -1;
        goto exit;
    }

    if(!((*padClose = (double*)malloc(MAX_BUFFER*sizeof(double*)))))
    {
        error("malloc *padClose failed");
        iRet = -1;
        goto exit;
    }

    if(!((*paszDates = (char**)malloc(MAX_BUFFER*sizeof(char**)))))
    {
        error("malloc *paszDates failed");
        iRet = -1;
        goto exit;
    }

    *iSize = 0;
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

                switch(sscanf(szReadBuf,
                              "%8s, %d, %d, %d, %d",
                              &paszDates[iSize],
                              &padOpen[iSize],
                              &padHigh[iSize],
                              &padLow[iSize],
                              &padClose[iSize]))
                {
                case 0: //expected at the end of the file

                    //resize the arrays to just the right size
                    *padOpen = (double*)realloc(*iSize*sizeof(double*));
                    *padHigh = (double*)realloc(*iSize*sizeof(double*));
                    *padLow = (double*)realloc(*iSize*sizeof(double*));
                    *padClose = (double*)realloc(*iSize*sizeof(double*));
                    *paszDates = (char**)realloc(*iSize*sizeof(char**));
                    goto exit;
                    break;
                case 5: //expected
                    //increment the index of the array
                    *iSize++;

                    //set the curLine to be the next line
                    szCurLine = szReadBuf+iIndex+1;
                    break;
                default:
                    error("sscanf returned an incorrect value");
                    iRet = -1;
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
        if (*paszDates)
        {
            free(*paszDates);
        }
    }

    close(fdFile);

    return iRet;
}

int generateChannels(commod comCommodity,
                     double* adLow,
                     double* adHigh,
                     int iEntryWindow,
                     int iTrailStopWindow,
                     int iStopLossWindow,
                     double** padEntryChannel,
                     double** padTrailStopChannel,
                     double** padStopLossChannel)
{
    int i;
    int iRet = 0;

    if (!padEntryChannel)
    {
        error("padEntryChannel was null");
        iRet = -1;
        goto exit;
    }

    if (!padTrailStopChannel)
    {
        error("padTrailStopChannel was null");
        iRet = -1;
        goto exit;
    }

    if (!padStopLossChannel)
    {
        error("padStopLossChannel was null");
        iRet = -1;
        goto exit;
    }

    if (!adLow)
    {
        error("adLow was null");
        iRet = -1;
        goto exit;
    }

    if (!adHigh)
    {
        error("adHigh was null");
        iRet = -1;
        goto exit;
    }

    *padEntryChannel = NULL;
    *padTrailStopChannel = NULL;
    *padStopLossChannel = NULL;

    if(!((*padEntryChannel = (double*)malloc(comCommodity.iSize*sizeof(double*)))))
    {
        error("malloc *padEntryChannel failed");
        iRet = -1;
        goto exit;
    }

    if(!((*padTrailStopChannel = (double*)malloc(comCommodity.iSize*sizeof(double*)))))
    {
        error("malloc *padTrailStopChannel failed");
        iRet = -1;
        goto exit;
    }

    if(!((*padStopLossChannel = (double*)malloc(comCommodity.iSize*sizeof(double*)))))
    {
        error("malloc *padStopLossChannel failed");
        iRet = -1;
        goto exit;
    }

    if (comCommodity.iType == SHORT)
    {
        for(i = 0; i < comCommodity.iSize; i++)
        {
            if(i < iEntryWindow)
            {
                *padEntryChannel[i] = -1;
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
                *padTrailStopChannel[i] = -1;
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
                *padStopLossChannel[i] = -1;
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
        for(i = 0; i < comCommodity.iSize; i++)
        {
            if(i < iEntryWindow)
            {
                *padEntryChannel[i] = -1;
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
                *padTrailStopChannel[i] = -1;
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
                *padStopLossChannel[i] = -1;
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
