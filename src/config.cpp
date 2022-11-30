#include<string.h>

#include "config.h"
#include "textoutput.h"

Config::Config(void)
{
	prtMsg(STATUS_DEBUG, "Constructor Config");

	rs232devname=new char [256];

    if(!(Load()))
	{
		prtMsg(STATUS_DEBUG, "Config::Config load failed");
		set_rs232devname("/dev/ttyUSB0");
		set_marlinreset(1);
		set_fspeed_probe_mmmin(60);
		set_fspeed_move_mmmin(60);
		set_maxztest_mm(10.0);
		set_diamProbe_mm(3.0);
		set_probe_depth_mm(3.0);
		set_dist_xy_mm(2.0);
		set_dist_z_mm(2.0);
		set_free_xy_mm(10.0);
		set_probelen_xy_mm( 90.0);
		set_probelen_z_mm( 40.0);

		set_plate_sizex_mm( 100.0);
		set_plate_sizey_mm( 100.0);
	}
	prtMsg(STATUS_DEBUG, "Constructor config done");
}

Config::Config(const Config &cfg)
{
	prtMsg(STATUS_DEBUG, "Config Konstruktor gets cfg");
	set_rs232devname(cfg.get_rs232devname());
	set_marlinreset(cfg.get_marlinreset());
	set_fspeed_probe_mmmin(cfg.get_fspeed_probe_mmmin());
	set_fspeed_move_mmmin(cfg.get_fspeed_move_mmmin());
	set_maxztest_mm(cfg.get_maxztest_mm());
	set_diamProbe_mm(cfg.get_diamProbe_mm());
	set_probe_depth_mm(cfg.get_probe_depth_mm());
	set_dist_xy_mm(cfg.get_dist_xy_mm());
	set_dist_z_mm(cfg.get_dist_z_mm());
	set_free_xy_mm(cfg.get_free_xy_mm());
	set_probelen_xy_mm(cfg.get_probelen_xy_mm());
	set_probelen_z_mm(cfg.get_probelen_z_mm());
	set_plate_sizex_mm(cfg.get_plate_sizex_mm());
	set_plate_sizey_mm(cfg.get_plate_sizey_mm());

	prtMsg(STATUS_DEBUG, "Config Konstruktor got cfg");
}

Config::~Config()
{
	prtMsg(STATUS_DEBUG, "Destructor config");
	delete [] rs232devname;
}

Config Config::operator = (const Config &cfg) {
	prtMsg(STATUS_DEBUG, "config operator= gets cfg");
	set_rs232devname(cfg.get_rs232devname());
	set_marlinreset(cfg.get_marlinreset());
	set_fspeed_probe_mmmin(cfg.get_fspeed_probe_mmmin());
	set_fspeed_move_mmmin(cfg.get_fspeed_move_mmmin());
	set_maxztest_mm(cfg.get_maxztest_mm());
	set_diamProbe_mm(cfg.get_diamProbe_mm());
	set_probe_depth_mm(cfg.get_probe_depth_mm());
	set_dist_xy_mm(cfg.get_dist_xy_mm());
	set_dist_z_mm(cfg.get_dist_z_mm());
	set_free_xy_mm(cfg.get_free_xy_mm());
	set_probelen_xy_mm(cfg.get_probelen_xy_mm());
	set_probelen_z_mm(cfg.get_probelen_z_mm());
	set_plate_sizex_mm(cfg.get_plate_sizex_mm());
	set_plate_sizey_mm(cfg.get_plate_sizey_mm());
	prtMsg(STATUS_DEBUG, "operator= done");
	return cfg;
}



void Config::Save(void)
{
	FILE *l_fp;
	
	if(!(l_fp=fopen(CFGNAME,"w")))
	{
		return;
	}
	fprintf(l_fp,"rs232devname = %s\n",rs232devname);
	fprintf(l_fp,"marlinreset = %s\n",marlinreset?"1":"0");
	fprintf(l_fp,"fspeed_probe_mmmin = %d\n",fspeed_probe_mmmin);
	fprintf(l_fp,"fspeed_move_mmmin = %d\n",fspeed_move_mmmin);
	fprintf(l_fp,"maxztest_mm = %.3g\n",maxztest_mm);
	fprintf(l_fp,"diamProbe_mm = %.3g\n",diamProbe_mm);
	fprintf(l_fp,"probe_depth_mm = %.3g\n",probe_depth_mm);
	fprintf(l_fp,"dist_xy_mm = %.3g\n",dist_xy_mm);
	fprintf(l_fp,"dist_z_mm = %.3g\n",dist_z_mm);
	fprintf(l_fp,"free_xy_mm = %.3g\n",free_xy_mm);
	fprintf(l_fp,"probelen_xy_mm = %.3g\n",probelen_xy_mm);
	fprintf(l_fp,"probelen_z_mm = %.3g\n",probelen_z_mm);
	fprintf(l_fp,"plate_sizex_mm = %.3g\n",plate_sizex_mm);
	fprintf(l_fp,"plate_sizey_mm = %.3g\n",plate_sizey_mm);
	
	fclose(l_fp);
}
	
void Config::readLine(char *buf, size_t n, FILE*fp)
{
	fgets(buf,n,fp);
	size_t len=strnlen(buf,n);

	// remove last character if equal to \n
	if(len > 0)
	{
		if(buf[len-1]=='\n')
			buf[len-1]='\0';
	}
}

int Config::Load(void)
{
	FILE *fp;
	char buf[256];
	
    if (!(fp=fopen(CFGNAME,"r")))
	{
		char txt[256];
		sprintf(txt,"cannot open %s for reading",CFGNAME);
		prtMsg(STATUS_ERROR, txt);

        return 0;
	}
	

	readLine(buf,255,fp);
	sscanf(buf,"rs232devname = %s",rs232devname);

	readLine(buf,255,fp);
	sscanf(buf,"marlinreset = %d",&marlinreset);
	
	readLine(buf,255,fp);
	sscanf(buf,"fspeed_probe_mmmin = %d",&fspeed_probe_mmmin);
	
	readLine(buf,255,fp);
	sscanf(buf,"fspeed_move_mmmin = %d",&fspeed_move_mmmin);
	
	readLine(buf,255,fp);
	sscanf(buf,"maxztest_mm = %lg",&maxztest_mm);
	
	readLine(buf,255,fp);
	sscanf(buf,"diamProbe_mm = %lg",&diamProbe_mm);
	
	readLine(buf,255,fp);
	sscanf(buf,"probe_depth_mm = %lg",&probe_depth_mm);
	
	readLine(buf,255,fp);
	sscanf(buf,"dist_xy_mm = %lg",&dist_xy_mm);
	
	readLine(buf,255,fp);
	sscanf(buf,"dist_z_mm = %lg",&dist_z_mm);
	
	readLine(buf,255,fp);
	sscanf(buf,"free_xy_mm = %lg",&free_xy_mm);
	
	readLine(buf,255,fp);
	sscanf(buf,"probelen_xy_mm = %lg",&probelen_xy_mm);
	
	readLine(buf,255,fp);
	sscanf(buf,"probelen_z_mm = %lg",&probelen_z_mm);
	
	readLine(buf,255,fp);
	sscanf(buf,"plate_sizex_mm = %lg",&plate_sizex_mm);

	readLine(buf,255,fp);
	sscanf(buf,"plate_sizey_mm = %lg",&plate_sizey_mm);

	fclose(fp);
	return 1;
}
