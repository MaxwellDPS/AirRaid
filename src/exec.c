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
#include <signal.h>
#include "siren.h"

//Radio Vars
extern FILE *popen();
int dtmf_count = 0;

void execCLI(int *Cancel_Var){
	int choice, wc, wc2;
	int loop = 0;

	while(loop==0){
		printf("\n----------------------------------------------------------------\n");
		printf("Air Control\n");
		printf("----------------------------------------------------------------\n");
		printf("1. Growl\n");
		printf("2. Alert\n");
		printf("3. Attack\n");
		printf("4. Off\n");
		printf("99. Quit\n");
		printf("----------------------------------------------------------------\n\n");
		printf("Make a Choice: ");
		scanf("%d",&choice);

		switch(choice){
			case 1:
				ACTcheck(cCODE_SIREN_GROWL,Cancel_Var);
				break;
			case 2:
				ACTcheck(cCODE_SIREN_ALERT,Cancel_Var);
				break;
			case 3:
				printf("\n1. WAIL till OFF\n2. Wail X times\n\nChoice: ");
				scanf("%d",&wc);
				switch(wc){
					case 1:
						*Cancel_Var = 1;
						ACTcheck(cCODE_SIREN_ATTACK,Cancel_Var);
						break;
					case 2:
						*Cancel_Var = 1;
						printf("How man WAIL loops: ");
						scanf("%d",&wc2);
						*Cancel_Var = 0;
						*Cancel_Var = wc2;
						ACTcheck(cCODE_SIREN_ATTACK,Cancel_Var);
						printf("Wailing for %d Times",wc2);
						break;
				}
				break;
			case 4:
				ACTcheck(cCODE_SIREN_OFF,Cancel_Var);
				break;
			case 99:
				shutdownh();
				break;
		}
	}
}

void execNET(int *Cancel_Var){
	int recvlen,fd,command;
	unsigned char buf[BUFSIZE];
	struct sockaddr_in myaddr;
	struct sockaddr_in remaddr;
	socklen_t addrlen = sizeof(remaddr);

	signal(SIGINT, sig_handler);
	signal(SIGKILL, sig_handler);

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket\n");
		shutdownh();
	}

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(cSERVICE_PORT);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		shutdownh();
	}

	printf("Listening on UDP port %d\n",cSERVICE_PORT);

	if (da !=0){
		fprintf(fp, "Listening on UDP port %d\n\n",cSERVICE_PORT);
		fflush(fp);
	}

	for(;;) {
		recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		if (recvlen > 0) {
			buf[recvlen] = 0;
			command = atoi(buf);
			switch(command){
				case 1:
					if (da !=0){
						fprintf(fp, "**Setting Sirens to GROWL**\n");
						fflush(fp);
					}
					ACTcheck(cCODE_SIREN_GROWL,Cancel_Var);
					break;
				case 2:
					if (da !=0){
						fprintf(fp, "**Setting Sirens to ALERT**\n");
						fflush(fp);
					}
					ACTcheck(cCODE_SIREN_ALERT,Cancel_Var);
					break;
				case 3:
					if (da !=0){
						fprintf(fp, "**Setting Sirens to ATTACK**\n");
						fflush(fp);
					}
					*Cancel_Var = 1;
					ACTcheck(cCODE_SIREN_ATTACK,Cancel_Var);
					break;
				case 4:
					if (da !=0){
						fprintf(fp, "**Setting Sirens to OFF**\n");
						fflush(fp);
					}
					ACTcheck(cCODE_SIREN_OFF,Cancel_Var);
					break;
				case 99:
					if (da !=0){
						fprintf(fp, "**Exiting**\n");
						fflush(fp);
						fclose(fp);
					}
					shutdownh();
			}
		}
	}
}

void execRadio(int *Cancel_Var){
		char cmd[500];
		char freq[30];
		char buff[BUFFER_SIZE];
		char *ps = buff;
		float mhz;

		strcpy(cmd, "rtl_fm -f ");
		strcat(cmd,  cFREQUENCY);
		strcat(cmd,  " -s ");
		strcat(cmd,  cSAMPLE_RATE);
		strcat(cmd,  " -p ");
		strcat(cmd,  cPPM_ERROR);
		strcat(cmd,  " 2>&1 | multimon-ng -a DTMF -t raw -"); 				//***NOTE*** THE 2>&1 FOLLOWING RTL_FM IS TO REDIRECT STDERR TO STDOUT TO PREVENT WRITING ALL OUTPUT TO SCREEN ***NOTE***

		signal(SIGINT, sig_handler);
		signal(SIGKILL, sig_handler);

		system("killall -9 rtl_fm");
		if(!(in = popen(cmd, "r"))){ 				//SPAWN RTL_FM PROCESS AND READ STDOUT
			exit(1);
		}

		mhz = atof(cFREQUENCY)/1000000.0;
		printf("Listening for DTMF on %.4f MHZ\n",mhz);

		if (da !=0){
			fprintf(fp, "Listening for DTMF on %.4f MHZ\n",mhz);
			fflush(fp);
		}

		while(fgets(buff, sizeof(buff), in)!=NULL){		//PARSE RTL_FM STDOUT BUFFER
			if (strstr(buff, "DTMF: ") != NULL) {		//CHECK FOR DTMF MULTIMON OUTPUT ie. DTMF: 5
				char *dtmfchar = buff + 6;
				digitalWrite (cDTMFLED_PIN, HIGH);
 				DTMFhandel(dtmfchar, dtmf_count, (unsigned)time(NULL), Cancel_Var);		//HANDEL DTMF CHAR
 				delay(50);
   		 		digitalWrite (cDTMFLED_PIN,  LOW);
			}
		}

		pclose(in);
}

void SetGPIO(){
	pinMode (cPWM_PIN, PWM_OUTPUT);				//SET PIN TO PWM PIN 		***NOTE*** BESURE TO USE A H/W PWM GPIO PIN! ***NOTE***
 	pinMode (cDTMFLED_PIN, OUTPUT);				//SET PIN TO LED PIN
 	pinMode (cONLED_PIN, OUTPUT);				//SET PIN TO LED PIN
	pinMode (cACTLED_PIN, OUTPUT);				//SET PIN TO LED PIN

	digitalWrite (cONLED_PIN, HIGH);			//SET STATUS LED TO ON
 	digitalWrite (cACTLED_PIN, LOW);			//SET ACTIVATION LED TO OFF
	digitalWrite (cDTMFLED_PIN, LOW);			//SET DTMF RECV LED TO OFF
}


