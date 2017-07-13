#include "siren.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <wiringPi.h>
#include <softPwm.h>

/*
HACKABLE CIVIL DEFENSE SIREN POF
COPYRIGHT MAX WATERMOLEN 2017

TESTED ON RASPBERRYPI B+ REV 1.2 (MODEL 1) 	***NOTE*** SHOULD WORK ON ALL REVISIONS ***NOTE*** 

NEEDS: 
	H/W: Adafruit DRV8871 Brushed DC Motor Driver Breakout - https://learn.adafruit.com/adafruit-drv8871-brushed-dc-motor-driver-breakout/overview 
	S/W: RTL_FM ; MULTIMON-NG ; WIRINGPI - TO OPPERATE

INSTALL WITH ONE COMMAND: 

OS RASPBERRIAN: 4.9.35+ #1014 Fri Jun 30 14:34:49 BST 2017 armv6l GNU/Linux

processor       : 0
model name      : ARMv6-compatible processor rev 7 (v6l)
BogoMIPS        : 697.95
Features        : half thumb fastmult vfp edsp java tls
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x0
CPU part        : 0xb76
CPU revision    : 7

Hardware        : BCM2835
Revision        : 0010
Serial          : #############

PWM VALUES INFO AT 6VDC @ 2A:
	PWM IS ON A SCALE OF 1 TO 1024... 
		OUR PWM MOTOR CONTROLER ONLY RESPONDS ONCE WE GET >= 550
	SIREN VALUE NOTES:
		550 For cutoff
		595 For Growl
		650 For LOW WAIL


Install: curl -o installer.sh https://raw.githubusercontent.com/melona380/Raspberry-Pi-Civil-Defense-Siren/master/install.sh && chmod +x installer.sh && sudo ./installer.sh

*/

//RTL_SDR OPTIONS
#define BUFFER_SIZE					512			//POPEN BUFFER SIZE 					DEFAULT: 512
#define FREQUENCY					"147480000"	//LISTEN FREQUENCY IN HZ 				DEFAULT: 147.48MHZ		***NOTE*** CHANGING THIS MAY BREAK FCC REGULATIONS ***ONLY USE WITH HAM RADIO LICENSE ***NOTE***
#define PPM_ERROR					"0"     	//PARTS PER MILLION SDR CORRECTION 		DEFAULT: 0
#define SAMPLE_RATE 				"22000"		//RTL_FM SAMPLE RATE IN HZ				DEFAULT: 22KHZ 			***NOTE*** CHANGING THIS MAY BREAK MULTIMON-NG ***NOTE***

//SIREN OPS OPTIONS
#define CODE_LENGHT					5			//SIREN ACTIVATION CODE LENGHT
#define CODE_CHAR_TIMEOUT 			5			//SIREN CHAR TIMEOUT. NUMBER OF SECONDS SINCE LAST CHAR TO RESET DTMF STRING
#define CODE_SIREN_OFF				"42061"		//SIREN OFF    CODE 			***NOTE*** MUST BE EXCATLY THE LENGHT OF CODE_LENGHT ***NOTE***
#define CODE_SIREN_GROWL			"42062"    	//SIREN GROWL  CODE 			***NOTE*** MUST BE EXCATLY THE LENGHT OF CODE_LENGHT ***NOTE***
#define CODE_SIREN_ALERT 			"42063"    	//SIREN ALERT  CODE 			***NOTE*** MUST BE EXCATLY THE LENGHT OF CODE_LENGHT ***NOTE***
#define CODE_SIREN_ATTACK 			"42064"		//SIREN ATTACK CODE 			***NOTE*** MUST BE EXCATLY THE LENGHT OF CODE_LENGHT ***NOTE***

//SIREN PWM OPTIONS
#define SIREN_PWM_CUTOFF			550			//SIREN PWM VOLTAGE CUTOFF 
#define SIREN_PWM_GROWL 			595			//SIREN PWM VOLTAGE FOR GROWL FUNCTION
#define SIREN_PWM_ALERT				1024		//SIREN PWM VOLTAGE FOR ALERT FUNCTION
#define SIREN_PWM_ATTACK_MAX		1024		//SIREN PWM VOLTAGE FOR ATTACK MAX VOLTAGE
#define SIREN_PWM_ATTACK_MIN		750			//SIREN PWM VOLTAGE FOR ATTACK MIN VOLTAGE

//SIREN TIMING OPTIONS
#define SIREN_TIME_GROWL_REVUP		3.417		//SIREN TIMING FOR REVUP ON GROWL FUNCTION
#define SIREN_TIME_ALERT_REVUP		3.417		//SIREN TIMING FOR REVUP ON ALERT FUNCTION
#define SIREN_TIME_ATTACK_REVUP		10.8		//SIREN TIMING FOR REVUP ON ATTACK FUNCTION
#define SIREN_TIME_ATTACK_REVDOWN	15.4		//SIREN TIMING FOR REVDOWN ON ATTACK FUNCTION
#define SIREN_TIME_ATTACK_HOLD		3000		//SIREN TIMING FOR REVUP ON GROWL FUNCTION

