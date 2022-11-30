#include <stdio.h>
#include <string.h>

#include "ausw_math.h"
#include "error.h"
#include "probe.h"

#include "meas.h"

#include "textoutput.h"

#define CW 1
#define CCW 2

FILE *g_fp;

extern int g_simul;


// Konstruktor used to initialize all member variables
// p_config provides the dimensions of the probing (blocksize, etc)
// p_mill is the class to control the mill (currently only Marlin is supported)
Probe::Probe(Config *p_config, Mill *p_mill)
{
	int measId;
    int edgeId;

	config = p_config;
	mill  = p_mill;
	
	for (measId=0;measId<4;measId++)
	{
		probeSW_res[measId]=0.0;
		probeNW_res[measId]=0.0;
		probeNE_res[measId]=0.0;
		probeSE_res[measId]=0.0;

		probeWT_res[measId]=0.0;
		probeNT_res[measId]=0.0;
		probeET_res[measId]=0.0;
		probeST_res[measId]=0.0;
    }

	probeS_res=0.0;
	probeW_res=0.0;

	for(edgeId=0;edgeId<4;edgeId++)
	{
		m_phi_block_xy[edgeId]=0.0;
		m_phi_block_z[edgeId]=0.0;
	}
    
	m_phi_xy=0.0;
	m_phi_xz=0.0;
	m_phi_yz=0.0;
    
    m_phi_rotx=0.0;
    m_phi_roty=0.0;
    m_phi_rotz=0.0;
    
}

Probe::~Probe(void)
{
	prtMsg(STATUS_DEBUG, "~Probe destructor");
}

void Probe::Test(void)
{
	Koord l_position,l_res;
	int res=0;
	int speed=120;
	
	printf("if the firmware control does not work as expected, immediately leave the program and clarify first\n");	


/////////////
	printf("getPosition: get position from mill\n");
    mill->getPosition(&l_position);
	printf("X:%.5lf, Y:%.5lf,%.5lf\n",l_position.x,l_position.y,l_position.z);


/////////////
	printf("SaveMoveAbs: move 10mm to east within 10 seconds\n"
			"activate probe within this time to stop movement [enter]\n");
	getchar();
	res=0;
	mill->getPosition(&l_position);
	try{
	mill->SaveMoveAbs(l_position+Koord(10,0,0),speed,"SaveMoveAbs");
	}
	catch(int a)
	{// this is good, the safetymechanism reported a wrongly hit to probe
		res=1;
	}
	if(res==1)
	{
		printf("safetymechanism works fine, probe was detected\n");
	}else{
		printf("SAFETYMECHANISM did not detect probe!!!\nRECONFIGURE firmware!!!!!!! and leave\n");
		getchar();
	}


/////////////
	printf("SaveMoveRel: move 10mm to west within 10 seconds\n"
			"activate probe within this time to stop movement [enter]\n");
	getchar();
	res=0;
	try
	{
	mill->SaveMoveRel(Koord(-10,0,0),speed,"SaveMoveRel");
	}
	catch(int a)
	{// this is good, the safetymechanism reported a wrongly hit to probe
		res=1;
	}
	if(res==1)
	{
		printf("safetymechanism works fine, probe was detected\n");
	}else{
		printf("SAFETYMECHANISM did not detect probe!!!\nRECONFIGURE firmware!!!!!!! and leave\n");
		getchar();
	}


/////////////
	printf("FreeMoveAbs: move 10mm to east within 10 seconds[enter]\n");
	getchar();
	mill->FreeMoveAbs(l_position+Koord(10,0,0),speed,"FreeMoveAbs");

/////////////
	printf("FreeMoveRel: move 10mm to west within 10 seconds[enter]\n");
	getchar();
	mill->FreeMoveRel(Koord(-10,0,0),speed,"FreeMoveRel");


/////////////
	printf("ProbeMoveAbs: move 10mm to east within 10 seconds\n"
			"activate probe within this time to probe position [enter]\n");
	getchar();
	res=0;
    mill->getPosition(&l_position);
	try
	{
		mill->ProbeMoveAbs(l_position+Koord(10,0,0),l_res,speed,"ProbeMoveAbs",0);
	}
	catch(int a)
	{// this is bad, the probe was not hit during move
		res=1;
	}
	if(res==1)
	{
		printf("probe was not activated\nRECONFIGURE firmware!!!!!!! and leave\n");
		getchar();
	}else{
		printf("probe was detected -> all fine\n");
	}
	printf("stopped at X:%.5lf, Y:%.5lf,%.5lf\n",l_res.x,l_res.y,l_res.z);


/////////////
	printf("ProbeMoveRel: move 10mm to west within 10 seconds\n"
			"activate probe within this time to stop movement [enter]\n");
	getchar();
	res=0;
	try
	{
		mill->ProbeMoveRel(Koord(-10,0,0),l_res,speed,"ProbeMoveRel",0);
	}
	catch(int a)
	{// this is bad, the probe was not hit during move
		res=1;
	}
	if(res==1)
	{
		printf("probe was not activated\nRECONFIGURE firmware!!!!!!! and leave\n");
		getchar();
	}else{
		printf("probe was detected -> all fine\n");
	}
	printf("stopped at X:%.5lf, Y:%.5lf,%.5lf\n",l_res.x,l_res.y,l_res.z);


/////////////
	printf("ProbeMoveAbs: Leaving Probe: move 10mm to east within 10 seconds\n"
			"activate probe before movement and leave during it[enter]\n");
	getchar();
    mill->getPosition(&l_position);
	mill->ProbeMoveAbs(l_position+Koord(10,0,0),l_res,speed,"ProbeMoveAbs",0);
	printf("stopped at X:%.5lf, Y:%.5lf,%.5lf\n",l_res.x,l_res.y,l_res.z);


/////////////
	printf("ProbeMoveRel: Leaving Probe: move 10mm to west within 10 seconds\n"
			"activate probe before movement and leave during it[enter]\n");
	getchar();
	mill->ProbeMoveRel(Koord(-10,0,0),l_res,speed,"ProbeMoveRel",0);
	printf("stopped at X:%.5lf, Y:%.5lf,%.5lf\n",l_res.x,l_res.y,l_res.z);
	
	printf("congratulations\n");
}


