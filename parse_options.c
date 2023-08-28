#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "parse_options.h"

int parse_options(int argc, char **argv, char **mvalue, char **ivalue, char **pvalue, char **wvalue)
{
    int index;
    int c;

    opterr = 0;

    while ((c = getopt (argc, argv, "m:i:p:w:")) != -1)
    {
        //printf("optarg = %s\n", optarg);
        size_t szOptArg = sizeof(char) * (strlen(optarg) + 1);
        //printf("sizeof(optarg) = %ld\n", strlen(optarg));
        switch (c)
        {
            case 'm':
                *mvalue = (char *)malloc(szOptArg);
                memset(*mvalue, 0, szOptArg);
                memcpy(*mvalue, optarg, szOptArg);
                // *mvalue[sizeof(optarg)] = '\0';
                // mvalue = optarg;
                break;
            case 'i':
                *ivalue = (char *)malloc(szOptArg);
                memset(*ivalue, 0, szOptArg);
                memcpy(*ivalue, optarg, szOptArg);
                // *ivalue[sizeof(optarg)] = '\0';
                // ivalue = optarg;
                break;
            case 'p':
                *pvalue = (char *)malloc(szOptArg);
                memset(*pvalue, 0, szOptArg);
                memcpy(*pvalue, optarg, szOptArg);
                // *pvalue[sizeof(optarg)] = '\0';
                // pvalue = optarg;
                break;
            case 'w':
                *wvalue = (char *)malloc(szOptArg);
                memset(*wvalue, 0, szOptArg);
                memcpy(*wvalue, optarg, szOptArg);
                // *pvalue[sizeof(optarg)] = '\0';
                // pvalue = optarg;
                break;
            case '?':
                if (optopt == 'c')
                  fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                  fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                  fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return STATUS_ERROR;
            default:
                abort ();
        }
    }
    
	printf ("%s mvalue = %s, ivalue = %s, pvalue = %s, wvalue = %s\n", __FUNCTION__, *mvalue, *ivalue, *pvalue, *wvalue);

  	for (index = optind; index < argc; index++)
    	printf ("Error: Non-option argument %s\n", argv[index]);
    	
  	return STATUS_OK;
}

