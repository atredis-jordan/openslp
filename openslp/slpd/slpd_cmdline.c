/***************************************************************************/
/*                                                                         */
/* Project:     OpenSLP - OpenSource implementation of Service Location    */
/*              Protocol Version 2                                         */
/*                                                                         */
/* File:        slpd_cmdline.c                                             */
/*                                                                         */
/* Abstract:    Simple command line processor                              */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*     Please submit patches to http://www.openslp.org                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*                                                                         */
/* Copyright (C) 2000 Caldera Systems, Inc                                 */
/* All rights reserved.                                                    */
/*                                                                         */
/* Redistribution and use in source and binary forms, with or without      */
/* modification, are permitted provided that the following conditions are  */
/* met:                                                                    */ 
/*                                                                         */
/*      Redistributions of source code must retain the above copyright     */
/*      notice, this list of conditions and the following disclaimer.      */
/*                                                                         */
/*      Redistributions in binary form must reproduce the above copyright  */
/*      notice, this list of conditions and the following disclaimer in    */
/*      the documentation and/or other materials provided with the         */
/*      distribution.                                                      */
/*                                                                         */
/*      Neither the name of Caldera Systems nor the names of its           */
/*      contributors may be used to endorse or promote products derived    */
/*      from this software without specific prior written permission.      */
/*                                                                         */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     */
/* `AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      */
/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   */
/* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CALDERA      */
/* SYSTEMS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, */
/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        */
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;  LOSS OF USE,  */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON       */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT */
/* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   */
/* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    */
/*                                                                         */
/***************************************************************************/

#include "slpd.h"

#if !defined(ETCDIR)
    #define ETCDIR "/etc"
#endif

#if !defined(VARDIR)
    #define VARDIR "/var"
#endif

/*=========================================================================*/
SLPDCommandLine G_SlpdCommandLine;
/* Global variable containing command line options                         */
/*=========================================================================*/

/*=========================================================================*/
void SLPDPrintUsage()
/* Displays available command line options of SLPD                         */
/*=========================================================================*/
{

#ifdef WIN32
    printf("USAGE: slpd -install|-remove|-debug [-d] [-c conf file] [-l log file] [-r reg file] [-v version]\n");
#else
    printf("USAGE: slpd [-d] [-c conf file] [-l log file] [-r reg file] [-v version]\n");
#endif

}


/*=========================================================================*/
int SLPDParseCommandLine(int argc,char* argv[])
/* Must be called to initialize the command line                           */
/*                                                                         */
/* argc (IN) the argc as passed to main()                                  */
/*                                                                         */
/* argv (IN) the argv as passed to main()                                  */
/*                                                                         */
/* Returns  - zero on success.  non-zero on usage error                    */
/*=========================================================================*/
{
    int i;

    /* Set defaults */
    memset(&G_SlpdCommandLine,0,sizeof(SLPDCommandLine));
#ifndef WIN32
    strcpy(G_SlpdCommandLine.cfgfile,SLPD_CONFFILE);
    strcpy(G_SlpdCommandLine.logfile,SLPD_LOGFILE);
    strcpy(G_SlpdCommandLine.regfile,SLPD_REGFILE);
    strcpy(G_SlpdCommandLine.pidfile,SLPD_PIDFILE);
#else
    ExpandEnvironmentStrings(SLPD_CONFFILE,G_SlpdCommandLine.cfgfile,MAX_PATH);
    ExpandEnvironmentStrings(SLPD_LOGFILE,G_SlpdCommandLine.cfgfile,MAX_PATH);
    ExpandEnvironmentStrings(SLPD_REGFILE,G_SlpdCommandLine.cfgfile,MAX_PATH);
    ExpandEnvironmentStrings(SLPD_PIDFILE,G_SlpdCommandLine.cfgfile,MAX_PATH);
    G_SlpdCommandLine.action = -1;
#endif

    G_SlpdCommandLine.detach = 1;

    for(i=1; i<argc; i++)
    {
#ifdef WIN32
        if(strcmp(argv[i],"-install") == 0)
        {
            G_SlpdCommandLine.action = SLPD_INSTALL;
        }
        else if(strcmp(argv[i],"-remove") == 0)
        {
            G_SlpdCommandLine.action = SLPD_REMOVE;
        }
        else if(strcmp(argv[i],"-debug") == 0)
        {
            G_SlpdCommandLine.action = SLPD_DEBUG;
        }
        else
#endif
            if(strcmp(argv[i],"-l") == 0)
        {
            i++;
            if(i >= argc) goto USAGE;
            strncpy(G_SlpdCommandLine.logfile,argv[i],MAX_PATH-1);
        }
        else if(strcmp(argv[i],"-r") == 0)
        {
            i++;
            if(i >= argc) goto USAGE;
            strncpy(G_SlpdCommandLine.regfile,argv[i],MAX_PATH-1);
        }
        else if(strcmp(argv[i],"-c") == 0)
        {
            i++;
            if(i >= argc) goto USAGE;
            strncpy(G_SlpdCommandLine.cfgfile,argv[i],MAX_PATH-1);        
        }
        else if(strcmp(argv[i],"-p") == 0)
        {
            i++;
            if(i >= argc) goto USAGE;
            strncpy(G_SlpdCommandLine.pidfile,argv[i],MAX_PATH-1);        
        }
        else if(strcmp(argv[i],"-d") == 0)
        {
            G_SlpdCommandLine.detach = 0;
        }
        else if((strcmp(argv[i], "-v") == 0) 
                || (strcmp(argv[i], "-V") == 0)
                || (strcmp(argv[i], "--version") == 0)
                || (strcmp(argv[i], "-version") == 0))
        {
#ifdef WIN32
            printf("slpd version: %s\n", SLP_VERSION);
#else /* UNIX */
            printf("slpd version: %s\n", VERSION);
#endif


            /* Show options. */
            printf("compile options:\n"
                   "            debugging     "
#ifdef DEBUG
                   "enabled (--enable-debug) "
#else
                   "disabled"
#endif /* NDEBUG */
                   "\n"
                   "           predicates     "
#ifdef USE_PREDICATES
                   "enabled "
#else 
                   "disabled (--disable-predicates) "
#endif /* USE_PREDICATES */
                   "\n"
                   "  slpv1 compatability     "
#ifdef ENABLE_SLPv1
                   "enabled "
#else
                   "disabled (--enable-slpv1=no)"
#endif /* ENABLE_SLPv1 */
                   "\n"
                  );
            exit(1);
        }
        else
        {
            goto USAGE;
        }
    }

    return 0;

    USAGE:
    SLPDPrintUsage();
    return 1;
}
