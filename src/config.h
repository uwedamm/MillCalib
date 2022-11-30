#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stddef.h>
#include <stdio.h>

#define CFGNAME ".probeconfig"


class Config
{
	
private:

	char *rs232devname;
	int 	marlinreset;
	
	int fspeed_probe_mmmin;		// default 60	speed at probing
	int fspeed_move_mmmin;		// default 60	speed for free moves

	double maxztest_mm;			// default 10	max movement for z probing
	
	double diamProbe_mm;		// xy diameter of the probe (for homing xy after SW/SE/S/W testing)

	
	double probe_depth_mm;		// default 3	go probe_depth_mm below Tastplatte to test from left and from bottom

	double dist_xy_mm;			// default 2	distance to material for the free moves
	double dist_z_mm;			// default 2

	double free_xy_mm; 			// default 10	move before diving below Tastplatte (to check from the side in next move)

	double probelen_xy_mm;		// default 40	length of the probe move along the plate in xy direction
	// for Probeall,etc. this should be plate_sizex/y_mm - 2*free_xy_mm
	double probelen_z_mm;		// default 40	length of the probe move along the plate in z direction

// used for ProbeComplete
	double plate_sizex_mm;		// default 100  size of the probe-plate/block for "ProbeComplete"
	double plate_sizey_mm;		// default 100
	
	void readLine(char *buf, size_t n, FILE*fp);

public:
	Config();
	Config(const Config &cfg);
	
	~Config(void);

	Config operator= (const Config &cfg);

	int Load(void);
	void Save(void);
	
	
	
	
	
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// get/set interfaces for all configurations
	////////////////////////////////////////////////////////////////////////////////////////////
	
	char *get_rs232devname(void) const {return rs232devname;}
	void set_rs232devname(const char *p_rs232devname){sprintf(rs232devname,"%s", p_rs232devname);}

	int get_marlinreset(void) const {return marlinreset;}
	void set_marlinreset(int p_marlinreset){marlinreset=p_marlinreset;}

	int get_fspeed_probe_mmmin(void) const {return fspeed_probe_mmmin;}
	void set_fspeed_probe_mmmin(int p_fspeed_probe_mmmin){fspeed_probe_mmmin=p_fspeed_probe_mmmin;}

	int get_fspeed_move_mmmin(void) const {return fspeed_move_mmmin;}
	void set_fspeed_move_mmmin(int p_fspeed_move_mmmin){fspeed_move_mmmin=p_fspeed_move_mmmin;}

	double get_maxztest_mm(void) const {return maxztest_mm;}
	void set_maxztest_mm(double p_maxztest_mm){maxztest_mm=p_maxztest_mm;}

	double get_diamProbe_mm(void) const {return diamProbe_mm;}
	void set_diamProbe_mm(double p_diamProbe_mm){diamProbe_mm=p_diamProbe_mm;}

	double get_probe_depth_mm(void) const {return probe_depth_mm;}
	void set_probe_depth_mm(double p_probe_depth_mm){probe_depth_mm=p_probe_depth_mm;}

	double get_dist_xy_mm(void) const {return dist_xy_mm;}
	void set_dist_xy_mm(double p_dist_xy_mm){dist_xy_mm=p_dist_xy_mm;}

	double get_dist_z_mm(void) const {return dist_z_mm;}
	void set_dist_z_mm(double p_dist_z_mm){dist_z_mm=p_dist_z_mm;}

	double get_free_xy_mm(void) const {return free_xy_mm;}
	void set_free_xy_mm(double p_free_xy_mm){free_xy_mm=p_free_xy_mm;}

	double get_probelen_xy_mm(void) const {return probelen_xy_mm;}
	void set_probelen_xy_mm(double p_probelen_xy_mm){probelen_xy_mm=p_probelen_xy_mm;}

	double get_probelen_z_mm(void) const {return probelen_z_mm;}
	void set_probelen_z_mm(double p_probelen_z_mm){probelen_z_mm=p_probelen_z_mm;}


	double get_plate_sizex_mm(void) const {return plate_sizex_mm;}
	void set_plate_sizex_mm(double p_plate_sizex_mm){plate_sizex_mm = p_plate_sizex_mm;}
	double get_plate_sizey_mm(void) const {return plate_sizey_mm;}
	void set_plate_sizey_mm(double p_plate_sizey_mm){plate_sizey_mm = p_plate_sizey_mm;}
};

#endif