//RASPBERRYPI OPTIONS
#define PWM_PIN 					1			//PWM PIN ON PI 				***NOTE*** ON REV 1 BOARDS USE 1 AS THIS IS BCM PIN 18 ***NOTE***

char dtmf_string[CODE_LENGHT + 1];							//DTMF CHARS CAPTURED
int last_time = 0; 								//TIMESTAMP OF LAST DTMF TONE
int dtmf_count = 0;								//NUMBER OF RECIVED DTMF CHARS

int main(int argc, char *argv []) {
	
	int *Cancel_Var = malloc(sizeof(int));		//PASSABLE SIREN DEAVTIVATION BOOLEAN
  	*Cancel_Var = 0;

  	if (wiringPiSetup () == -1){				//INTI PWM AND WIRING PI LIB
    	printf("FAULT");
   		exit (1);
 	 }

  	pinMode (PWM_PIN, PWM_OUTPUT);				//SET PIN TO PWM PIN 		***NOTE*** BESURE TO USE A H/W PWM GPIO PIN! ***NOTE***

	FILE *in;									//INPUT FILE
	extern FILE *popen();						
	char buff[BUFFER_SIZE];						//BUFFER FOR RECIVED INPUT
	char cmd[1024];								//COMMAND CHAR BUFFER
	char *ps = buff;
	
	strcpy (cmd,"rtl_fm -f ");
  	strcat (cmd, FREQUENCY);
  	strcat (cmd, " -s ");
  	strcat (cmd, SAMPLE_RATE);
  	strcat (cmd, " -p ");
  	strcat (cmd, PPM_ERROR);
  	strcat (cmd, " 2>&1 | multimon-ng -a DTMF -t raw -  ");					  		//***NOTE*** THE 2>&1 FOLLOWING RTL_FM IS TO REDIRECT STDERR TO STDOUT TO PREVENT WRITING ALL OUTPUT TO SCREEN ***NOTE***

  																					//COMMAND: "rtl_fm -f 147480000 -s 22000 -p 0 2>&1 | multimon-ng -a DTMF -t raw - "

	if(!(in = popen(cmd, "r"))){													//SPAWN RTL_FM PROCESS AND READ STDOUT
		exit(1);
	}
	
	while(fgets(buff, sizeof(buff), in)!=NULL){										//PARSE RTL_FM STDOUT BUFFER
		if (strstr(buff, "DTMF: ") != NULL) {										//CHECK FOR DTMF MULTIMON OUTPUT ie. DTMF: 5
			char *dtmfchar = buff + 6;
 			DTMFhandel(dtmfchar, dtmf_count, (unsigned)time(NULL), Cancel_Var);		//HANDEL DTMF CHAR

		}
	}
	
	pclose(in);
	free(Cancel_Var);

	return 0;
}

void DTMFhandel(char *dtmf, int charcount, int timestamp,int *Cancel_Var){			//HANDELS TIME PROCESSING AND DTMF CHAR[] CREATION
	if(last_time == 0){
		last_time = timestamp;														//CHECKS FOR JUST INITILIZED TIMESTAMP
	}
	if(timestamp <= last_time + 5){													//CHECKS IF TIMEOUT HAS PASSED SINCE LAST DTMF CHAR
		last_time = timestamp;														//RESETS TIMESTAMP
		if(charcount + 1 <= CODE_LENGHT){											//COUNTS # OF CHARS SINCE FIRST START
			dtmf_string[charcount] = *dtmf;											//ADDS CHAR TO DTMF STRING			
			if(charcount + 1 == CODE_LENGHT){										//CHECK IF DTMF STRING IS PROPPER LENGHT
				DTMFcheck(dtmf_string, Cancel_Var);									//CHECK DTMF STRING FOR ACTIVATION CODES
			}
		}else{																		
			dtmf_count = 0;															//RESETS DTMF CHAR RECIVED COUNT
			last_time = timestamp;													//RESETS TIMESTAMP
			CleanChar(dtmf_string);													//RESETS DTMF CHARS
			dtmf_string[dtmf_count] = *dtmf;										//ADDS CHAR TO DTMF STRING
		}
	}else{
		last_time = timestamp;														//RESETS TIMESTAMP
		dtmf_count = 0;																//RESETS DTMF CHAR RECIVED COUNT
		CleanChar(dtmf_string);														//ADDS CHAR TO DTMF STRING
		dtmf_string[dtmf_count] = *dtmf;											//RESETS DTMF CHARS
	}
	dtmf_count++;																	//INCREMENT DTMF CHAR COUNT
}

