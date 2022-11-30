# Usage
youtube video will follow in few weeks (today is 30.11.2022)<BR>
Results, which are measured on my mill can be found in bin/measresults.CSV


# Installation using cygwin:
install cygwin: https://www.cygwin.com/install.html

Add packages:
- gcc-g++
- make

execute cygwin<BR>
change to source directory.<BR>
Note: '' are needed to be able to use windows paths, e.g.:<BR>
```
$ cd 'f:\messknecht\Fraese\src\Antasten\MillCalib\src'
$ make
$ ./MillCalib.exe -d1
```
Note: "-d1" enables all debug print messages, may be a good starting point, not needed later

test the connection to firmware:<BR> 
t [ENTER]


# Installation using mingw (untested):
download mingw:
http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe/download
installed (online)

Alternativ offline:
mingw-w64.zip (zipped by myself)
copy media\uwe\win10\Program Files (x86)\mingw-w64\i686-8.1.0-posix-dwarf-rt_v6-rev0
	inside to d:\install   (take care about file-length)
	
Add bin folder into PATH
cmd.exe
	-> compiles/links ("make clean" uses rm, which is not supported using windows)
	del *.o
	del MillCalib.exe
make
-> MillCalib.exe

# Marlin configuration:
I do have the following setup:
- Ramps 1.4 compatible board
- probe connected to XMIN input (D3). This is a bit special, I had to exchange XMIN<->XMAX pins in pins_RAMPS.h
```
#ifndef X_STOP_PIN
  #ifndef X_MIN_PIN
    #define X_MIN_PIN                          2
  #endif
  #ifndef X_MAX_PIN
    #define X_MAX_PIN                          3
  #endif
#endif
```
## Configuration.h
```
#define BAUDRATE 115200
//#define E0_DRIVER_TYPE A4988

//dammuw1 no extruder for a mill
#define EXTRUDERS 0
//#define PREVENT_COLD_EXTRUSION
//#define EXTRUDE_MINTEMP 170
//#define PREVENT_LENGTHY_EXTRUDE
//#define EXTRUDE_MAXLENGTH 200

// not needed, but that is how my configuration looks like:
#define USE_XMAX_PLUG
#define USE_YMAX_PLUG
#define USE_ZMAX_PLUG

// dammuw1: I am using pullup-resistors on the board and the switches switch to GND
// #define ENDSTOPPULLUPS

// dammuw1: This is probably wrong... but works fine ...
  #define ENDSTOPPULLUP_XMIN

// dammuw1: NC means Normally Closed
// dammuw1: FALSE means: Portpin==ground -> ENDSTOP is NOT triggered
// dammuw1: FALSE means: Portpin==VCC (via pullup) -> ENDSTOP is triggered
// in other words:
//    false: endstop pressed -> open -> VCC on portpin
//    FALSE: endstop not pressed -> GND on portpin
// dammuw1: If Endstop input is used for probing, NO is used instead of NC => vice versa
#define X_MIN_ENDSTOP_INVERTING true // Set to true to invert the logic of the endstop.
#define Y_MIN_ENDSTOP_INVERTING true // Set to true to invert the logic of the endstop.
#define Z_MIN_ENDSTOP_INVERTING true // Set to true to invert the logic of the endstop.

#define X_MAX_ENDSTOP_INVERTING true // Set to true to invert the logic of the endstop.
#define Y_MAX_ENDSTOP_INVERTING true // Set to true to invert the logic of the endstop.
#define Z_MAX_ENDSTOP_INVERTING true // Set to true to invert the logic of the endstop.

// dammuw1: this setting succeeded for my probe (?)
#define Z_MIN_PROBE_ENDSTOP_INVERTING true // Set to true to invert the logic of the probe.

// dammuw1
#define ENDSTOP_INTERRUPTS_FEATURE

//dammuw1 adapted to 5mm/1600 steps and removed extruder entry
//#define DEFAULT_AXIS_STEPS_PER_UNIT   { 320, 320, 320, 500 }
#define DEFAULT_AXIS_STEPS_PER_UNIT   { 320, 320, 320 }

// dammuw1: removed extruder
#define DEFAULT_MAX_FEEDRATE          { 80, 80, 80 }
//#define DEFAULT_MAX_FEEDRATE          { 80, 80, 80, 25 }

 //dammuw1 adapted to my mill and removed extruder entry
#define DEFAULT_MAX_ACCELERATION      { 800, 800, 800}
//#define DEFAULT_MAX_ACCELERATION      { 800, 800, 800, 800 }

 //dammuw1 adapt to my mill
#define DEFAULT_ACCELERATION          500    // X, Y, Z and E acceleration for printing moves
#define DEFAULT_RETRACT_ACCELERATION  1000    // E acceleration for retracts
#define DEFAULT_TRAVEL_ACCELERATION   1000    // X, Y, Z acceleration for travel (non printing) moves

//dammuw1 ... check
//#define Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN

// dammuw1: not really understood, why my probing at XMIN Endstop input works fine
#define USE_PROBE_FOR_Z_HOMING

// dammuw1: configured to xmin pin for probing via G38.2/3/4/5
// This can only be done, if the pin is not equal to 
//     #define X_MIN_PIN                          2
// which I changed in pins_RAMPS.h
#define Z_MIN_PROBE_PIN 3 // dammuw1: Pin 3 is D3 is XMIN of the RAMPS setup (used for my mill)

// dammuw1: enabled to avoid sanitycheck: #error "G38_PROBE_TARGET requires a bed probe."
#define FIX_MOUNTED_PROBE

// X and Y axis travel speed (mm/min) between probes
// dammuw1: dont remember, why I changed this
#define XY_PROBE_FEEDRATE (1*60)

// dammuw1: dont remember, why I changed this
#define Z_PROBE_FEEDRATE_FAST (5*60)

// dammuw1: dont know, if this is really needed for my probe configuration
#define PROBE_ACTIVATION_SWITCH

// dammuw1 check, which pin is xminpin and configure here???
#define PROBE_ACTIVATION_SWITCH_PIN 3 // Override default pin
  
// dammuw1: no extruder for a mill
//#define DISABLE_E false             // Disable the extruder when not stepping
//#define DISABLE_INACTIVE_EXTRUDER   // Keep only the active extruder enabled
  
// dammuw1: adapted to my machine
#define INVERT_Y_DIR false

// dammuw1: if no endstop available, -1 and 1 are not allowed (sanity-check)
// dammuw1: furthermore in Conditionals_post.h the home-position is set to max, if <AXIS>_HOME_DIR is >= 0
// -1 seems to work for my g38 usage
#define X_HOME_DIR -1
#define Y_HOME_DIR -1
#define Z_HOME_DIR -1

// dammuw1: no need for an end (I dont use homing
#define X_BED_SIZE 20000
#define Y_BED_SIZE 20000

// Travel limits (mm) after homing, corresponding to endstop positions.
#define X_MIN_POS -10000
#define Y_MIN_POS -10000
#define Z_MIN_POS -10000
#define X_MAX_POS 10000
#define Y_MAX_POS 10000
#define Z_MAX_POS 10000

// dammuw1: SW Endstops disabled
//#define MIN_SOFTWARE_ENDSTOPS

// dammuw1: SW Endstops disabled
//#define MAX_SOFTWARE_ENDSTOPS

// dammuw1:
#define SOFT_ENDSTOPS_MENU_ITEM  // Enable/Disable software endstops from the LCD

// dammuw1 enabled bedleveling (Maybe I will try this feature later on)
#define MESH_BED_LEVELING

// dammuw1 disabled fading for bedleveling
//#define ENABLE_LEVELING_FADE_HEIGHT

// dammuw1 2x2 grid for bedleveling
#define GRID_MAX_POINTS_X 2

// dammuw1: for later experiments
#define MESH_INSET 0          // Set Mesh bounds as an inset region of the bed
#define GRID_MAX_POINTS_X 2    // Don't use more than 7 points per axis, implementation limited.

// dammuw1: my startposition for mill is 0,0,0
#define MANUAL_X_HOME_POS 0
#define MANUAL_Y_HOME_POS 0
#define MANUAL_Z_HOME_POS 0

// dammuw1: get rid of compiler-error
#define Z_SAFE_HOMING

// dammuw1: all axis same speed on my mill
#define HOMING_FEEDRATE_MM_M { (40*60), (40*60), (40*60) }

// dammuw1: used for testing results of probing
#define SKEW_CORRECTION
//  #define XY_DIAG_AC 282.8427124746
//  #define XY_DIAG_BD 282.8427124746
//  #define XY_SIDE_AD 200
// Or, set the default skew factors directly here
// to override the above measurements:
// dammuw1: for the probing dont use the SKEW correction, but later change it with G-codes
#define XY_SKEW_FACTOR 0.0
#define XZ_SKEW_FACTOR 0.0
#define YZ_SKEW_FACTOR 0.0
#define SKEW_CORRECTION_GCODE

// dammuw1: these should be set for my probing!!! (as per default)
#define HOST_KEEPALIVE_FEATURE        // Disable this if your host doesn't like keepalive messages
#define DEFAULT_KEEPALIVE_INTERVAL 2  // Number of seconds between "busy" messages. Set with M113.
#define BUSY_WHILE_HEATING            // Some hosts require "busy" messages even during heating

//dammuw1 OLED display connected to RAMPs MEGA2560 board
#define MKS_12864OLED_SSD1306  // Uses the SSD1306 controller
```

