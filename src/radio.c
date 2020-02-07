#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "siren.h"

int last_time = 0;

void DTMFhandel(char *dtmf, int charcount, int timestamp,int *Cancel_Var){			//HANDELS TIME PROCESSING AND DTMF CHAR[] CREATION
	if(last_time == 0){
		last_time = timestamp;								//CHECKS FOR JUST INITILIZED TIMESTAMP
	}

	if(timestamp <= last_time + 5){								//CHECKS IF TIMEOUT HAS PASSED SINCE LAST DTMF CHAR
		last_time = timestamp;								//RESETS TIMESTAMP
		if(charcount + 1 <= cCODE_LENGHT){						//COUNTS # OF CHARS SINCE FIRST START
			dtmf_string[charcount] = *dtmf;						//ADDS CHAR TO DTMF STRING
			if(charcount + 1 == cCODE_LENGHT){					//CHECK IF DTMF STRING IS PROPPER LENGHT
				DTMFcheck(dtmf_string, Cancel_Var);				//CHECK DTMF STRING FOR cACTIVATION CODES
			}
		}else{
			dtmf_count = 0;								//RESETS DTMF CHAR RECIVED COUNT
			last_time = timestamp;							//RESETS TIMESTAMP
			CleanChar(dtmf_string);							//RESETS DTMF CHARS
			dtmf_string[dtmf_count] = *dtmf;					//ADDS CHAR TO DTMF STRING
		}
	}else{
		last_time = timestamp;								//RESETS TIMESTAMP
		dtmf_count = 0;									//RESETS DTMF CHAR RECIVED COUNT
		CleanChar(dtmf_string);								//ADDS CHAR TO DTMF STRING
		dtmf_string[dtmf_count] = *dtmf;						//RESETS DTMF CHARS
	}
	dtmf_count++;										//INCREMENT DTMF CHAR COUNT
}

void DTMFcheck(char *dtmf, int *Cancel_Var){
	if(strcmp(dtmf, cCODE_SIREN_OFF) ==  0 ){						//CHECK FOR OFF CODE
		printf("Setting Sirens to OFF\n");
		*Cancel_Var = 1;
	}else if(strcmp(dtmf, cCODE_SIREN_GROWL) ==  0 ) {					//CHECK FOR GROWL CODE
		printf("Setting Sirens to GROWL\n");
		*Cancel_Var = 1;
		*Cancel_Var = 0;
		SoundSiren(1, Cancel_Var);
		digitalWrite (cACTLED_PIN, HIGH);
	}else if(strcmp(dtmf,cCODE_SIREN_ALERT) ==  0 ){						//CHECK FOR ALERT CODE
		printf("Setting Sirens to ALERT\n");
		*Cancel_Var = 1;
		*Cancel_Var = 0;
		SoundSiren(2, Cancel_Var);
		digitalWrite (cACTLED_PIN, HIGH);
	}else if(strcmp(dtmf, cCODE_SIREN_ATTACK) ==  0 ){					//CHECK FOR ATTACK CODE
		printf("Setting Sirens to ATTACK\n");
		*Cancel_Var = 1;
		*Cancel_Var = 0;
		SoundSiren(3, Cancel_Var);
		digitalWrite (cACTLED_PIN, HIGH);
	}
}

void CleanChar(char *var) {									//CHAR[] CLEARER
    int i = 0;
    while(var[i] != '\0') {
        var[i] = '\0';
        i++;
    }
}
