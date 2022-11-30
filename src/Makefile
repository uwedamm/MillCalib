
ifdef COMSPEC
  MV ?= move
  RM ?= del
else
  MV ?= mv -f
  RM ?= rm -f
endif



# -static -mwindows
MillCalib: main.o probe.o ausw_math.o config.o rs232.o rs232_cl.o textoutput.o timediff_ms.o Marlin.o meas.o
	gcc -o MillCalib -static main.o probe.o ausw_math.o config.o rs232.o rs232_cl.o textoutput.o timediff_ms.o Marlin.o meas.o -lm -lstdc++	
	$(MV) MillCalib* ../bin/

main.o: main.cpp Marlin.h Mill.h probe.h
	gcc -Wall -c main.cpp

probe.o: probe.cpp probe.h ausw_math.h koord.h textoutput.h
	gcc -Wall -c probe.cpp

ausw_math.o: ausw_math.cpp ausw_math.h
	gcc -Wall -c ausw_math.cpp

config.o: config.cpp config.h textoutput.h
	gcc -Wall -c config.cpp

Marlin.o: Marlin.cpp Marlin.h Mill.h koord.h rs232_cl.h config.h timediff_ms.h textoutput.h 
	gcc -Wall -c Marlin.cpp

rs232.o: rs232.c rs232.h
	gcc -Wall -c rs232.c

rs232_cl.o: rs232_cl.cpp Std_Types.h rs232_cl.h timediff_ms.h
	gcc -Wall -c rs232_cl.cpp

textoutput.o: textoutput.cpp textoutput.h
	gcc -Wall -c textoutput.cpp

timediff_ms.o: timediff_ms.cpp timediff_ms.h
	gcc -Wall -c timediff_ms.cpp

meas.o: meas.cpp meas.h koord.h
	gcc -Wall -c meas.cpp



clean:
	$(RM) *.o
	$(RM) ./bin/MillCalib*
	