//sourced from https://github.com/adeept/Adeept_Ultimate_Starter_Kit_C_Code_for_RPi
/*
 * File name   : stepperMotor.c
 * Description : control a stepper motor.
 * Website     : www.adeept.com
 * E-mail      : support@adeept.com
 * Author      : Jason
 * Date        : 2015/06/21
 */
#include <wiringPi.h>

#define IN1 19    // 
#define IN2 5   // 
#define IN3 6    // 
#define IN4 13    // 

void initMotor(){

	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);

}

void setStep(int a, int b, int c, int d)  
{  
	digitalWrite(IN1, a);     
	digitalWrite(IN2, b);     
	digitalWrite(IN3, c);     
	digitalWrite(IN4, d);     
}  

void stop(void)  
{  
	setStep(0, 0, 0, 0);      
}  

void forward(int t, int steps)  
{  
	int i;  

	for(i = 0; i < steps; i++){  
		setStep(1, 0, 0, 0);  
		delay(t);  
		setStep(0, 1, 0, 0);      
		delay(t);  
		setStep(0, 0, 1, 0);      
		delay(t);  
		setStep(0, 0, 0, 1);      
		delay(t);  
	}  
}  

void backward(int t, int steps)  
{  
	int i;  

	for(i = 0; i < steps; i++){  
		setStep(0, 0, 0, 1);  
		delay(t);  
		setStep(0, 0, 1, 0);      
		delay(t);  
		setStep(0, 1, 0, 0);      
		delay(t);  
		setStep(1, 0, 0, 0);      
		delay(t);  
	}  
}  
