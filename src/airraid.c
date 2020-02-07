#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "siren.h"


//allows safe shutdown
int *Cancel_Var;

void sig_handler(int sig){
	signal(SIGINT, sig_handler);
	signal(SIGKILL, sig_handler);
	shutdownh();
}

int main(int argc, char *argv []) {
	int net = 0, cli = 0, shcfg = 0, configt = 0;
	size_t i;
	FILE *config;

	da = radio = 0;
	pid_t process_id = 0, sid = 0;

	//Initalize Wiring Pi - RIP :(
  	if (wiringPiSetup () == -1){
   		printf("FAULT");
   		exit (1);
 	 }

	//Read Command Line Args
	if (argc > 1){
		for(int i=0; i < argc; i++){
			if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help" )){
				helper();
				return 0;
			}else if(!strcmp(argv[i], "-C") || !strcmp(argv[i], "--config" )){
				strcpy(cfgfile, argv[i+1]);
				configt = 1;
			}else if(!strcmp(argv[i], "-n") || !strcmp(argv[i], "--network" )){
				net = 1;
			}else if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon" )){
				da = 1;
			}else if(!strcmp(argv[i], "-s") || !strcmp(argv[i], "--ShowConfig" )){
				shcfg = 1;
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
		if (configt == 1 && (radio == 0 && net == 0 && cli == 0)){
			helper();
			return 0;
		}
	}else{
		helper();
		return 0;
	}

	//Parse Config file
	if(configt == 0){
		if( access( "config.json", F_OK ) != -1 ) {
			config = fopen("config.json", "r");
			configParse(config);
		} else {
			printf("\nCan find config.json\n\n");
			helper();
			return 1;
		}
	}else{
		if( access( cfgfile, F_OK ) != -1 ) {
			config = fopen(cfgfile, "r");
			configParse(config);
		} else {
			printf("\nCan find config.json\n\n");
			helper();
			return 1;
		}
	}

	if (shcfg == 1)
		showConfig();

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

	signal(SIGINT, sig_handler);
	signal(SIGKILL, sig_handler);

	Cancel_Var = (int *)malloc(sizeof(int));
  	*Cancel_Var = 1;

	SetGPIO();

	if (cli == 1){
		execCLI(Cancel_Var);
	}else if (net == 1){
		execNET(Cancel_Var);
	}else if (radio==1){
		execRadio(Cancel_Var);
	}

	// Not An Exit Route :(
	return 2;
}

void shutdownh(){
        if(radio == 1){
                system("killall -9 rtl_fm");
                pclose(in);
        }

        pinMode (cPWM_PIN, INPUT);
        pinMode (cDTMFLED_PIN, INPUT);                           //SET PIN TO LED PIN
        pinMode (cONLED_PIN, INPUT);                             //SET PIN TO LED PIN
        pinMode (cACTLED_PIN, INPUT);                            //SET PIN TO LED PIN

        free(Cancel_Var);

        if (da !=0){
                fprintf(fp, "**Caught SIG Exiting**\n");
                fflush(fp);
                fclose(fp);
        }else{
                printf("\nSIG CAUGHT EXITING\n");
        }

	exit(0);

}