void DTMFcheck(char *dtmf, int *Cancel_Var){
	if(strcmp(dtmf, CODE_SIREN_OFF) ==  0 ){										//CHECK FOR OFF CODE
		printf("Setting Sirens to OFF\n");
		*Cancel_Var = 1;
	}else if(strcmp(dtmf, CODE_SIREN_GROWL) ==  0 ) {								//CHECK FOR GROWL CODE
		printf("Setting Sirens to GROWL\n");
		*Cancel_Var = 1;
		*Cancel_Var = 0;
		SoundSiren(1, Cancel_Var);
	}else if(strcmp(dtmf,CODE_SIREN_ALERT) ==  0 ){									//CHECK FOR ALERT CODE
		printf("Setting Sirens to ALERT\n");
		*Cancel_Var = 1;
		*Cancel_Var = 0;
		SoundSiren(2, Cancel_Var);
	}else if(strcmp(dtmf, CODE_SIREN_ATTACK) ==  0 ){								//CHECK FOR ATTACK CODE
		printf("Setting Sirens to ATTACK\n");
		*Cancel_Var = 1;
		*Cancel_Var = 0;
		SoundSiren(3, Cancel_Var);
	}	
}

void CleanChar(char *var) {															//CHAR[] CLEARER									
    int i = 0;
    while(var[i] != '\0') {
        var[i] = '\0';
        i++;
    }
}

void SirenStop(int cur){				//WIND SIREN DOWN TO CUFOFF THEN RESET PWM
  int pwmVal;
   for (pwmVal = cur; pwmVal > SIREN_PWM_CUTOFF; --pwmVal){
      pwmWrite(PWM_PIN, pwmVal);
      delay(1);
    }
    pwmWrite (PWM_PIN, 0);
}

void* SirenGrowl(void *c){				//SOUND THE SIREN IN GROWL MODE
  int pwmVal;
  int *cncl = (int *)c;											//CANCEL VARIBLE PASSOVER
  for (pwmVal = 0; pwmVal < SIREN_PWM_GROWL; ++pwmVal){			//REV SIREN UP TO ALERT LEVEL
      pwmWrite(PWM_PIN, pwmVal);
      delay(SIREN_TIME_GROWL_REVUP);							//REV UP DELAY
  }
  while (*cncl == 0)
  {
    															//KEEP THE THREAD ALIVE TILL CANCEL IS CALLED
  }
  SirenStop(pwmVal);
  pthread_exit(0);
}

void* SirenAlert(void *c){				//SOUND THE SIREN IN ALERT MODE
  int pwmVal;
  int *cncl = (int *)c;											//CANCEL VARIBLE PASSOVER
  for (pwmVal = 0; pwmVal < SIREN_PWM_ALERT; ++pwmVal){			//REV SIREN UP TO ALERT LEVEL
      pwmWrite(PWM_PIN, pwmVal);
      delay(SIREN_TIME_ALERT_REVUP);							//REV UP DELAY
  }
   while (*cncl == 0)
  {
    															//KEEP THE THREAD ALIVE TILL CANCEL IS CALLED
  }
  SirenStop(pwmVal);
  pthread_exit(0);
}


void* SirenAttack(void *c){				//SOUND THE SIREN IN ATTACK MODE 
	int pwmVal;
	int *cncl = (int *)c;											//CANCEL VARIBLE PASSOVER
	while (*cncl == 0)											//KEEP THE THREAD ALIVE TILL CANCEL IS CALLED
	{
	    for (pwmVal = SIREN_PWM_ATTACK_MIN; pwmVal < SIREN_PWM_ATTACK_MAX; ++pwmVal)  //REVUP LOOP
	    {
	    	if(*cncl == 0){
	    		pwmWrite (PWM_PIN, pwmVal);
	    		delay (SIREN_TIME_ATTACK_REVUP); 						//REV UP DELAY
	    	}else{
	        	break;
	    	}
	    }
		
		if(*cncl != 0){
	    	break;
	    }
	    
	    delay(SIREN_TIME_ATTACK_HOLD);								//REV HOLD DELAY
	    
	    for (pwmVal = SIREN_PWM_ATTACK_MAX; pwmVal > SIREN_PWM_ATTACK_MIN; --pwmVal)   //REVDOWN LOOP
	    {
	    	if(*cncl == 0){
	       		pwmWrite (PWM_PIN, pwmVal);
	        	delay (SIREN_TIME_ATTACK_REVDOWN);						//REV DOWN DELAY
	    	}else{
	        	break;
	    	}
	    }
	}
	SirenStop(pwmVal);
	pthread_exit(0);
}


void SoundSiren(int mode, int *go){		//SIREN THREAD ASSIGNMENT FUNCTION
  pthread_t tid;
  int err;
  switch(mode){
      case 1:												//CREATE A GROWL THREAD
        err = pthread_create(&tid, NULL, &SirenGrowl, (void *)go);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        break;	
      case 2:												//CREATE A ALERT THREAD
        err = pthread_create(&tid, NULL, &SirenAlert, (void *)go);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        break;
      case 3:												//CREATE A ATTACK THREAD
        err = pthread_create(&tid, NULL, &SirenAttack, (void *)go);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        break;
    }
}
