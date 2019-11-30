/*
* File name   : voltmeter.c
* Description : a simple voltmeter
* Website     : www.adeept.com
* E-mail      : support@adeept.com
* Author      : Jason
* Date        : 2015/06/04
*/
#include <stdio.h>
#include <stdlib.h>
//#include <wiringPi.h>
#include <lcd.h>
#include "monitor.h"

int fd;

void initLCD()
{
	
	fd = lcdInit(2,16,4, 24,23, 17,18,27,22,0,0,0,0);

	if (fd == -1){
		printf ("lcdInit failed\n") ;
		exit(1);
	}

	lcdPosition(fd, 0, 0); 
	
	lcdPuts (fd, "Room temp:");

	lcdPosition(fd, 0, 1);
	
	lcdPuts (fd, "Remote temp:");
	
}

void printTemp(float roomTemp, float remoteTemp){

	char buf[4];

	lcdPosition(fd, 10, 0);

	sprintf(buf, "%0.1f", roomTemp);

	lcdPuts(fd, buf);

	lcdPosition(fd, 12, 1);

	sprintf(buf, "%0.1f", remoteTemp);

	lcdPuts(fd, buf);

}

