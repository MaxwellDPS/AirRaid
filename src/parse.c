#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <json-c/json.h>
#include "siren.h"

void configParse(FILE *config){
	char configBuf[10000];

	fread(configBuf, 10000, 1, config);
	fclose(config);

	parsed_json = json_tokener_parse(configBuf);

	json_object_object_get_ex(parsed_json, "Network", &Network);
	json_object_object_get_ex(parsed_json, "SDR", &Sdr);
	json_object_object_get_ex(parsed_json, "SirenOPS", &SirenOPS);
	json_object_object_get_ex(parsed_json, "SirenPWM", &SirenPWM);
	json_object_object_get_ex(parsed_json, "SirenTiming", &SirenTiming);
	json_object_object_get_ex(parsed_json, "RPIOptions", &RPIOptions);

	//Network
	json_object_object_get_ex(Network,"UDP Port", &Network_port);

	//SDR
	json_object_object_get_ex(Sdr,"FREQUENCY", &Sdr_frequency);
	json_object_object_get_ex(Sdr,"PPM_ERROR", &Sdr_ppm);
	json_object_object_get_ex(Sdr,"SAMPLE_RATE", &Sdr_sampleRate);

	//SirenOPS
	json_object_object_get_ex(SirenOPS,"CODE_LENGHT", &SirenOPS_codeLenght);
	json_object_object_get_ex(SirenOPS,"CODE_CHAR_TIMEOUT", &SirenOPS_codetimeout);
	json_object_object_get_ex(SirenOPS,"CODE_SIREN_OFF", &SirenOPS_codeOFF);
	json_object_object_get_ex(SirenOPS,"CODE_SIREN_GROWL", &SirenOPS_codeGROWL);
	json_object_object_get_ex(SirenOPS,"CODE_SIREN_ALERT", &SirenOPS_codeALERT);
	json_object_object_get_ex(SirenOPS,"CODE_SIREN_ATTACK", &SirenOPS_codeATTACK);

	//SirenPWM
	json_object_object_get_ex(SirenPWM,"SIREN_PWM_CUTOFF", &SirenPWM_cutoff);
	json_object_object_get_ex(SirenPWM,"SIREN_PWM_GROWL", &SirenPWM_Growl);
	json_object_object_get_ex(SirenPWM,"SIREN_PWM_ALERT", &SirenPWM_Alert);
	json_object_object_get_ex(SirenPWM,"SIREN_PWM_ATTACK_MAX", &SirenPWM_AttackMax);
	json_object_object_get_ex(SirenPWM,"SIREN_PWM_ATTACK_MIN", &SirenPWM_AttackMin);

	//SirenTiming
	json_object_object_get_ex(SirenTiming,"SIREN_TIME_GROWL_REVUP", &SirenTiming_GrowlRevUP);
	json_object_object_get_ex(SirenTiming,"SIREN_TIME_ALERT_REVUP", &SirenTiming_AlertRevUP);
	json_object_object_get_ex(SirenTiming,"SIREN_TIME_ATTACK_REVUP", &SirenTiming_AttackRevUP);
	json_object_object_get_ex(SirenTiming,"SIREN_TIME_ATTACK_REVDOWN", &SirenTiming_AttackRevDown);
	json_object_object_get_ex(SirenTiming,"SIREN_TIME_ATTACK_HOLD", &SirenTiming_AttackHold);

	//RPIOptions
	json_object_object_get_ex(RPIOptions,"PWM_PIN", &RPIOptions_PWMPin);
	json_object_object_get_ex(RPIOptions,"ACTLED_PIN", &RPIOptions_ActLedPin);
	json_object_object_get_ex(RPIOptions,"DTMFLED_PIN", &RPIOptions_DTMFLedPin);
	json_object_object_get_ex(RPIOptions,"ONLED_PIN", &RPIOptions_ONLedPin);


	//Network
	cSERVICE_PORT = atoi(json_object_get_string(Network_port));

	//RTL_SDR OPTIONS
	strcpy(cFREQUENCY, json_object_get_string(Sdr_frequency));
	strcpy(cPPM_ERROR, json_object_get_string(Sdr_ppm));
	strcpy(cSAMPLE_RATE, json_object_get_string(Sdr_sampleRate));

	//SIREN OPS OPTIONS
	cCODE_LENGHT = atoi(json_object_get_string(SirenOPS_codeLenght));
	cCODE_CHAR_TIMEOUT = atoi(json_object_get_string(SirenOPS_codetimeout));
	strcpy(cCODE_SIREN_OFF, json_object_get_string(SirenOPS_codeOFF));
	strcpy(cCODE_SIREN_GROWL, json_object_get_string(SirenOPS_codeGROWL));
	strcpy(cCODE_SIREN_ALERT, json_object_get_string(SirenOPS_codeALERT));
	strcpy(cCODE_SIREN_ATTACK, json_object_get_string(SirenOPS_codeATTACK));

	//SIREN PWM OPTIONS
	cSIREN_PWM_CUTOFF = atoi(json_object_get_string(SirenPWM_cutoff));
	cSIREN_PWM_GROWL = atoi(json_object_get_string(SirenPWM_Growl));
	cSIREN_PWM_ALERT = atoi(json_object_get_string(SirenPWM_Alert));
	cSIREN_PWM_ATTACK_MAX = atoi(json_object_get_string(SirenPWM_AttackMax));
	cSIREN_PWM_ATTACK_MIN = atoi(json_object_get_string(SirenPWM_AttackMin));

	//SIREN TIMING OPTIONS
	cSIREN_TIME_GROWL_REVUP = atof(json_object_get_string(SirenTiming_GrowlRevUP));
	cSIREN_TIME_ALERT_REVUP = atof(json_object_get_string(SirenTiming_AlertRevUP));
	cSIREN_TIME_ATTACK_REVUP = atof(json_object_get_string(SirenTiming_AttackRevUP));
	cSIREN_TIME_ATTACK_REVDOWN = atof(json_object_get_string(SirenTiming_AttackRevDown));
	cSIREN_TIME_ATTACK_HOLD = atof(json_object_get_string(SirenTiming_AttackHold));

	//RASPBERRYPI OPTIONS
	cPWM_PIN = atoi(json_object_get_string(RPIOptions_PWMPin));
	cACTLED_PIN = atoi(json_object_get_string(RPIOptions_ActLedPin));
	cDTMFLED_PIN = atoi(json_object_get_string(RPIOptions_DTMFLedPin));
	cONLED_PIN = atoi(json_object_get_string(RPIOptions_ONLedPin));
}

