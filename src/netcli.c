#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include "siren.h"

void ACTcheck(char *dtmf, int *Cancel_Var){
	int orig;
	if(strcmp(dtmf, CODE_SIREN_OFF) ==  0 ){
		printf("\n\n**Setting Sirens to OFF**\n");
		*Cancel_Var = 0;
	}else if(strcmp(dtmf, CODE_SIREN_GROWL) ==  0 ){
		printf("\n\n**Setting Sirens to GROWL**\n");
		*Cancel_Var = 0;
		*Cancel_Var = 1;
		SoundSiren(1, Cancel_Var);
		digitalWrite (ACTLED_PIN, HIGH);
	}else if(strcmp(dtmf,CODE_SIREN_ALERT) ==  0 ){
		printf("\n\n**Setting Sirens to ALERT**\n");
		*Cancel_Var = 0;
		*Cancel_Var = 1;
		digitalWrite (ACTLED_PIN, HIGH);
		SoundSiren(2, Cancel_Var);
	}else if(strcmp(dtmf, CODE_SIREN_ATTACK) ==  0 ){
		printf("\n\n**Setting Sirens to ATTACK**\n");
		if(*Cancel_Var > 1){
			orig = *Cancel_Var;
			*Cancel_Var = 0;
			*Cancel_Var = orig;
		}else{
			*Cancel_Var = 0;
			*Cancel_Var = 1;
		}
		digitalWrite (ACTLED_PIN, HIGH);
		SoundSiren(3, Cancel_Var);
	}
}