/*
 store all measured data to blockgeometry%d.cfg",n -> use n++ of last existing file
 m_phi_block_xy[4]
 m_phi_block_z[4]
 probeSW_res[4], etc.
 probeWT_res[4]
 probeBedLevel_res[4]
*/
void Probe::SaveGeometry(void)
{
    
    int cnt;
	int edgeId;
    int measId;
    
	FILE *fp;
	char fname[30];		// last words: "30" should be enough for each and everyone forever :-)

	cnt=0;
	sprintf(fname,"blockgeometry%d.cfg",cnt);
	while((fp=fopen(fname,"r")))
	{
		fclose(fp);
		cnt++;			// increment while file exist
		sprintf(fname,"blockgeometry%d.cfg",cnt);
	}
// fname holds the file to use for save operation
	printf("SaveGeometry: %s\n",fname);
	fp=fopen(fname,"w");

	// version holds the information, which parameters are available
	fprintf(fp,"version1\n");
	for(edgeId=0;edgeId<4;edgeId++)
		fprintf(fp,"m_phi_block_xy%d=%.5f\n",edgeId,m_phi_block_xy[edgeId]);

	for(edgeId=0;edgeId<4;edgeId++)
		fprintf(fp,"m_phi_block_z%d=%.5f\n",edgeId,m_phi_block_z[edgeId]);

	for(measId=0;measId<4;measId++)
		fprintf(fp,"probeSW_res%d=%.5f\n",measId,probeSW_res[measId]);
	for(measId=0;measId<4;measId++)
		fprintf(fp,"probeNW_res%d=%.5f\n",measId,probeNW_res[measId]);
	for(measId=0;measId<4;measId++)
		fprintf(fp,"probeNE_res%d=%.5f\n",measId,probeNE_res[measId]);
	for(measId=0;measId<4;measId++)
		fprintf(fp,"probeSE_res%d=%.5f\n",measId,probeSE_res[measId]);

	for(measId=0;measId<4;measId++)
		fprintf(fp,"probeWT_res%d=%.5f\n",measId,probeWT_res[measId]);
	for(measId=0;measId<4;measId++)
		fprintf(fp,"probeNT_res%d=%.5f\n",measId,probeNT_res[measId]);
	for(measId=0;measId<4;measId++)
		fprintf(fp,"probeET_res%d=%.5f\n",measId,probeET_res[measId]);
	for(measId=0;measId<4;measId++)
		fprintf(fp,"probeST_res%d=%.5f\n",measId,probeST_res[measId]);

	fprintf(fp,"probeBedLevel_res1=%.5f\n",probeBedLevel_res[0]);
	fprintf(fp,"probeBedLevel_res2=%.5f\n",probeBedLevel_res[1]);
	fprintf(fp,"probeBedLevel_res3=%.5f\n",probeBedLevel_res[2]);
	fprintf(fp,"probeBedLevel_res4=%.5f\n",probeBedLevel_res[3]);

	fclose(fp);
}


/* load measured data from the latest blockgeometry%d.cfg file
  see SaveGeometry for further details */
void Probe::LoadGeometry(void)
{
	// store all measured data to blockgeometry%d.cfg",n -> use n++ of last existing file
	// m_phi_block_xy[4]
	// m_phi_block_z[4]
	// probeSW_res[4], etc.
	// probeWT_res[4]
    // probeBedLevel_res[4]

    int cnt;
    int edgeId;
	int measId;
    
	FILE *fp;
	char fname[30];		// last words: "30" should be enough for each and everyone forever :-)
	char linebuf[256];

	cnt=0;
	sprintf(fname,"blockgeometry%d.cfg",cnt);
	while((fp=fopen(fname,"r")))
	{
		fclose(fp);
		cnt++;			// increment while file exist
		sprintf(fname,"blockgeometry%d.cfg",cnt);
	}
	cnt--;
	sprintf(fname,"blockgeometry%d.cfg",cnt);

// fname holds the file to use for load operation
	fp=fopen(fname,"r");

	// version holds the information, which parameters are available
	fgets(linebuf,255,fp);
	if(strcmp(linebuf,"version1\n"))
	{
        // no error handling implemented ...
		printf("unsupported fileversion for geometry: %s: #%s#\n",fname,linebuf);
		return;
	}

	for(edgeId=0;edgeId<4;edgeId++)
		fscanf(fp,"m_phi_block_xy%d=%lg\n",&edgeId,&m_phi_block_xy[edgeId]);

	for(edgeId=0;edgeId<4;edgeId++)
		fscanf(fp,"m_phi_block_z%d=%lg\n",&edgeId,&m_phi_block_z[edgeId]);

	for(measId=0;measId<4;measId++)
		fscanf(fp,"probeSW_res%d=%lg\n",&measId,&probeSW_res[measId]);

	for(measId=0;measId<4;measId++)
		fscanf(fp,"probeNW_res%d=%lg\n",&measId,&probeNW_res[measId]);

	for(measId=0;measId<4;measId++)
		fscanf(fp,"probeNE_res%d=%lg\n",&measId,&probeNE_res[measId]);

	for(measId=0;measId<4;measId++)
		fscanf(fp,"probeSE_res%d=%lg\n",&measId,&probeSE_res[measId]);

	for(measId=0;measId<4;measId++)
		fscanf(fp,"probeWT_res%d=%lg\n",&measId,&probeWT_res[measId]);

	for(measId=0;measId<4;measId++)
		fscanf(fp,"probeNT_res%d=%lg\n",&measId,&probeNT_res[measId]);

	for(measId=0;measId<4;measId++)
		fscanf(fp,"probeET_res%d=%lg\n",&measId,&probeET_res[measId]);

	for(measId=0;measId<4;measId++)
		fscanf(fp,"probeST_res%d=%lg\n",&measId,&probeST_res[measId]);


	fscanf(fp,"probeBedLevel_res1=%lg\n",&probeBedLevel_res[0]);
	fscanf(fp,"probeBedLevel_res2=%lg\n",&probeBedLevel_res[1]);
	fscanf(fp,"probeBedLevel_res3=%lg\n",&probeBedLevel_res[2]);
	fscanf(fp,"probeBedLevel_res4=%lg\n",&probeBedLevel_res[3]);

	fclose(fp);

	printGeometry();
}


