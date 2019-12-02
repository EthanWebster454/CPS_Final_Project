#define ERROR -273.15

//DHT11 functions
double read_dht11_dat();

// D6T functions
void initRemote();
float getRemoteTemp();

// LCD functions
void initLCD();
void printRoomTemp(double);
void printRemoteTemp(float);

// buzzer functions
void makeSound();

// motor functions
void stop(void);
void forward(int, int);
void backward(int, int);
void initMotor();