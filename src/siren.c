#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "siren.h"

void SirenStop(int cur){
	int pwmVal;

	for (pwmVal = cur; pwmVal > cSIREN_PWM_CUTOFF; --pwmVal){
		pwmWrite(cPWM_PIN, pwmVal);
		delay(1);
	}

	pwmWrite (cPWM_PIN, 0);
}

void* SirenGrowl(void *c){
	int pwmVal;
	int *cncl = (int *)c;

	delay(500);

	for (pwmVal = 0; pwmVal < cSIREN_PWM_GROWL; ++pwmVal){
		pwmWrite(cPWM_PIN, pwmVal);
		if(*cncl == 0)
   			break;
		delay(cSIREN_TIME_GROWL_REVUP);
	}

	while (*cncl > 0){
	}

	SirenStop(pwmVal);
	pthread_exit(0);

}

void* SirenAlert(void *c){
	int pwmVal;
	int *cncl = (int *)c;

	delay(500);

	for (pwmVal = 0; pwmVal < cSIREN_PWM_ALERT; ++pwmVal){
		pwmWrite(cPWM_PIN, pwmVal);
		if(*cncl == 0)
   			break;
		delay(cSIREN_TIME_ALERT_REVUP);
	}
	while (*cncl > 0){
	}

	SirenStop(pwmVal);
	pthread_exit(0);
}

void* SirenAttack(void *c){
	int pwmVal,count, i;
	int *cncl = (int *)c;

	i = 0;

	delay(500);

	if (*cncl == 1){
		while (*cncl > 0){
		    for (pwmVal = cSIREN_PWM_ATTACK_MIN; pwmVal < cSIREN_PWM_ATTACK_MAX; ++pwmVal){
		    	if(*cncl > 0){
		    		pwmWrite (cPWM_PIN, pwmVal);
		    		delay (cSIREN_TIME_ATTACK_REVUP);
		    	}else{
		        	break;
		    	}
		    }

		    if(*cncl == 0)
		    	break;

		    for(int i = 0; i < 20; i++){
			    delay(cSIREN_TIME_ATTACK_HOLD/20);
			    if(*cncl==0)
				break;
		    }

		    if(*cncl == 0)
		    	break;

		    for (pwmVal = cSIREN_PWM_ATTACK_MAX; pwmVal > cSIREN_PWM_ATTACK_MIN; --pwmVal){
		    	if(*cncl > 0){
		       		pwmWrite (cPWM_PIN, pwmVal);
		        	delay (cSIREN_TIME_ATTACK_REVDOWN);
		    	}else{
		        	break;
		    	}
		    }
		}
	}else if(*cncl > 0 & *cncl != 1){
		count = *cncl;
		for (i = 0; i < count; ++i){
			for (pwmVal = cSIREN_PWM_ATTACK_MIN; pwmVal < cSIREN_PWM_ATTACK_MAX; ++pwmVal){
		    		if(*cncl > 0){
			    		pwmWrite (cPWM_PIN, pwmVal);
			    		delay (cSIREN_TIME_ATTACK_REVUP);
			    	}else{
		        		break;
			    	}
			  }

			  if(*cncl == 0)
			  	break;

			  for(int i = 0; i < 20; i++){
				delay(cSIREN_TIME_ATTACK_HOLD/20);
				if(*cncl==0)
					break;
			  }

			  if(*cncl == 0)
		  		break;

			for (pwmVal = cSIREN_PWM_ATTACK_MAX; pwmVal > cSIREN_PWM_ATTACK_MIN; --pwmVal){
		  		if(*cncl > 0){
			     		pwmWrite (cPWM_PIN, pwmVal);
			  	    	delay (cSIREN_TIME_ATTACK_REVDOWN);
			  	}else{
		  	    		break;
		    		}
		 	}
  		}
	}
	SirenStop(pwmVal);
	pthread_exit(0);
}

void SoundSiren(int mode, int *go){
	pthread_t tid;
	int err;
	switch(mode){
		case 1:
			delay(500);
			err = pthread_create(&tid, NULL, &SirenGrowl, (void *)go);
		        if (err != 0)
	        		printf("\ncan't create thread :[%s]", strerror(err));
	        	break;
		case 2:
			delay(500);
	       		err = pthread_create(&tid, NULL, &SirenAlert, (void *)go);
	        	if (err != 0)
		            printf("\ncan't create thread :[%s]", strerror(err));
        		break;
		case 3:
			delay(500);
		        err = pthread_create(&tid, NULL, &SirenAttack, (void *)go);
		        if (err != 0)
		            printf("\ncan't create thread :[%s]", strerror(err));
		        break;
	}
}