/* attach measured data to measresults.CSV
 blockgeometry:
  m_phi_block_xy0 4 values
  m_phi_block_z0 4 values
 calibration data:
  xy_skew_factor, m_phi_xy, dev_mm 3 val
  xy_skew_factor, m_phi_xy, dev_mm 3 val
  xz_skew_factor, m_phi_xz, dev_mm 3 val
  m_phi_rotx,y,z 3 values

Headline of CSV:
 block_xy1,block_xy2,block_xy3,block_xy4,block_z1,block_z2,block_z3,block_z4,XYskew,phi_XYskew,mm_m_XYskew,XZskew,phi_XZskew,mm_m_XZskew,YZskew,phi_YZskew,mm_m_YZskew,rotx,mm_m_rotx,roty,mm_m_roty,rotz,mm_m_rotz
*/
void Probe::SaveMeasResults(void)
{
    int edgeId;
    
    // attach measurement results to CSV file
	FILE *fp;

	fp=fopen("measresults.CSV","a");
    
	for(edgeId=0;edgeId<4;edgeId++)
		fprintf(fp,"%.5f,",m_phi_block_xy[edgeId]);

	for(edgeId=0;edgeId<4;edgeId++)
		fprintf(fp,"%.5f,",m_phi_block_z[edgeId]);
    
    double xyskew_factor = tan(PI/2 - PI / 180.0 * m_phi_xy);
	double xyskew_mm_m   = 1000*sin(PI / 180.0 * (90.0 - m_phi_xy));
    fprintf(fp,"%.5f,",xyskew_factor);
    fprintf(fp,"%.5f,",m_phi_xy);
    fprintf(fp,"%.5f,",xyskew_mm_m);

    double xzskew_factor = tan(PI/2 - PI / 180.0 * m_phi_xz);
	double xzskew_mm_m   = 1000*sin(PI / 180.0 * (90.0 - m_phi_xz));
    fprintf(fp,"%.5f,",xzskew_factor);
    fprintf(fp,"%.5f,",m_phi_xz);
    fprintf(fp,"%.5f,",xzskew_mm_m);

    double yzskew_factor = tan(PI/2 - PI / 180.0 * m_phi_yz);
	double yzskew_mm_m   = 1000*sin(PI / 180.0 * (90.0 - m_phi_yz));
    fprintf(fp,"%.5f,",yzskew_factor);
    fprintf(fp,"%.5f,",m_phi_yz);
    fprintf(fp,"%.5f,",yzskew_mm_m);

	double rotx_mm_m   = 1000*sin(PI / 180.0 * m_phi_rotx);
    fprintf(fp,"%.5f,",m_phi_rotx);
    fprintf(fp,"%.5f,",rotx_mm_m);

	double roty_mm_m   = 1000*sin(PI / 180.0 * m_phi_roty);
    fprintf(fp,"%.5f,",m_phi_roty);
    fprintf(fp,"%.5f,",roty_mm_m);

	double rotz_mm_m   = 1000*sin(PI / 180.0 * m_phi_rotz);
    fprintf(fp,"%.5f,",m_phi_rotz);
    fprintf(fp,"%.5f",rotz_mm_m);
    fprintf(fp,"\n");

    fclose(fp);
}

/* print geometry data to screen, e.g. after loading it, after measurement is complete, etc. */
void Probe::printGeometry(void)
{
  if(debug){
      
	int edgeId;
    int measId;
    
	for(edgeId=0;edgeId<4;edgeId++)
		printf("m_phi_block_xy%d=%lg\n",edgeId,m_phi_block_xy[edgeId]);

	for(edgeId=0;edgeId<4;edgeId++)
		printf("m_phi_block_z%d=%lg\n",edgeId,m_phi_block_z[edgeId]);

    if(debug)
    {
        for(measId=0;measId<4;measId++)
            printf("probeSW_res%d=%lg\n",measId,probeSW_res[measId]);

        for(measId=0;measId<4;measId++)
            printf("probeNW_res%d=%lg\n",measId,probeNW_res[measId]);

        for(measId=0;measId<4;measId++)
            printf("probeNE_res%d=%lg\n",measId,probeNE_res[measId]);

        for(measId=0;measId<4;measId++)
            printf("probeSE_res%d=%lg\n",measId,probeSE_res[measId]);

        for(measId=0;measId<4;measId++)
            printf("probeWT_res%d=%lg\n",measId,probeWT_res[measId]);

        for(measId=0;measId<4;measId++)
            printf("probeNT_res%d=%lg\n",measId,probeNT_res[measId]);

        for(measId=0;measId<4;measId++)
            printf("probeET_res%d=%lg\n",measId,probeET_res[measId]);

        for(measId=0;measId<4;measId++)
            printf("probeST_res%d=%lg\n",measId,probeST_res[measId]);
    }
    prtMsg(STATUS_INFO, "-----------------------------------------------------------------------\n");
  }
}



/* probe in the direction of dir
  if probe contacts block, drive in oposite direction and measure the distance which is needed
  to uncontact block

  length of the dir vector (abs(dir)) should be normalized to 1.0*/
void Probe::Backlash(Koord dir)
{
	int Fprobe     = config->get_fspeed_probe_mmmin();

	Koord antastenpos;
	Koord abtastenpos;
	double backlash;

	mill->ProbeMoveRel(dir*config->get_free_xy_mm(), antastenpos, Fprobe, "Backlash antasten");
	mill->ProbeMoveRel(-dir*config->get_free_xy_mm(), abtastenpos, Fprobe, "Backlash abtasten",1);
	mill->FreeMoveRel(-dir*config->get_dist_xy_mm(), Fprobe, "Backlash move away");

	backlash=Koord::absKoord(abtastenpos-antastenpos);

	printf("dir: %.3f,%.3f,%.3f -> backlash: %.3f\n",dir.x,dir.y,dir.z,backlash);
	printf("M425 %c%.3f\n",abs(dir.x)>abs(dir.y)?'x':'y',backlash);
}



/* probe the 4 top corners of the block and the flats on the side
  store the resulting coordinates in m_posarr[measId][16]*/
