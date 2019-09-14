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
   for (pwmVal = cur; pwmVal > SIREN_PWM_CUTOFF; --pwmVal){
      pwmWrite(PWM_PIN, pwmVal);
      delay(1);
    }
    pwmWrite (PWM_PIN, 0);
}

void* SirenGrowl(void *c){
  int pwmVal;
  int *cncl = (int *)c;
  for (pwmVal = 0; pwmVal < SIREN_PWM_GROWL; ++pwmVal){
      pwmWrite(PWM_PIN, pwmVal);
      delay(SIREN_TIME_GROWL_REVUP);
  }
  while (*cncl > 0)
  {
  }
  SirenStop(pwmVal);
  pthread_exit(0);
}

void* SirenAlert(void *c){
  int pwmVal;
  int *cncl = (int *)c;
  for (pwmVal = 0; pwmVal < SIREN_PWM_ALERT; ++pwmVal){
      pwmWrite(PWM_PIN, pwmVal);
      delay(SIREN_TIME_ALERT_REVUP);
  }
   while (*cncl > 0)
  {
  }
  SirenStop(pwmVal);
  pthread_exit(0);
}

void* SirenAttack(void *c){
	int pwmVal,count, i;
	int *cncl = (int *)c;
	i = 0;
	if (*cncl == 1){
		while (*cncl > 0){
		    for (pwmVal = SIREN_PWM_ATTACK_MIN; pwmVal < SIREN_PWM_ATTACK_MAX; ++pwmVal){
		    	if(*cncl > 0){
		    		pwmWrite (PWM_PIN, pwmVal);
		    		delay (SIREN_TIME_ATTACK_REVUP);
		    	}else{
		        	break;
		    	}
		    }

		    if(*cncl == 0)
		    	break;

		    delay(SIREN_TIME_ATTACK_HOLD);

		    for (pwmVal = SIREN_PWM_ATTACK_MAX; pwmVal > SIREN_PWM_ATTACK_MIN; --pwmVal){
		    	if(*cncl > 0){
		       		pwmWrite (PWM_PIN, pwmVal);
		        	delay (SIREN_TIME_ATTACK_REVDOWN);
		    	}else{
		        	break;
		    	}
		    }
		}
	}else if(*cncl > 0 & *cncl != 1){
		count = *cncl;
		  for (i = 0; i < count; ++i){
			for (pwmVal = SIREN_PWM_ATTACK_MIN; pwmVal < SIREN_PWM_ATTACK_MAX; ++pwmVal){
		    	if(*cncl > 0){
		    		pwmWrite (PWM_PIN, pwmVal);
		    		delay (SIREN_TIME_ATTACK_REVUP);
		    	}else{
		        	break;
		    	}
		    }

		    if(*cncl == 0)
		    	break;

		    delay(SIREN_TIME_ATTACK_HOLD);

		    for (pwmVal = SIREN_PWM_ATTACK_MAX; pwmVal > SIREN_PWM_ATTACK_MIN; --pwmVal){
		    	if(*cncl > 0){
		       		pwmWrite (PWM_PIN, pwmVal);
		        	delay (SIREN_TIME_ATTACK_REVDOWN);
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
      	err = pthread_create(&tid, NULL, &SirenGrowl, (void *)go);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        break;
      case 2:
        err = pthread_create(&tid, NULL, &SirenAlert, (void *)go);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        break;
      case 3:
        err = pthread_create(&tid, NULL, &SirenAttack, (void *)go);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        break;
    }
}
