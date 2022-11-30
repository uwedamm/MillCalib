#ifndef _PROBE_H_
#define _PROBE_H_

// include RS232
#include "config.h"
#include "Mill.h"


// block, view from TOP
// 2   3
//
// 1   4

// main chain of the orthogonality measurement:
// loop over the 4 TOP edges of the block (rotating the block CCW after each measurement)
// Place a block with known size (x*y*z) on the table of the mill
// move the probe a bit above the block to the SW, a bit to the inside of the block
// start measurement:
// 1a. Probe 4 corners of the block, 5 testpoints each, the 1st 2 only to optimize the position, where the other 3 are measured -> improve accuracy
// 1b. Probe 4 sides of the block at lower z-position
// 2. calculate the measured angles in xy, z direction
// 3a. calculate the blockgeometry (angles of the block)
// 3b. calculate the skew of the mill
// 3c. calculate the misalignment of the table with respect to the moving axis



class Probe
{
private:
	Config *config;
	Mill *mill;
	
    /* new SW architecture */
    Koord m_posarr[4][16];
    
    
// rotating identifier for each measurement id[0] is the number of the corner to SW
	int id[4];


// normvectors from the surfaces of the block
// calculated from probe measurements (posarr[0..15]]), see Probe_CompleteAccurate()
// index is the number of the measurement, i.e. the corner, which pointed to SW
	Koord probe_nW[4];
	Koord probe_nN[4];
	Koord probe_nE[4];
	Koord probe_nS[4];
	Koord probe_nT[4];


// bedleveling, index are the positions in the sense of 0:SW,1:NW,2:NE,3:SE
// measured as meanvalues of the 4 measurements to eliminate the block-geometry
	double probeBedLevel_res[4];

//////////////////////////////////////////////////
/* measure angles in xy and z direction (determined from nW, nS, ... */
	double probeSW_res[4];	// id[n] is used to access this array. It defines the corner of the block, which points to SW,NW, ...
	double probeNW_res[4];	// der index auf diese arrays benennt die Ecke des Blocks
	double probeNE_res[4];  // 0: 1, 1: 2, 2: 3, 3: 4
	double probeSE_res[4];
	
	double probeWT_res[4]; // id[0] to access (if id[0]==0, d.h. Ecke 1 bei SW -> dann ist dies die Kante von 1->2
	double probeNT_res[4]; // der index auf diese Arrays benennt die Kante des Blocks
	double probeET_res[4]; // 0: 1->2, 1: 2->3, 2: 3->4, 3: 4->1
	double probeST_res[4];
//////////////////////////////////////////////////
	

	double probeS_res;		// used for the 45 deg measurements to measure the ratio of the spindle feeds in x/y direction
	double probeW_res;


/////////////////////////////////////////////////////////////////////////////
// Blockgeometry:
// calculated from the measurement (and used for later calculations)
	double m_phi_block_xy[4];	// angle for each corner in xy plane
	double m_phi_block_z[4];	// angle for each corner in xy plane
    
    double meanphi_block_all_xy;    // should be near to 90.0000 depending on the accuracy of the measurement
/////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// final results of the probe mechanism
    // angles with respect to skew (angles between moving axis of mill
	double m_phi_xy;			// angle between movement along x and y axis
	double m_phi_xz,m_phi_yz;	// meanvalue of the skew angles (between movement in Z and TABLE
    // angles with respect to table rotation (angles between moving axis of mill and table)
    double m_phi_rotx;
    double m_phi_roty;
    double m_phi_rotz;
////////////////////////////////////////////////////////////

	void NewResults(void);  /* print all measured angles on the screen */

/* Used to print geometric data, after loading it */
    void printGeometry(void);

	void meas2geo(double phi1, double phi2, double &phi_block, double &phi_xy);

	void calc_ZSkewAndBlockgeometry(double *xzskewptr=NULL,double *yzskewptr=NULL);
	void calc_XYSkewAndBlockgeometry(void);
	void calc_ZSkewFromBlockgeometry(double *xzskewptr=NULL,double *yzskewptr=NULL);
	double calc_XYSkewFromBlockgeometry(void);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// new SW architecture
/*  fill m_posarr[id][16] */
    void Meas_Block(int id);
    
	// store posarr in cpp like structure -> can be used later for recalculation
	void Save_posarr(void);
	
/*  calculate normVectors: probe_nW[measId], ...
        from m_posarr[measId]*/
    void calc_NormVectors(void);

/*  calculate measured angles: probeSW_res[measId], etc (all 4 measurements)
        from m_posarr[measId]
    calculate measured angles: probeWT_res[measId], etc (all 4 measurements)
        from m_posarr[measId]*/
    void calc_Angles(void);
    
/*  calculate m_phi_block_xy[edgeId]    xy-Winkel des Blocks
        from probeSW_res[measId]...
            via ???
    calculate m_phi_block_z[cornerId]
            edgeId is the starting edge CW for the cornerId
            Winkel der Kanten des Blocks (0: 1-2, 1: 2-3, ...)
        from probeWT_res
            via phi_blockz*/
    void calc_BlockGeometry(void);
    void calc_BlockGeometryxy(void);
    void calc_BlockGeometryz(void);

/*  calculate x,y and z skews:
    calculate m_phi_xy <-> xy_skew_factor: Winkel zwischen x und y Bewegung des Fräsers
        from probeSW_res...
    calculate m_phi_xz and m_phi_yz <-> x/yz_skew_factor
        from probeWT_res[measId], ...*/
    void calc_Skews(void);

/* calculate probeBedLevel_res[posId] (sum of the 4 measurements)
    from m_posarr[measId]*/
    void calc_BedLevel(void);

/*  calculate m_phi_rotx/y/z: Verdrehung des Tisches um X/Y/Z
            Expectation: block is aligned to x-corner of table
        from probe_nW[measId] ...*/
    void calc_rotAlign(void);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////



public:
	Probe(Config *p_config, Mill *P_mill);
	~Probe(void);


	void Test(void);

/////////////////////////////////////////////////////////////////////////////////////////////
/* new SW architecture */
/*
    perform measurement of 4 block rotations
        calculate all intermediated results
        calculate blockgeometry
        calculate calibration results
        (execute all the functions below
*/
    void MeasureAndCalc(void);

/////////////////////////////////////////////////////////////////////////////////////////////


	void Send2Mill(unsigned char *str){
		mill->send(str);
		mill->rcvResult();
	}

	int getId(int num){return id[num];}

	double get_probeSW_res(int id){return probeSW_res[id];}
	double get_probeNW_res(int id){return probeNW_res[id];}
	double get_probeNE_res(int id){return probeNE_res[id];}
	double get_probeSE_res(int id){return probeSE_res[id];}

	double get_probeWT_res(int id){return probeWT_res[id];}
	double get_probeNT_res(int id){return probeNT_res[id];}
	double get_probeET_res(int id){return probeET_res[id];}
	double get_probeST_res(int id){return probeST_res[id];}

	double get_probeS_res(void){return probeS_res;}
	double get_probeW_res(void){return probeW_res;}


// save and load measured blockgeometry<n>.cfg
	void SaveGeometry(void);
	void LoadGeometry(void);

// attach measurement results to CSV file
	void SaveMeasResults(void);
    

    // perform 1 (probeallflags==0 -> id[0] edge to SW) or 4 (probeallflags==1) measurements
	void Probe_CompleteAccurate(int probeallflag=1);

// measure backlash in the given direction and report result
	void Backlash(Koord dir);

};
#endif