void Probe::Meas_Block(int measId)
{
    Koord posarr[16];

	double distxy  = config->get_dist_xy_mm();
	double distz   = config->get_dist_z_mm();
	double probexy    = config->get_diamProbe_mm()/2.0 + config->get_free_xy_mm() + config->get_dist_xy_mm();
	double probez     = config->get_dist_z_mm()+config->get_probe_depth_mm();
	int Fprobe     = config->get_fspeed_probe_mmmin();
	int Fmove      = config->get_fspeed_move_mmmin();

	Koord dpos;		// used for the dummy measurements, which perform the "rough" probing


	// config->get_probelen_xy_mm();
	double blkx    = config->get_plate_sizex_mm() - 2*config->get_free_xy_mm();
	// config->get_probelen_xy_mm();
	double blky    = config->get_plate_sizey_mm() - 2*config->get_free_xy_mm();

	Koord l_startpos;

// in any case of error during the measurement,
// go to top of the block (z=l_startpos.z)
    try {
        // store position, where we started to go back there at the end (SW above block)
        mill->getPosition(&l_startpos);
    }
    catch(int val)
    {
        prtMsg(STATUS_ERROR, "Probe::Meas_Block(): getPosition() failed");
//        throw 0:
//        return;
    }


    if(debug)
        printf("startpos: %lf,%lf,%lf\n",l_startpos.x,l_startpos.y,l_startpos.z);


    try {
// probe is used like endstop, if probe unexpectatly hits the block
// ProbeMoveRel: all moves, which measure, when the probe contacts the block (each corner is measured 5 times, the 1st 2 times to position the final 3 measurement)
// FreeMoveRel: move away from block
// SaveMoveRel: move arround the block, stop moving, when probe gets active (in this case handle as error!)
// SaveMoveAbs: move back to startposition, stop moving, when probe gets active (in this case handle as error!)
    // probeZ
        mill->ProbeMoveRel(Koord(0.0, 0.0, -probez), dpos, Fprobe, "probeSW T1");
        mill->FreeMoveRel( Koord(0.0, 0.0, distz), Fmove,	"move U");
        mill->SaveMoveRel( Koord(0.0, -probexy, 0.0), Fmove,	"move S");
        mill->SaveMoveRel( Koord(0.0, 0.0, -probez),  Fmove,	"move D");

        //SW corner
        mill->ProbeMoveRel(Koord(0.0, probexy, 0.0), dpos, Fprobe, "probeSW N1");
        mill->FreeMoveRel( Koord(0.0,-distxy,0.0), Fmove,	"moveSW S");
        mill->SaveMoveRel( Koord(-probexy, 0.0, 0.0), Fmove,	"moveSW W");
        mill->SaveMoveRel( Koord(0.0, probexy,  0.0), Fmove,	"moveSW N");
        mill->ProbeMoveRel(Koord(probexy, 0.0, 0.0), dpos, Fprobe, "probeSW E1");
        mill->FreeMoveRel( Koord(-distxy, 0.0, 0.0), Fmove,	"moveSW W");
        mill->SaveMoveRel( Koord(0.0, 0.0,probez), Fmove,	"moveSW U");
        mill->SaveMoveRel( Koord(probexy, 0.0, 0.0), Fmove,	"moveSW E");
        mill->ProbeMoveRel(Koord(0.0,0.0, -probez), posarr[0], Fprobe, "probeSW T2");
        mill->FreeMoveRel( Koord(0.0, 0.0, distz), Fmove,	"moveSW U2");
        mill->SaveMoveRel( Koord(0.0, -probexy, 0.0), Fmove,	"moveSW S2");
        mill->SaveMoveRel( Koord(0.0, 0.0, -probez),  Fmove,	"moveSW D2");
        mill->ProbeMoveRel(Koord(0.0, probexy, 0.0), posarr[15], Fprobe, "probeSW N2");
        mill->FreeMoveRel( Koord(0.0,-distxy,0.0), Fmove,	"moveSW S2");
        mill->SaveMoveRel( Koord(-probexy, 0.0, 0.0), Fmove,	"moveSW W2");
        mill->SaveMoveRel( Koord(0.0, probexy, 0.0), Fmove,	"moveSW N2");
        mill->ProbeMoveRel(Koord(probexy, 0.0, 0.0), posarr[4], Fprobe, "probeSW E2");
        mill->FreeMoveRel( Koord(-distxy, 0.0, 0.0), Fmove,	"moveSW W");
        mill->SaveMoveRel( Koord(0.0, blky, 0.0), Fmove,	"moveSW N");

        //NW corner
        mill->ProbeMoveRel(Koord(probexy, 0.0, 0.0), dpos, Fprobe, "probeNW E1");
        mill->FreeMoveRel( Koord(-distxy, 0.0 ,0.0), Fmove,	"moveNW W");
        mill->SaveMoveRel( Koord(0.0, probexy, 0.0), Fmove,	"moveNW N");
        mill->SaveMoveRel( Koord(probexy, 0.0, 0.0), Fmove,	"moveNW E");
        mill->ProbeMoveRel(Koord(0.0, -probexy, 0.0), dpos, Fprobe, "probeNW S1");
        mill->FreeMoveRel( Koord(0.0, distxy, 0.0), Fmove,	"moveNW N");
        mill->SaveMoveRel( Koord(0.0, 0.0, probez), Fmove,	"moveNW U");
        mill->SaveMoveRel( Koord(0.0, -probexy, 0.0), Fmove,	"moveNW S");
        mill->ProbeMoveRel(Koord(0.0,0.0, -probez), posarr[1], Fprobe, "probeSW T2");
        mill->FreeMoveRel( Koord(0.0, 0.0, distz), Fmove,	"moveNW U2");
        mill->SaveMoveRel( Koord(-probexy, 0.0, 0.0), Fmove,	"moveNW W2");
        mill->SaveMoveRel( Koord(0.0, 0.0, -probez),  Fmove,	"moveNW D2");
        mill->ProbeMoveRel(Koord(probexy, 0.0, 0.0), posarr[6], Fprobe, "probeNW E2");
        mill->FreeMoveRel( Koord(-distxy, 0.0, 0.0), Fmove,	"moveNW W2");
        mill->SaveMoveRel( Koord(0.0, probexy, 0.0), Fmove,	"moveNW N2");
        mill->SaveMoveRel( Koord(probexy, 0.0, 0.0), Fmove,	"moveNW E2");
        mill->ProbeMoveRel(Koord(0.0, -probexy, 0.0), posarr[7], Fprobe, "probeNW S2");
        mill->FreeMoveRel( Koord(0.0, distxy, 0.0), Fmove,	"moveNW N");
        mill->SaveMoveRel( Koord(blkx, 0.0, 0.0), Fmove,	"moveNW E");

        //NE corner
        mill->ProbeMoveRel(Koord(0.0, -probexy, 0.0), dpos, Fprobe, "probeNE S1");
        mill->FreeMoveRel( Koord(0.0, distxy,0.0), Fmove,	"moveNE N");
        mill->SaveMoveRel( Koord(probexy, 0.0, 0.0), Fmove,	"moveNE E");
        mill->SaveMoveRel( Koord(0.0, -probexy,  0.0), Fmove,	"moveNE S");
        mill->ProbeMoveRel(Koord(-probexy,0.0, 0.0), dpos, Fprobe, "probeNE W1");
        mill->FreeMoveRel( Koord(distxy, 0.0, 0.0), Fmove,	"moveNE E");
        mill->SaveMoveRel( Koord(0.0, 0.0, probez), Fmove,	"moveNE U");
        mill->SaveMoveRel( Koord(-probexy, 0.0, 0.0), Fmove,	"moveNE W");
        mill->ProbeMoveRel(Koord(0.0, 0.0, -probez), posarr[2], Fprobe, "probeNE T2");
        mill->FreeMoveRel( Koord(0.0, 0.0, distz), Fmove,	"moveNE U2");
        mill->SaveMoveRel( Koord(0.0, probexy, 0.0), Fmove,	"moveNE N2");
        mill->SaveMoveRel( Koord(0.0, 0.0, -probez),  Fmove,	"moveNE D2");
        mill->ProbeMoveRel(Koord(0.0, -probexy, 0.0), posarr[9], Fprobe, "probeNE S2");
        mill->FreeMoveRel( Koord(0.0, distxy,0.0), Fmove,	"moveNE N2");
        mill->SaveMoveRel( Koord(probexy, 0.0, 0.0), Fmove,	"moveNE E2");
        mill->SaveMoveRel( Koord(0.0, -probexy, 0.0), Fmove,	"moveNE S2");
        mill->ProbeMoveRel(Koord(-probexy, 0.0, 0.0), posarr[10], Fprobe, "probeNE W2");
        mill->FreeMoveRel( Koord(distxy, 0.0, 0.0), Fmove,	"moveNE E");
        mill->SaveMoveRel( Koord(0.0, -blky, 0.0), Fmove,	"moveNE S");

        //SE corner
        mill->ProbeMoveRel(Koord(-probexy, 0.0, 0.0), dpos, Fprobe, "probeSE W1");
        mill->FreeMoveRel( Koord(distxy, 0.0 ,0.0), Fmove,	"moveSE E");
        mill->SaveMoveRel( Koord(0.0, -probexy, 0.0), Fmove,	"moveSE S");
        mill->SaveMoveRel( Koord(-probexy, 0.0, 0.0), Fmove,	"moveSE W");
        mill->ProbeMoveRel(Koord(0.0, probexy, 0.0), dpos, Fprobe, "probeSE N1");
        mill->FreeMoveRel( Koord(0.0, -distxy, 0.0), Fmove,	"moveSE S");
        mill->SaveMoveRel( Koord(0.0, 0.0, probez), Fmove,	"moveSE U");
        mill->SaveMoveRel( Koord(0.0, probexy, 0.0), Fmove,	"moveSE N");
        mill->ProbeMoveRel(Koord(0.0,0.0, -probez), posarr[3], Fprobe, "probeSE T2");
        mill->FreeMoveRel( Koord(0.0, 0.0, distz), Fmove,	"moveSE U2");
        mill->SaveMoveRel( Koord(probexy, 0.0, 0.0), Fmove,	"moveSE E2");
        mill->SaveMoveRel( Koord(0.0, 0.0, -probez),  Fmove,	"moveSE D2");
        mill->ProbeMoveRel(Koord(-probexy, 0.0, 0.0), posarr[12], Fprobe, "probeSE W2");
        mill->FreeMoveRel( Koord(distxy, 0.0, 0.0), Fmove,	"moveSE E2");
        mill->SaveMoveRel( Koord(0.0, -probexy, 0.0), Fmove,	"moveSE S2");
        mill->SaveMoveRel( Koord(-probexy, 0.0, 0.0), Fmove,	"moveSE W2");
        mill->ProbeMoveRel(Koord(0.0, probexy, 0.0), posarr[13], Fprobe, "probeSE N2");
        mill->FreeMoveRel( Koord(0.0, -distxy, 0.0), Fmove,	"moveSE S");

        // S
        mill->SaveMoveAbs((posarr[13] + posarr[15])/2.0 + Koord(0.0,-distxy, -config->get_probelen_z_mm()),Fmove,"moveZ S");
        // next one was strange ... seemed to not only drive to y-direction, but x/z as well?
        mill->ProbeMoveRel(Koord(0.0, probexy, 0.0), posarr[14], Fprobe, "probeZ N");
        mill->FreeMoveRel( Koord(0.0, -distxy, 0.0), Fmove,	"move S");
        mill->SaveMoveRel( Koord(-blkx/2.0 - probexy, 0.0, config->get_probelen_z_mm()), Fmove,	"move WT");

        // W
        mill->SaveMoveAbs((posarr[4] + posarr[6])/2.0 + Koord(-distxy, 0.0, -config->get_probelen_z_mm()),Fmove,"moveZ W");
        mill->ProbeMoveRel(Koord(probexy, 0.0, 0.0), posarr[5], Fprobe, "probeZ E");
        mill->FreeMoveRel( Koord(-distxy, 0.0, 0.0), Fmove,	"move W");
        mill->SaveMoveRel( Koord(0.0, blky/2.0 + probexy, config->get_probelen_z_mm()), Fmove,	"move NT");

        // N
        mill->SaveMoveAbs((posarr[7] + posarr[9])/2.0 + Koord(0.0, distxy, -config->get_probelen_z_mm()),Fmove,"moveZ N");
        mill->ProbeMoveRel(Koord(0.0, -probexy, 0.0), posarr[8], Fprobe, "probeZ S");
        mill->FreeMoveRel( Koord(0.0, distxy, 0.0), Fmove,	"move N");
        mill->SaveMoveRel( Koord(blkx/2.0 + probexy, 0.0, config->get_probelen_z_mm()), Fmove,	"move ET");

        // E
        mill->SaveMoveAbs((posarr[10] + posarr[12])/2.0 + Koord(distxy, 0.0, -config->get_probelen_z_mm()),Fmove,"moveZ E");
        mill->ProbeMoveRel(Koord(-probexy, 0.0, 0.0), posarr[11], Fprobe, "probeZ W");
        mill->FreeMoveRel( Koord(distxy, 0.0, 0.0), Fmove,	"move E");

    // move zposition above the block
        mill->SaveMoveRel( Koord(0.0, 0.0, probez + config->get_probelen_z_mm()), Fmove,	"move U");


    // move back to the startposition
        mill->SaveMoveAbs( l_startpos, Fmove,	"move home");
    }
    catch(int val)
    {
// if any move fails (e.g. probe does not reach block or probe is hit for Save move)
//   - move probe up to Z==l_startpos.z, and than to l_startpos.x/y
//   - ask user to increase dist_xy_mm and dist_z_mm
//   - Than exit
        prtMsg(STATUS_INFO,"error during probing, press ENTER to move up, than ENTER to move back to start\n");
        prtMsg(STATUS_ERROR,"it is a good idea to quit the application and restart\n");

		#if 0==1// fix this... it does not work, current coordinate unknown!
        getchar();
        mill->SaveMoveRel( Koord(0.0, 0.0, probez + config->get_probelen_z_mm()), Fmove,	"move U");
        getchar();
    // move back to the startposition
        mill->SaveMoveAbs( l_startpos, Fmove,"move home");
		#endif
		
    }

    // copy results to member data
    for(int n=0;n<16;n++)
        m_posarr[measId][n]=posarr[n];
}


