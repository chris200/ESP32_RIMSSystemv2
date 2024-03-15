const char *ssid = cszSSID;         // Enter SSID here
const char *password = cszPassword; // Enter Password here
long timezone = -6;
byte daysavetime = 1;


const int buzzer= 15;



//Sensors 
double actualTemp = 0;

//get temp interval loop
unsigned long previousMillis = 0;
unsigned long interval = 1000;

//timer input
String hours_select;
String minutes_select;
int targetTemp_int;
bool timerRunning;
String timerRunning_str;
String timeLeft_str;
String startTime_str;
String stopTime_str;
String alarm_str;
int timeLeft;
int startTime;
int stopTime;
int alarm_int;
//Target temp
String targetTemp_select;

//EEPROM
#define EEPROM_SIZE 200
int eeprom_address_hours = 0;
int eeprom_address_minutes = 1;
int eeprom_address_targetTemp=2;
int eeprom_address_timerRunning=3;
int eeprom_address_Alarm=4;
int eeprom_address_pid_kp=5;
int eeprom_address_pid_ki=14;
int eeprom_address_pid_kd=23;

int eeprom_address_startTime=50;
int eeprom_address_stopTime=100;
int eeprom_address_timeLeft=150;
//time variables
char datebuffer[72];
char timebuffer[9];
struct tm tmstruct;
int currentSecond;
int previousSecond;

//PID
double targetTemp ;
double pid_kp, pid_ki, pid_kd;
String pid_kp_str, pid_ki_str, pid_kd_str;
double Kp = 3, Ki = 5, Kd = 1;
int heater;
double Output;
#define RELAY_PIN 25 //2
PID myPID(&actualTemp, &Output, &targetTemp, Kp, Ki, Kd,P_ON_M, DIRECT);
int WindowSize = 5000;
unsigned long windowStartTime;
