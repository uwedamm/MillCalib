#include <stdlib.h>
#include <stdio.h>
#include "textoutput.h"


// can be set by -d<val> commandline option
int debug=0;

// prtMsg(STATUS_DEBUG, "some information");
// prtMsg(STATUS_ERROR, "unexpected error -> quit application");

void prtMsg(msgType type, const char* txt) // C-style "const char* fmt, ..." is also valid
{
	switch (type) {
    case STATUS_INFO:
        printf("%s\n",txt);
		break;
    
	case STATUS_DEBUG:
		if(debug)
		    printf("%s\n",txt);
		break;
        
	case STATUS_ERROR:
	    printf("ERROR: %s\n",txt);
	    exit(1);
		break;
        
	case RS232SEND:
	case RS232_MSG:
	case RS232_DIVERSE:
		if(debug & 1)
		    printf("%s\n",txt);
		break;
	}
}