/* calculate the norm vectors for all 5 measured flats (W,N,E,S,T) */
void Probe::calc_NormVectors(void)
{
    for(int measId=0;measId<4;measId++)
    {
		probe_nW[measId] = Koord::crossProd(m_posarr[measId][6]-m_posarr[measId][4], m_posarr[measId][5]-m_posarr[measId][4]);
		probe_nN[measId] = Koord::crossProd(m_posarr[measId][9]-m_posarr[measId][7], m_posarr[measId][8]-m_posarr[measId][7]);
		probe_nE[measId] = Koord::crossProd(m_posarr[measId][12]-m_posarr[measId][10], m_posarr[measId][11]-m_posarr[measId][10]);
		probe_nS[measId] = Koord::crossProd(m_posarr[measId][15]-m_posarr[measId][13], m_posarr[measId][14]-m_posarr[measId][13]);
		probe_nT[measId] = (Koord::crossProd(m_posarr[measId][2]-m_posarr[measId][0], m_posarr[measId][1]-m_posarr[measId][0])+
				Koord::crossProd(m_posarr[measId][3]-m_posarr[measId][1], m_posarr[measId][2]-m_posarr[measId][1])+
				Koord::crossProd(m_posarr[measId][0]-m_posarr[measId][2], m_posarr[measId][3]-m_posarr[measId][2])+
				Koord::crossProd(m_posarr[measId][1]-m_posarr[measId][3], m_posarr[measId][0]-m_posarr[measId][3])
                )/4.0;
    }
}