## Configuration_Adv.h
```
// dammuw1: removed entry for extruder
#define AXIS_RELATIVE_MODES { false, false, false}

// dammuw1: do not inactivate steppers to not loose microsteps in positioning
#define DEFAULT_STEPPER_DEACTIVE_TIME 0

// dammuw1: enable backlash compensation
#define BACKLASH_COMPENSATION

// dammuw1: kept defaults for the moment
#define BACKLASH_DISTANCE_MM { 0, 0, 0 } // (linear=mm, rotational=°) One value for each linear axis
#define BACKLASH_CORRECTION    0.0       // 0.0 = no correction; 1.0 = full correction

// dammuw1: enable usage of BACKLASH correction via GCODE
#define BACKLASH_GCODE

//dammuw1: figure out, how to disable probing at all (uncommented due to compile error)
//#define MEASURE_BACKLASH_WHEN_PROBING

// dammuw1: sanitycheck... without extruder
#define MANUAL_FEEDRATE { 40*60, 40*60, 40*60} // (mm/min) Feedrates for manual moves along X, Y, Z, E from panel

// dammuw1 adapt the mesh positions
#define MESH_MIN_X 0
#define MESH_MIN_Y 0
#define MESH_MAX_X 100
#define MESH_MAX_Y 100

// dammuw1: from time to time I am using the ARC feature
#define MIN_ARC_SEGMENT_MM      1.0 // (mm) Minimum length of each arc segment
#define MAX_ARC_SEGMENT_MM      2.0 // (mm) Maximum length of each arc segment
#define MIN_CIRCLE_SEGMENTS    12   // Minimum number of segments in a complete circle
//#define ARC_SEGMENTS_PER_SEC 50   // Use the feedrate to choose the segment length
#define N_ARC_CORRECTION       25   // Number of interpolated segments between corrections
#define ARC_P_CIRCLES             // Enable the 'P' parameter to specify complete circles
//#define SF_ARC_FIX                // Enable only if using SkeinForge with "Arc Point" fillet procedure

// dammuw1 enabled probing
#define G38_PROBE_TARGET

// dammuw1: enabled away probing for backlash measurements
#define G38_PROBE_AWAY        // Include G38.4 and G38.5 to probe away from target
#define G38_MINIMUM_MOVE 0.003 // (mm) Minimum distance that will produce a move.  

// dammuw1 works fine for me
#define MIN_STEPS_PER_SEGMENT 1

// dammuw1: works fine with my stepper drivers
#define MINIMUM_STEPPER_POST_DIR_DELAY 10000
#define MINIMUM_STEPPER_PRE_DIR_DELAY 10000

// dammuw1: works fine with my stepper drivers
#define MINIMUM_STEPPER_PULSE 30

// dammuw1 increase the precision for "M114 R" to improve accuracy for probing
#define SERIAL_FLOAT_PRECISION 4

// dammuw1: needed for using M114 for probing
#define M114_DETAIL         // Use 'M114` for details to check planner calculations
// dammuw1: needed for using M114 for probing
#define M114_REALTIME       // Real current position based on forward kinematics

//dammuw1 check, if needed for endstops to be active while using the probing
//#define ENDSTOPS_ALWAYS_ON_DEFAULT
```

