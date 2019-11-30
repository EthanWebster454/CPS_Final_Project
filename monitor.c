#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "monitor.h"


int main(){

    if(wiringPiSetupGpio() == -1){
		printf("wiringPi setup failed !\n");
		exit(1);
	}

    initLCD();

    printTemp(88.1, 64.4);

    makeSound();

    delay(700);

    initMotor();

	forward(3, 100);

	delay(200);

	backward(3, 200);

	stop();

}