/* calculate the measured angles of all available edges, e.g. SW,NW,NE,SE, WT, NT, ET, ST */
void Probe::calc_Angles(void)
{
		// 16 measurements (0-f), TOP view of the block
		//   -------
		//   |  8  |
		//   |7   9|
		//-------------
		//| 6|1   2|a |
		//|5 |     | b|
		//| 4|0   3|c |
		//-------------
		//   |f   d|
		//   |  e  |
		//   -------
		// calculate all the angles for xyskew from 8 measurements all done on same height
    for(int measId=0;measId<4;measId++)
    {
		probeSW_res[measId] = angle(m_posarr[measId][13]-m_posarr[measId][15], m_posarr[measId][6]-m_posarr[measId][4]);
		probeNW_res[measId] = angle(m_posarr[measId][4]-m_posarr[measId][6], m_posarr[measId][9]-m_posarr[measId][7]);
		probeNE_res[measId] = angle(m_posarr[measId][7]-m_posarr[measId][9], m_posarr[measId][12]-m_posarr[measId][10]);
		probeSE_res[measId] = angle(m_posarr[measId][10]-m_posarr[measId][12], m_posarr[measId][15]-m_posarr[measId][13]);

       	// calculate angles for xz and yzskew
        // -nT is used to calculate the inner angle
		probeWT_res[measId] = angle(probe_nW[measId],-probe_nT[measId]);
		probeNT_res[measId] = angle(probe_nN[measId],-probe_nT[measId]);
		probeET_res[measId] = angle(probe_nE[measId],-probe_nT[measId]);
		probeST_res[measId] = angle(probe_nS[measId],-probe_nT[measId]);
    }
}




void Probe::calc_BlockGeometry(void)
{
    calc_BlockGeometryxy();
    calc_BlockGeometryz();
}

void Probe::calc_BlockGeometryz(void)
{
	double phi_xz[4];	// [id][dir]. id is the identifier for the edge of the plate, which was pointing to SW, when measurement was done.
	double phi_yz[4];	// [id][dir]. id is the identifier for the edge of the plate.
	double phi_blockz[4][2];	// phi is angle between top-surface and the side of the block. This is given for each edge and for each direction in which it was measured
	
	int edgeId,dir;		// loop counter for the 4 edges of the plate and the for directions under which it were measured
	int cnt;
	int cntdir;

	
	// Berechne phi_xz aus Z_W und Z_E Probe
	// Berechne phi_yz aus Z_S und Z_N Probe
	for(edgeId=0;edgeId<4;edgeId++)
	{
        // W/E usage for xzskew
        phi_blockz[edgeId][0]=.5*(probeWT_res[edgeId]+probeET_res[(edgeId+2)%4]);
        phi_xz[edgeId]=90.0 + 0.5 * (probeWT_res[edgeId]-probeET_res[(edgeId+2)%4]);

		// S/N usage for yzskew
        phi_blockz[edgeId][1]=.5*(probeST_res[(edgeId+1)%4]+probeNT_res[(edgeId+3)%4]);
        phi_yz[edgeId]=90.0 + 0.5 * (probeST_res[(edgeId+1)%4]-probeNT_res[(edgeId+3)%4]);
	}

    m_phi_xz=0.0;
    m_phi_yz = 0.0;
    
	cnt=0;
	for (edgeId=0;edgeId<4;edgeId++)
	{
		cntdir=0;
		m_phi_block_z[edgeId]=0.0;
		for(dir=0;dir<2;dir++)
		{
            if(debug)
            {
                printf("%d, %d, %.4f, %.4f, %.4f\n",edgeId, dir, phi_blockz[edgeId][dir],phi_xz[edgeId],phi_yz[edgeId]);
            }
            m_phi_xz    += phi_xz[edgeId];
            m_phi_yz    += phi_yz[edgeId];
            cnt++;
            m_phi_block_z[edgeId]+=phi_blockz[edgeId][dir];
            cntdir++;
		}
		m_phi_block_z[edgeId]/=cntdir;
	}
	m_phi_xz    /= (double)cnt;
	m_phi_yz    /= (double)cnt;

}
    

