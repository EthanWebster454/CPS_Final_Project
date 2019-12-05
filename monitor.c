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
#define PYTHON_CALL "python3 camera.py %s"
#define CAMERA_DELAY 1000


void sendEmailAlert(bool tempType){

    char command[100];

    if(tempType)
        sprintf(command,PYTHON_CALL,"room");
    else
        sprintf(command,PYTHON_CALL,"remote");

    system(command);
}


// experimentally determined linear calibration model for object temperature
float calibrateValue(float measVal){

    return 1.152*measVal - 3.462;
}


int main(){

    if(wiringPiSetupGpio() == -1){
		printf("wiringPi setup failed !\n");
		return 1;
	}

    double roomTemp;
    float remoteTemp;
    size_t i;
    bool roomTempError=0, remTempError=0, direction = 1;
    void (*rotate)(int,int); // function pointer for rotation

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
        // only perform the alarm once. The system is reset once the temperature is back inside the safe zone
        if(!roomTempError && roomTemp != ERROR && (roomTemp > ROOM_HIGH_THRESH || roomTemp < ROOM_LOW_THRESH)){
            roomTempError=true;
            makeSound();
            sendEmailAlert(true);
        }
        // this resets the tracker variable if the situation is resolved
        else if(roomTemp != ERROR && roomTemp < ROOM_HIGH_THRESH && roomTemp > ROOM_LOW_THRESH)
            roomTempError=false;

        for(i=0;i<NUM_STEPS;i++){

            remoteTemp = getRemoteTemp();
            if(remoteTemp>0){
                remoteTemp = calibrateValue(remoteTemp); // calibrate temperature read using D6T
                printRemoteTemp(remoteTemp);
            }

            // temperature is in the danger zone, perform alarm
            if(!remTempError && remoteTemp != ERROR && (remoteTemp > REMOTE_HIGH_THRESH && remoteTemp < HUMAN_MAX_TEMP)) {
                remTempError = true;
                makeSound();
                sendEmailAlert(false);
                delay(CAMERA_DELAY);
            }
            // this resets the tracker variable if another person is present
            else if(remoteTemp != ERROR && remoteTemp < REMOTE_HIGH_THRESH)
                remTempError=false;

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