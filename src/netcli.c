#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include "siren.h"

void ACTcheck(char *dtmf, int *Cancel_Var){
	int orig;

	if(strcmp(dtmf, cCODE_SIREN_OFF) ==  0 ){
		printf("**Setting Sirens to OFF**\n");
		*Cancel_Var = 0;
		delay(500);
	}else if(strcmp(dtmf, cCODE_SIREN_GROWL) ==  0 ){
		printf("**Setting Sirens to GROWL**\n");
		*Cancel_Var = 0;
		delay(500);
		*Cancel_Var = 1;
		SoundSiren(1, Cancel_Var);
		digitalWrite (cACTLED_PIN, HIGH);
	}else if(strcmp(dtmf,cCODE_SIREN_ALERT) ==  0 ){
		printf("**Setting Sirens to ALERT**\n");
		*Cancel_Var = 0;
		delay(500);
		*Cancel_Var = 1;
		digitalWrite (cACTLED_PIN, HIGH);
		SoundSiren(2, Cancel_Var);
	}else if(strcmp(dtmf, cCODE_SIREN_ATTACK) ==  0 ){
		printf("**Setting Sirens to ATTACK**\n");
		*Cancel_Var = 0;
		delay(500);
		if(*Cancel_Var > 1){
			*Cancel_Var = 0;
			orig = *Cancel_Var;
			*Cancel_Var = orig;
		}else{
			*Cancel_Var = 0;
			*Cancel_Var = 1;
		}
		digitalWrite (cACTLED_PIN, HIGH);
		SoundSiren(3, Cancel_Var);
	}
}

