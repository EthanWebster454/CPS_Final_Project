monitor: monitor.c d6t.c dht11.c buzzer.c lcd.c stepperMotor.c monitor.h
	gcc monitor.c d6t.c dht11.c buzzer.c lcd.c stepperMotor.c -o monitor -lm -lwiringPi -lwiringPiDev
