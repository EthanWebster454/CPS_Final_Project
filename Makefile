monitor: buzzer.c lcd.c monitor.c stepperMotor.c monitor.h
	gcc buzzer.c lcd.c monitor.c stepperMotor.c -o monitor -lwiringPi -lwiringPiDev
