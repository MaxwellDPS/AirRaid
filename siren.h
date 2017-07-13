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

void  SirenStop  (int cur);																//THIS FUNCTION REVS DOWN THE SIREN AND KILLS THE PWM HOLD ON THE PIN
void* SirenGrowl (void *c);																//THIS FUNCTION REVES THE SIREN UP TO A GROWL UNTIL CNCL = 1 THE IT CALLS SIRENSTOP()
void* SirenAlert (void *c);																//THIS FUNCTION REVES THE SIREN UP TO ALERT UNTIL CNCL = 1 THE IT CALLS SIRENSTOP()
void* SirenAttack(void *c);																//THIS FUNCTION REVES THE SIREN UP TO ATTACK AND WAILS UNTIL CNCL = 1 THE IT CALLS SIRENSTOP()
void  SoundSiren (int mode, int *go);									//THIS FUNCTION CREATES A THREAD FOR THE SIREN FUNCTION THEN PASSES CNCL VAR
void  DTMFhandel (char *dtmf, int charcount, int timestamp, int *Cancel_Var);			//HANDELS DTMF RECOGNITION
void  DTMFcheck (char *dtmf, int *Cancel_Var);
void  CleanChar (char *var); 

extern char dtmf_string[5]; 
extern int last_time; 
extern int dtmf_count;

#endif