/* calculate edge-angel and skew-angle of an edge of the block,
which was measured in two different directions, e.g. SW and NW, etc.*/
void Probe::meas2geo(double phi1, double phi2, double &phi_block, double &phi_xy)
{
	phi_block = .5 * (phi2+phi1);
	phi_xy    = 90.0 + 0.5 * (phi2-phi1);
}



void Probe::calc_BlockGeometryxy(void)
{
	double phi_xy[4][4];	// [id][dir]. id is the identifier for the edge of the plate. dir is the direction in which the edge was measured
	double phi_block_xy[4][4];
	
	int dirId;		// loop counter for the 4 edges of the plate and the for directions under which it were measured
	int cnt,cntdir;

    int edgeId;

// statistics

	for(edgeId=0;edgeId<4;edgeId++)
	{   // phi_block_xy, phi_xy are set inside
// calculate (phi1+phi2)/2 as phi_block_xy (blockgeometry)
// calculate difference as phi_xy (skew)
		meas2geo(probeSW_res[edgeId],probeNW_res[(edgeId+3)%4], phi_block_xy[edgeId][0],phi_xy[edgeId][0]);
		meas2geo(probeNE_res[(edgeId+2)%4],probeNW_res[(edgeId+3)%4], phi_block_xy[edgeId][1],phi_xy[edgeId][1]);
		meas2geo(probeNE_res[(edgeId+2)%4],probeSE_res[(edgeId+1)%4], phi_block_xy[edgeId][2],phi_xy[edgeId][2]);
		meas2geo(probeSW_res[edgeId],probeSE_res[(edgeId+1)%4], phi_block_xy[edgeId][3],phi_xy[edgeId][3]);
	}

    if(debug)
    {
        for (edgeId=0;edgeId<4;edgeId++)
        {
            printf("probeSW_res[%d]=%.5lf\n",edgeId,probeSW_res[edgeId]);
            printf("probeNW_res[%d]=%.5lf\n",edgeId,probeNW_res[edgeId]);
            printf("probeNE_res[%d]=%.5lf\n",edgeId,probeNE_res[edgeId]);
            printf("probeSE_res[%d]=%.5lf\n",edgeId,probeSE_res[edgeId]);

            for (dirId=0;dirId<4;dirId++)
            {
                printf("phi_block_xy[%d][%d]=%.5lf\n",edgeId,dirId,phi_block_xy[edgeId][dirId]);
                printf("phi_xy[%d][%d]=%.5lf\n",edgeId,dirId,phi_xy[edgeId][dirId]);
            }
        }
    }

	prtMsg(STATUS_DEBUG, "edge, dir, phiblk_xy, phi_xy \n");
	meanphi_block_all_xy = m_phi_xy = 0.0;
	cnt=0;
	for (edgeId=0;edgeId<4;edgeId++)
	{
		m_phi_block_xy[edgeId]=0.0;
		cntdir=0;
		for(dirId=0;dirId<4;dirId++)
		{
			if(debug)
            {
				const char *dirstr[]={"SW","NW","NE","SE"};
				printf("%d, %s, %.4f, %.4f\n"
						,edgeId
						,dirstr[dirId]
						,phi_block_xy[edgeId][dirId]
						,phi_xy[edgeId][dirId]);
            }
            // just for some statistics (overall meanvalue should be near to 90deg)
            meanphi_block_all_xy +=phi_block_xy[edgeId][dirId];
            m_phi_block_xy[edgeId]+=phi_block_xy[edgeId][dirId];
            cntdir++;
            m_phi_xy    += phi_xy[edgeId][dirId];
            cnt++;
		}
		// store true geometries of the block here:
		m_phi_block_xy[edgeId]/=cntdir;
	}
	meanphi_block_all_xy /= (double)cnt;
	m_phi_xy       /= (double)cnt;
    
}



void Probe::calc_Skews(void)
{
    double dev_mm;
    double xy_skew_factor;
	double xz_skew_factor;
	double yz_skew_factor;

	dev_mm         = 1000*sin(PI / 180.0 * (90.0 - m_phi_xy));
	xy_skew_factor = tan(PI/2 - PI / 180.0 * m_phi_xy);

    prtMsg(STATUS_INFO, "The following value should be near to 90.0000 depending on the accuracy of the measurement\n");
    printf("mean value of the 4 measured angles in xy orientation: %.5f\n",meanphi_block_all_xy);
    
    prtMsg(STATUS_INFO, "\nXYSKEW:\n");
	printf("phi_skew=%.5f, dev_mm=%.3f, xy_skew_factor=%.6f\n"
                ,m_phi_xy,dev_mm,xy_skew_factor);
    prtMsg(STATUS_INFO, "For correction of marlin firmware add the following to Configuration.h:\n");
	printf("#define XY_SKEW_FACTOR %.5f\n", xy_skew_factor);

    
	xz_skew_factor=tan(PI/2 - PI/180.0 * m_phi_xz);
	yz_skew_factor=tan(PI/2 - PI/180.0 * m_phi_yz);

    prtMsg(STATUS_INFO, "\nXZ/YZSKEW:\n");
	printf("phi_skew_xz=%.5f, phi_skew_yz=%.5f, xz_skew_factor=%.6f, yz_skew_factor=%.6f\n"
				,m_phi_xz, m_phi_yz, xz_skew_factor, yz_skew_factor);
    prtMsg(STATUS_INFO, "For correction of marlin firmware add the following to Configuration.h:\n");
	printf("#define XZ_SKEW_FACTOR %.5f\n", xz_skew_factor);
	printf("#define YZ_SKEW_FACTOR %.5f\n", yz_skew_factor);
}


void Probe::calc_BedLevel(void)
{
    int dirId;
    int measId;
   	// config->get_probelen_xy_mm();
	double blkx    = config->get_plate_sizex_mm() - 2*config->get_free_xy_mm();
	// config->get_probelen_xy_mm();
	double blky    = config->get_plate_sizey_mm() - 2*config->get_free_xy_mm();


// dirId is the direction with respect to the table: 0: SW, 1: NW, 2: NE, 3: SE
	for(dirId=1;dirId<4;dirId++)
        probeBedLevel_res[dirId]=0.0;

// measId is the number of the 4 measurements
    for(measId=0;measId<4;measId++)
    {
        for(dirId=0;dirId<4;dirId++)// meas 0..3 are the 4 positions on TOP of the block
            probeBedLevel_res[dirId] += m_posarr[measId][dirId].z/4.0;
    }
	for(dirId=1;dirId<4;dirId++)
	{
		probeBedLevel_res[dirId] -= probeBedLevel_res[0];
	}
	probeBedLevel_res[0]=0.0;

    prtMsg(STATUS_INFO, "\nSet Bedleveling in marlin:\n");
	printf("M421 X%.3f Y%.3f Z%.5f\n",0.0 ,0.0, probeBedLevel_res[0]);
	printf("M421 X%.3f Y%.3f Z%.5f\n",0.0 ,blky,probeBedLevel_res[1]);
	printf("M421 X%.3f Y%.3f Z%.5f\n",blkx,blky,probeBedLevel_res[2]);
	printf("M421 X%.3f Y%.3f Z%.5f\n",blkx,0.0, probeBedLevel_res[3]);
	printf("M420 S1\n");
}


