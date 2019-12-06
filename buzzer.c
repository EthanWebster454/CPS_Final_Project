//sourced from https://github.com/adeept/Adeept_Ultimate_Starter_Kit_C_Code_for_RPi
/*
* File name   : passiveBuzzer.c
* Description : Make an passive buzzer sound with different frequency.
* Website     : www.adeept.com
* E-mail      : support@adeept.com
* Author      : Jason
* Date        : 2015/06/22
*/
#include <softTone.h>
#include <stdio.h>
#include <stdlib.h>
#include "monitor.h"

#define  BZRPin  4

void makeSound()
{
	
	if(softToneCreate(BZRPin) < 0){
		printf("SoftTone setup failed!\n");
		exit(1);
	}

	softToneWrite(BZRPin, 800);	
	delay(200);
	softToneWrite(BZRPin, 800);	
	delay(200);
	softToneWrite(BZRPin, 800);	
	delay(200);
	softToneWrite(BZRPin, 0);
	
}

