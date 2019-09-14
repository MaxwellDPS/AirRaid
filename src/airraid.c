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

int dtmf_count = 0;

int main(int argc, char *argv []) {
	int choice, wc, wc2,recvlen,fd,command;
	int loop = 0;
	int da = 0;
	int net = 0;
	int radio = 0;
	int custmhz =0;
	int cli = 0;

	float mhz;

	FILE *in;
	FILE *fp= NULL;
	extern FILE *popen();

	pid_t process_id = 0;
	pid_t sid = 0;

	struct sockaddr_in myaddr;
	struct sockaddr_in remaddr;
    	socklen_t addrlen = sizeof(remaddr);

	unsigned char buf[BUFSIZE];
	char buff[BUFFER_SIZE];
	char *ps = buff;
	char freq[30];
	char cmd[500];
	strcpy(cmd, "rtl_fm -f " FREQUENCY " -s " SAMPLE_RATE " -p " PPM_ERROR " 2>&1 | multimon-ng -a DTMF -t raw -"); 				//***NOTE*** THE 2>&1 FOLLOWING RTL_FM IS TO REDIRECT STDERR TO STDOUT TO PREVENT WRITING ALL OUTPUT TO SCREEN ***NOTE***

  	if (wiringPiSetup () == -1){
   		printf("FAULT");
   		exit (1);
 	 }

	pinMode (PWM_PIN, PWM_OUTPUT);				//SET PIN TO PWM PIN 		***NOTE*** BESURE TO USE A H/W PWM GPIO PIN! ***NOTE***
 	pinMode (DTMFLED_PIN, OUTPUT) ;				//SET PIN TO LED PIN
 	pinMode (ONLED_PIN, OUTPUT) ;				//SET PIN TO LED PIN
	pinMode (ACTLED_PIN, OUTPUT) ;				//SET PIN TO LED PIN

	digitalWrite (ONLED_PIN, HIGH);				//SET STATUS LED TO ON
    	digitalWrite (ACTLED_PIN, LOW);				//SET ACTIVATION LED TO OFF
	digitalWrite (DTMFLED_PIN, LOW);			//SET DTMF RECV LED TO OFF

	if (argc > 1){
		for(int i=0; i < argc; i++){
			if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help" )){
				helper();
				return 0;
			}else if(!strcmp(argv[i], "-n") || !strcmp(argv[i], "--network" )){
				net = 1;
			}else if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon" )){
				da = 1;
			}else if(!strcmp(argv[i], "-f") || !strcmp(argv[i], "--freq" )){
				strcpy(freq, argv[i+1]);
				strcpy(cmd, "rtl_fm -f ");
				strcat(cmd, freq);
				strcat(cmd, " -s " SAMPLE_RATE " -p " PPM_ERROR " 2>&1 | multimon-ng -a DTMF -t raw -");
				custmhz = 1;
			}else if(!strcmp(argv[i], "-r") || !strcmp(argv[i], "--radio" )){
				radio = 1;
			}else if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--cli" )){
				cli = 1;
			}
		}

		if (radio == 1 && net == 1){
			printf("\n\nYou can only have Network or Radio\n\n-n,--network for UDP Control\n-d, --daemon To daemonize\n\n");
	                return 0;
		}

		if(custmhz == 1 && radio == 0){
			helper();
        	        return 0;
		}

	}else{
		helper();
		return 0;
	}


	//printf("radio: %d; Daemon: %d, UDP: %d\n",radio,da,net);

	if(da == 1){
		process_id = fork();

		if(net ==0 && radio ==0){
			net = 1;
		}

		if (process_id < 0){
			printf("fork failed!\n");
			exit(1);
		}

		if (process_id > 0){
			printf("Running as PID %d \n", process_id);
			exit(0);
		}

		umask(0);
		sid = setsid();

		if(sid < 0)
			exit(1);

		chdir("/");
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		fp = fopen ("/var/log/AirRaid.log", "w+");
		sleep(1);
	}

	int* Cancel_Var = (int *)malloc(sizeof(int));
  	*Cancel_Var = 1;

  	pinMode (PWM_PIN, PWM_OUTPUT);				//SET PIN TO PWM PIN	***NOTE*** BESURE TO USE A H/W PWM GPIO PIN! ***NOTE***

	if (cli == 1){
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
	//		printf("\n\nCNCL: %d\n\n",*Cancel_Var);

			switch(choice){
				case 1:
					ACTcheck(CODE_SIREN_GROWL,Cancel_Var);
					break;
				case 2:
					ACTcheck(CODE_SIREN_ALERT,Cancel_Var);
					break;
				case 3:
					printf("\n1. WAIL till OFF\n2. Wail X times\n\nChoice: ");
					scanf("%d",&wc);
					switch(wc){
						case 1:
							*Cancel_Var = 1;
							ACTcheck(CODE_SIREN_ATTACK,Cancel_Var);
							break;
						case 2:
							*Cancel_Var = 1;
							printf("How man WAIL loops: ");
							scanf("%d",&wc2);
							*Cancel_Var = 0;
							*Cancel_Var = wc2;
							ACTcheck(CODE_SIREN_ATTACK,Cancel_Var);
							printf("Wailing for %d Times",wc2);
							break;
					}
					break;
				case 4:
					ACTcheck(CODE_SIREN_OFF,Cancel_Var);
					break;
				case 99:
					loop = 1;
					break;
			}
		}
	}else if (net == 1){
	        if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        	        perror("cannot create socket\n");
        	        return 0;
	        }

	        memset((char *)&myaddr, 0, sizeof(myaddr));
	        myaddr.sin_family = AF_INET;
	        myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        	myaddr.sin_port = htons(SERVICE_PORT);

	        if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
	               	perror("bind failed");
        	        return 0;
	        }

		printf("Listening on UDP port %d\n",SERVICE_PORT);
		if (da !=0){
			fprintf(fp, "Listening on UDP port %d\n\n",SERVICE_PORT);
			fflush(fp);
		}


		for (;;) {
        	      	  recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
               		 if (recvlen > 0) {
        	                buf[recvlen] = 0;
        	                command = atoi(buf);
				//printf("received message: \"%s\"\n", buf);
				switch(command){
					case 1:
						if (da !=0){
							fprintf(fp, "**Setting Sirens to GROWL**\n");
							fflush(fp);
						}
						ACTcheck(CODE_SIREN_GROWL,Cancel_Var);
						break;
					case 2:
						if (da !=0){
							fprintf(fp, "**Setting Sirens to ALERT**\n");
							fflush(fp);
						}
						ACTcheck(CODE_SIREN_ALERT,Cancel_Var);
						break;
					case 3:
						if (da !=0){
							fprintf(fp, "**Setting Sirens to ATTACK**\n");
							fflush(fp);
						}
						*Cancel_Var = 1;
						ACTcheck(CODE_SIREN_ATTACK,Cancel_Var);
						break;
					case 4:
						if (da !=0){
							fprintf(fp, "**Setting Sirens to OFF**\n");
							fflush(fp);
						}
						ACTcheck(CODE_SIREN_OFF,Cancel_Var);
						break;
					case 99:
						if (da !=0){
							fprintf(fp, "**Exiting**\n");
							fflush(fp);
							fclose(fp);
						}
						pinMode (PWM_PIN, INPUT);
						free(Cancel_Var);
						return 0;
						break;
				}

        	        }
	        }

	}else if (radio==1){
		system("killall -9 rtl_fm");
		if(!(in = popen(cmd, "r"))){ 				//SPAWN RTL_FM PROCESS AND READ STDOUT
			exit(1);
		}
		if(custmhz == 0){
			mhz = atof(FREQUENCY)/1000000.0;
		}else{
			mhz = atof(freq)/1000000.0;
		}
		printf("Listening for DTMF on %.4f MHZ\n",mhz);

		if (da !=0){
			fprintf(fp, "Listening for DTMF on %.4f MHZ\n",mhz);
			fflush(fp);
		}

		while(fgets(buff, sizeof(buff), in)!=NULL){		//PARSE RTL_FM STDOUT BUFFER
			if (strstr(buff, "DTMF: ") != NULL) {		//CHECK FOR DTMF MULTIMON OUTPUT ie. DTMF: 5
				char *dtmfchar = buff + 6;
				digitalWrite (DTMFLED_PIN, HIGH);
 				DTMFhandel(dtmfchar, dtmf_count, (unsigned)time(NULL), Cancel_Var);		//HANDEL DTMF CHAR
 				delay(50);
   		 		digitalWrite (DTMFLED_PIN,  LOW);
			}
		}
		pclose(in);
	}

  	pinMode (PWM_PIN, INPUT);
	pinMode (DTMFLED_PIN, INPUT);				//SET PIN TO LED PIN
 	pinMode (ONLED_PIN, INPUT);				//SET PIN TO LED PIN
	pinMode (ACTLED_PIN, INPUT);				//SET PIN TO LED PIN
	free(Cancel_Var);
	return 0;
}

void helper(){
	printf("----------------------------------------------------------\n");
	printf("AirRaid PWM RPI Hackable Civil Defense Siren Controler\n");
	printf("----------------------------------------------------------\n");
	printf("-c, --cli for STDIO Control\n");
	printf("-n, --network for UDP Control\n");
	printf("-d, --daemon To daemonize MUST BE USED with -r OR  -n (Defaults to --network)\n");
	printf("-r, --radio to use RTL-SDR for DTMF\n");
	printf("-f, --freq Set listen Frequency in HZ Default: 147480000 HZ\n\n");
}