// calculate the rotation of the table with respect to the moving axes
// Assumptions:
//   side-flats of the block are parallel to the ground plane of the block
void Probe::calc_rotAlign(void)
{
    double l_rotx;
    double l_roty;
    double l_rotz;
    int measId;
    
    m_phi_rotx=0.0;
    m_phi_roty=0.0;
    m_phi_rotz=0.0;
    
    // mean values over all 4 flats and all 4 measurements
    for(measId=0;measId<4;measId++)
    {
        // just approximation, projection in xy-plane not taken into account here
        m_phi_rotz+=angle(probe_nW[measId],Koord( 0.0, 1.0, 0.0));
        m_phi_rotz+=angle(probe_nN[measId],Koord( 1.0, 0.0, 0.0));
        m_phi_rotz+=angle(probe_nE[measId],Koord( 0.0,-1.0, 0.0));
        m_phi_rotz+=angle(probe_nS[measId],Koord(-1.0, 0.0, 0.0));
    }
    m_phi_rotz/=16;
    m_phi_rotz-=90;


    // mean values over all 4 measurements
    for(measId=0;measId<4;measId++)
    {
        // just approximation, projection in xy-plane not taken into account here
        m_phi_rotx+=angle(probe_nT[measId],Koord( 0.0, 1.0, 0.0));
        m_phi_roty+=angle(probe_nT[measId],Koord( -1.0, 0.0, 0.0));
    }
    m_phi_rotx/=4;
    m_phi_rotx-=90;
    m_phi_roty/=4;
    m_phi_roty-=90;


	l_rotx=sin(PI/180.0 * m_phi_rotx);
	l_roty=sin(PI/180.0 * m_phi_roty);
	l_rotz=sin(PI/180.0 * m_phi_rotz);

    if(debug)
    {
        prtMsg(STATUS_INFO, "Rotations of table with respect to moving axes:\n");
        printf("m_phi_rotx=%.5lf (%.5lf mm/m)\n",m_phi_rotx,l_rotx*1000.0);
        printf("m_phi_roty=%.5lf (%.5lf mm/m)\n",m_phi_roty,l_roty*1000.0);
        printf("m_phi_rotz=%.5lf (%.5lf mm/m)\n",m_phi_rotz,l_rotz*1000.0);
    }
}


void Probe::Save_posarr(void)
{
	int cnt;
	int measId;
	FILE *l_measfp;
	char fname[30];		// last words: "30" should be enough for each and everyone forever :-)

	cnt=0;
	sprintf(fname,"meas%d.cpp",cnt);
	
	while((l_measfp=fopen(fname,"r")))
	{
		fclose(l_measfp);
		cnt++;			// increment while file exist
		sprintf(fname,"meas%d.cpp",cnt);
	}
// fname holds the file to use for save operation
	printf("Save_posarr: %s\n",fname);
	l_measfp=fopen(fname,"w");
	
	fprintf(l_measfp,"#include \"Koord.h\"\n");
    fprintf(l_measfp,"Koord oldmeas_posarr[4][16]={\n");

	for(measId=0;measId<4;measId++)
	{
        fprintf(l_measfp,"{\n");
        for(int p=0;p<16;p++)
        {
            fprintf(l_measfp,"{%.5lf,%.5lf,%.5lf}"
                ,m_posarr[measId][p].x
                ,m_posarr[measId][p].y
                ,m_posarr[measId][p].z
            );
            if(p!=15)
                fprintf(l_measfp,",");
            fprintf(l_measfp,"\n");
        }
		if(3!=measId)
			fprintf(l_measfp,"},\n");
		else
			fprintf(l_measfp,"}\n");
	}
    fprintf(l_measfp,"};\n");
	fclose(l_measfp);
}
	



/* perform 4 times measurement of all edges/flats of the block
  calculate block geometry data
  calculate calibration data
  store the result */
void Probe::MeasureAndCalc(void)
{
    int measId;
    int posId;

    prtMsg(STATUS_INFO, "-----------------------------------------------------------------------\n");
    prtMsg(STATUS_INFO, "HWconfiguration:\n");
	printf("dist_xy=%g\n",config->get_dist_xy_mm());
	printf("dist_z=%g\n",config->get_dist_z_mm());
	printf("diamProbe=%g\n",config->get_diamProbe_mm());
	printf("free_xy=%g\n",config->get_free_xy_mm());
	printf("probe_depth=%g\n",config->get_probe_depth_mm());
	printf("fspeed_probe_mmmin=%d\n",config->get_fspeed_probe_mmmin());
	printf("fspeed_move_mmmin=%d\n",config->get_fspeed_move_mmmin());
	printf("plate_sizex_mm=%g\n",config->get_plate_sizex_mm());
	printf("plate_sizey_mm=%g\n",config->get_plate_sizey_mm());
	printf("probelen_z_mm=%g\n",config->get_probelen_z_mm());
    prtMsg(STATUS_INFO, "-----------------------------------------------------------------------\n");



	


// measure/use old measured data
    for(measId=0;measId<4;measId++)
    {
		printf("%d/4 measurement, Corner %d to SW [ENTER]",measId+1,measId+1);
		getchar();

        if(!g_simul)
        {
            Meas_Block(measId);
        }
        else
        {   // data was given by meas.cpp for testing the mathematics are unchanged
            for(posId=0;posId<16;posId++)
                m_posarr[measId][posId]=oldmeas_posarr[measId][posId];
        }
    }

	if(!g_simul)
		Save_posarr();

// calculate some data, which is used for further calculations    
    calc_NormVectors();
    calc_Angles();

// calculate geometry of the block, which is probed
    calc_BlockGeometry();

// calculate calibration data    
    calc_Skews();
    calc_BedLevel();
    calc_rotAlign();
}

