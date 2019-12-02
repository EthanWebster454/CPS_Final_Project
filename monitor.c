#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <wiringPi.h>
#include "monitor.h"

#define HALF_SWEEP 64
#define ANGLE_INCR 8
#define NUM_STEPS 16
#define MOTOR_DEL 3
#define ROOM_HIGH_THRESH 30.0
#define ROOM_LOW_THRESH 15.0
#define REMOTE_HIGH_THRESH 37.5
#define HUMAN_MAX_TEMP 40.0
#define DELAY_BETW_READ 300
#define ROOM_TEMP_DANGER "Child's room temperature in danger zone"
#define CHILD_TEMP_DANGER "Child may have fever"
#define PYTHON_CALL "python3 camera.py \"%s\""


void sendEmailAlert(bool tempType){

    char buffer[100];

    if(tempType)
        sprintf(buffer,PYTHON_CALL,ROOM_TEMP_DANGER);
    else
        sprintf(buffer,PYTHON_CALL,CHILD_TEMP_DANGER);

    system(buffer);
}


int main(){

    if(wiringPiSetupGpio() == -1){
		printf("wiringPi setup failed !\n");
		return 1;
	}

    double roomTemp;
    float remoteTemp;
    size_t i;
    bool direction = 1;
    void (*rotate)(int,int);

    initLCD();

    initMotor();

    initRemote();

    makeSound();

    // start the motor at half sweep forward
	forward(MOTOR_DEL, HALF_SWEEP);
    stop();

    // main temperature reading loop
    while(1){

        rotate = direction ? backward : forward;
        
        // get room temperature
        roomTemp = read_dht11_dat(); // from DHT11
        if(roomTemp > 0)
            printRoomTemp(roomTemp);

        // if roomTemp > highThresh or < lowThresh, perform alarm
        if(roomTemp != ERROR && (roomTemp > ROOM_HIGH_THRESH || roomTemp < ROOM_LOW_THRESH)){
            makeSound();
            sendEmailAlert(true);
        }

        for(i=0;i<NUM_STEPS;i++){

            remoteTemp = getRemoteTemp();

            printRemoteTemp(remoteTemp);

            // temperature is in the danger zone, perform alarm
            if(remoteTemp > REMOTE_HIGH_THRESH && remoteTemp < HUMAN_MAX_TEMP) {
                makeSound();
                sendEmailAlert(false);
            }

            // rotate the motor by the correct increment
            rotate(MOTOR_DEL, ANGLE_INCR);
            stop();

            delay(DELAY_BETW_READ);
        }

        //flip direction bit
        direction = !direction;
    }


    return 0;
}