void helper(){
	printf("----------------------------------------------------------\n");
	printf("AirRaid PWM RPI Hackable Civil Defense Siren Controler\n");
	printf("----------------------------------------------------------\n");
	printf("-C, --config config.json Path to fonfig file (Defaults to ./config.json)\n");
	printf("-s, --ShowConfig Show Config Datal\n");
	printf("-c, --cli for STDIO Control\n");
	printf("-d, --daemon To daemonize MUST BE USED with -r OR  -n (Defaults to --network)\n");
	printf("-n, --network for UDP Control\n");
	printf("-r, --radio to use RTL-SDR for DTMF\n\n");
}

void showConfig(){
	printf("\nSDR CONFIG:\n  freq: %s\n  PPM: %s\n  SAMP: %s\n",cFREQUENCY,cPPM_ERROR,cSAMPLE_RATE);
	printf("\nSIREN OPS CONFIG:\n  Code Len: %d\n  Timeout: %d\n  OFF Code: %s\n  Growl CODE: %s\n  Alert CODE: %s\n  Attack CODE: %s\n",cCODE_LENGHT,cCODE_CHAR_TIMEOUT,cCODE_SIREN_OFF,cCODE_SIREN_GROWL,cCODE_SIREN_ALERT,cCODE_SIREN_ATTACK);
	printf("\nSIREN PWM CONFIG:\n  CUTOFF: %d\n  GROWL: %d\n  ALERT: %d\n  ATTACK MAX: %d\n  ATTACK MIN: %d\n",cSIREN_PWM_CUTOFF,cSIREN_PWM_GROWL,cSIREN_PWM_ALERT,cSIREN_PWM_ATTACK_MAX,cSIREN_PWM_ATTACK_MIN);
	printf("\nSIREN Timing CONFIG:\n  Growl RevUP ms: %.3f\n  Alert RevUP ms: %.3f\n  Attack RevUP ms: %.3f\n  Attack RevDown ms: %.3f\n  Attack hold: %.3f\n",cSIREN_TIME_GROWL_REVUP,cSIREN_TIME_ALERT_REVUP,cSIREN_TIME_ATTACK_REVUP,cSIREN_TIME_ATTACK_REVDOWN,cSIREN_TIME_ATTACK_HOLD);
	printf("\nNetwork CONFIG:\n  UDP Port: %d;\n",cSERVICE_PORT);
	printf("\nRPI OPS CONFIG:\n  PWM PIN: %d\n  Activation LED PIN: %d\n  DTMF Revied LED PIN: %d\n  ON LED PIN: %d\n\n",cPWM_PIN,cACTLED_PIN,cDTMFLED_PIN,cONLED_PIN);
}

