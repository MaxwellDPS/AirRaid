#ifndef siren
#define siren

#define BUFSIZE 	2048

//RTL_SDR OPTIONS
#define BUFFER_SIZE	512	//POPEN BUFFER SIZE DEFAULT: 512

//individual Vars
char cfgfile[500];
int da, radio;
FILE *in, *fp;

//DTMF STUFZ
char dtmf_string[50];
int dtmf_count;

/*

Funtions

*/


//siren.c
void  SirenStop  (int cur);				//THIS FUNCTION REVS DOWN THE SIREN AND KILLS THE PWM HOLD ON THE PIN
void* SirenGrowl (void *c);				//THIS FUNCTION REVES THE SIREN UP TO A GROWL UNTIL CNCL = 1 THE IT CALLS SIRENSTOP()
void* SirenAlert (void *c);				//THIS FUNCTION REVES THE SIREN UP TO ALERT UNTIL CNCL = 1 THE IT CALLS SIRENSTOP()
void* SirenAttack(void *c);				//THIS FUNCTION REVES THE SIREN UP TO ATTACK AND WAILS UNTIL CNCL = 1 THE IT CALLS SIRENSTOP()
void  SoundSiren (int mode, int *go);			//THIS FUNCTION CREATES A THREAD FOR THE SIREN FUNCTION THEN PASSES CNCL VAR

//exec.c
void SetGPIO();						//SETS GPIO TO OPERATION MODES
void execRadio(int *Cancel_Var);			//STARTS RADIO THREAD
void execNET(int *Cancel_Var);				//STARTS NETWORK THREAD
void execCLI(int *Cancel_Var);				// STARTS CLI

//netcli.c
void  ACTcheck (char *dtmf, int *Cancel_Var);  		//THIS FUNCTION CHECKS THE DTMF CHAR

//Radio.c
void CleanChar (char *var);				//CLEAN DTMF CHAR BUFFER
void DTMFcheck (char *dtmf, int *Cancel_Var);		//CHECK DTMF FOR MATCH
void DTMFhandel (char *dtmf, int charcount, int timestamp,int *Cancel_Var);	// CALLS ON DTMF RECV

//airraid.c
void shutdownh();					//SHUTDOWN SIREN - WIND DOWN CROSS CALL
void sig_handler(int sig);				//HANDEL SIGNALS

//parse.c
void configParse(FILE *config);				//PARSE CONFIG
void showConfig();					//PRINT CONFIG TO CLI
void helper();						//PRINT HELP TO CLI


/*

Config file Values

*/

//Primary JSON Structure
struct json_object *parsed_json;
struct json_object *Network;
struct json_object *Sdr;
struct json_object *SirenOPS;
struct json_object *SirenPWM;
struct json_object *SirenTiming;
struct json_object *RPIOptions;

//network config
struct json_object *Network_port;

//sdr config
struct json_object *Sdr_frequency;
struct json_object *Sdr_ppm;
struct json_object *Sdr_sampleRate;

//SirenOPS Config
struct json_object *SirenOPS_codeLenght;
struct json_object *SirenOPS_codetimeout;
struct json_object *SirenOPS_codeOFF;
struct json_object *SirenOPS_codeGROWL;
struct json_object *SirenOPS_codeALERT;
struct json_object *SirenOPS_codeATTACK;

//SirenPWM Config
struct json_object *SirenPWM_cutoff;
struct json_object *SirenPWM_Growl;
struct json_object *SirenPWM_Alert;
struct json_object *SirenPWM_AttackMax;
struct json_object *SirenPWM_AttackMin;

//Siren Timing Config
struct json_object *SirenTiming_GrowlRevUP;
struct json_object *SirenTiming_AlertRevUP;
struct json_object *SirenTiming_AttackRevUP;
struct json_object *SirenTiming_AttackRevDown;
struct json_object *SirenTiming_AttackHold;

//RPI Options config
struct json_object *RPIOptions_PWMPin;
struct json_object *RPIOptions_ActLedPin;
struct json_object *RPIOptions_DTMFLedPin;
struct json_object *RPIOptions_ONLedPin;

/*

THe indiviual Vars

*/

//Network
int cSERVICE_PORT;

//RTL_SDR OPTIONS
char cFREQUENCY[50];
char cPPM_ERROR[10];
char cSAMPLE_RATE[15];

//SIREN OPS OPTIONS
int cCODE_LENGHT;
int cCODE_CHAR_TIMEOUT;
char cCODE_SIREN_OFF[50];
char cCODE_SIREN_GROWL[50];
char cCODE_SIREN_ALERT[50];
char cCODE_SIREN_ATTACK[50];

//SIREN PWM OPTIONS
int cSIREN_PWM_CUTOFF;
int cSIREN_PWM_GROWL;
int cSIREN_PWM_ALERT;
int cSIREN_PWM_ATTACK_MAX;
int cSIREN_PWM_ATTACK_MIN;

//SIREN TIMING OPTIONS
double cSIREN_TIME_GROWL_REVUP;
double cSIREN_TIME_ALERT_REVUP;
double cSIREN_TIME_ATTACK_REVUP;
double cSIREN_TIME_ATTACK_REVDOWN;
double cSIREN_TIME_ATTACK_HOLD;

//RASPBERRYPI OPTIONS
int cPWM_PIN;
int cACTLED_PIN;
int cDTMFLED_PIN;
int cONLED_PIN;


#endif
