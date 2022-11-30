#include <stdio.h>
#include <string.h>

#include "Marlin.h"
#include "probe.h"
#include "textoutput.h"


// MillCalib:
// - compiles win10(cygwin) and linux (ubuntu)
// - commandline interface, NO GUI
// - goal:
//     one-button-solution to:
//     - measure as many as possible calibration parameters of a mill
//     - measure as accurate as possibel with suitable amount of time
//     - cheap equipment:
//       - selfmade probe (skreq + plate ... 1EUR)
//       - aluminium block <20EUR (100x100x60mm)
//     - store results in human readable form (E.g. CSV)
//     - be able to calibrate Marlin, using the results


// LoP:
// - replace all printf by prtMsg(STATUS_INFO, " ...
//    support argc/argv ...
// - make selection of RS232 interface more comfortable
//     check, which rs232 connections are available (for linux and windows!):
//     win: https://stackoverflow.com/questions/2674048/what-is-proper-way-to-detect-all-available-serial-ports-on-windows
//     linux: search for /dev/ttyUSB<n> ???
// - measure spindle rates: check probeS_res and how I implemented it
//     implement S/W measurement for 45deg block -> compare spindle for x/y direction
// - concept to send gcode for initialization (e.g. set acceleration)
// - check into gitlab
// - video for using
// - support for estlcam firmware



void DevTstMill(Mill *p_mill)
{
    Koord dummypos;
#if 0==1 // test passed
// testing mill->SaveMoveRel
    try
    {
        p_mill->SaveMoveRel(Koord (6,0,0),60, "testerror");
    }
    catch(int val)
    {// this message should be shown, if probe is activated during the 6s move
    // only dbg messages, if move can finish
        printf("error during SaveMoveRel\n");
    }
#endif

#if 1==0 // 
// testing probe moves for SaveMoves
    try
    {
        p_mill->SaveMoveAbs(Koord (6,0,0),60, "testerror");
    }
    catch(int val)
    {// this message should be shown, if probe is activated during the 6s move
    // only dbg messages, if move can finish
        printf("error during SaveMoveAbs\n");
    }
#endif

#if 0==1 // 
// testing probe moves for SaveMoves
    try
    {
        p_mill->ProbeMoveAbs(Koord (6,0,0),dummypos,60, "testerror");
    }
    catch(int val)
    {// this message should be shown, if probe is NOT activated during the 6s move
    // only dbg messages, if probe is activated
        printf("error during ProbeMoveAbs\n");
    }
#endif

#if 1==1 // 
// testing probe moves for SaveMoves
    try
    {
        p_mill->ProbeMoveRel(Koord (6,0,0),dummypos,60, "testerror");
    }
    catch(int val)
    {// this message should be shown, if probe is NOT activated during the 6s move
    // only dbg messages, if probe is activated
        printf("error during ProbeMoveRel\n");
    }
#endif

// if DevTst is activated, exit application after testing
    printf("DevTst finished -> exit()\n");
    exit(1);
}


void Usage(void)
{
	printf("./Marlin_MC -d<debuglevel>\n");
	printf("debuglevel != 0 --> print probe messages\n");
	printf("debuglevel & 1 --> print RS232 messages\n");
}

int g_simul;

int
main(int argc, char *argv[])
{
	Usage();
	
	int ch;

	char gcmd[100];
	size_t strpos;

    Mill *mill;
	Probe *probe;
	Config *config;
	
    g_simul=0;
	if(argc>1)
	{
		if(!strncmp(argv[1],"-d",2))
		{
			debug=atoi(&argv[1][2]);
			printf("enabled debug option\n");
		}
		if(!strncmp(argv[1],"-s",2))
		{
			g_simul=1;
			printf("enabled simulation\n");
		}
	}

    
	config   = new Config();
    
    if(!g_simul)
    {
        printf("connect to MARLIN [ENTER]\n");
        getchar();
        mill = new Marlin(config);
        if (!(mill->connected()))
        {
            delete mill;
            exit(1);
        }
    }

	probe = new Probe(config, mill);


// perform some experiments for Mill control
//    DevTstMill(mill);

	while(1)
	{
		// the following options should be given to the user
		// load blockgeometry<n>.cfg  (e.g. load the file with max <n>)
		// 0) Probe_CompleteAccurate
		//    delete results
		//    probe all 4 directions, starting with 1 to SW
		//    calc all Skews
		// 1-4) Probe_CompleteAccurate
		//    delete results
		//    probe 1 directions (1,2,3 or 4 to SW)
		//    calc all Skews, using blockgeometry (either, the one, which was loaded, or the measured one)
		// x/X,y/Y,z/Z: Backlash in pos/neg x/y/z direction

        printf("\n----------------------------------------------------------\n");
        printf("Select Measurement and press ENTER\n");
        printf("----------------------------------------------------------\n");
        printf("0 [ENTER] Probe_CompleteAccurate (4 meas)\n");
		printf("x/X,y/Y,z/Z: [ENTER] Backlash in pos/neg x/y/z direction\n");
		printf("c [ENTER] send user-command to Marlin\n");
		printf("t [ENTER] test FW control\n");
		
		ch=getchar();
		getchar(); // dummy read to get the enter press

		switch(ch)
		{
		case '0':
			probe->MeasureAndCalc();    // measure all 4 edges to SW in a loop and calculate all the results

// attach results to results.CSV -> track the accuracy.
            probe->SaveMeasResults();
// store blockgeometry into blockgeometry<n>.cfg            
			probe->SaveGeometry();
			break;

		case 'x':
			probe->Backlash(Koord(1.0,0.0,0.0));
			break;
		case 'y':
			probe->Backlash(Koord(0.0,1.0,0.0));
			break;
		case 'z':
			probe->Backlash(Koord(0.0,0.0,1.0));
			break;
		case 'X':
			probe->Backlash(Koord(-1.0,0.0,0.0));
			break;
		case 'Y':
			probe->Backlash(Koord(0.0,-1.0,0.0));
			break;
		case 'Z':
			probe->Backlash(Koord(0.0,0.0,-1.0));
			break;

		case 't':
			probe->Test();
			break;

		case 'c':
// Note: only makes sense, if something can be sent to the mill
// send custom gcode to Firmware
			printf("enter cmd/gcode to be sent to the mill:");
			fgets(&gcmd[0], 100, stdin);
			strpos=0;
			while(gcmd[strpos]!=10 && strpos<strlen(gcmd)) strpos++;
			gcmd[strpos]='\r';
			for(strpos=0;strpos<strlen(gcmd);strpos++)
				printf("%ld) %d ''%c''\n",strpos,gcmd[strpos],gcmd[strpos]);
			printf("ENTER to send the command\n");
			getchar();
			probe->Send2Mill((unsigned char *)&gcmd[0]);
			break;

		default:
			printf("unknown entry:'%c'\n",ch);
		}
	}
	return 0;
}
