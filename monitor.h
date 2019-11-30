

// LCD functions
void initLCD();
void printTemp(float roomTemp, float remoteTemp);

// buzzer functions
void makeSound();

// motor functions
void setStep(int a, int b, int c, int d);
void stop(void);
void forward(int t, int steps);
void backward(int t, int steps);
void initMotor();