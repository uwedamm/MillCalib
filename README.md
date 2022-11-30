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

