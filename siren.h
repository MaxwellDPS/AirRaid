#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


/*
HACKABLE CIVIL DEFENSE SIREN POF
COPYRIGHT MAX WATERMOLEN 2017
*/

#ifndef siren
# define siren
#define CODE_LENGHT					5

void  SirenStop  (int cur);												//THIS FUNCTION REVS DOWN THE SIREN AND KILLS THE PWM HOLD ON THE PIN
void* SirenGrowl (void *c);												//THIS FUNCTION REVES THE SIREN UP TO A GROWL UNTIL CNCL = 1 THE IT CALLS SIRENSTOP()
void* SirenAlert (void *c);												//THIS FUNCTION REVES THE SIREN UP TO ALERT UNTIL CNCL = 1 THE IT CALLS SIRENSTOP()
void* SirenAttack(void *c);												//THIS FUNCTION REVES THE SIREN UP TO ATTACK AND WAILS UNTIL CNCL = 1 THE IT CALLS SIRENSTOP()
void  SoundSiren (int mode, int *go);				    	//THIS FUNCTION CREATES A THREAD FOR THE SIREN FUNCTION THEN PASSES CNCL VAR
void  DTMFhandel (char *dtmf, int charcount, int timestamp, int *Cancel_Var);			//THIS FUNCTION HANDELS DTMF RECOGNITION
void  DTMFcheck (char *dtmf, int *Cancel_Var);   //THIS FUNCTION CHECKS THE DTMF CHAR
void  CleanChar (char *var);                     //THIS FUNCTION CLEARS THE DTMF CHAR

extern char dtmf_string[CODE_LENGHT + 1];        //DTMF CHAR
extern int last_time;                            // TIME OF LAST RECIVED DTMF CHAR
extern int dtmf_count;                           //COUNT OF RECIEVED DTMF CHARS

#endif
