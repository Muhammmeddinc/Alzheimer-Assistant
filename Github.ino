// ----These library is for audio----
#include <TMRpcm.h>
#include <pcmConfig.h>
#include <pcmRF.h>

//----This library is for SD Card----
#include <SD.h>

//----This library is for SPI protocol----
#include <SPI.h>

//----This Library is for LCD----
#include <LiquidCrystal_I2C.h>

//----This library Real Time Clock----
#include <DS3231.h>

// This library is for I2C protocol----
#include <Wire.h>

//prosedural coding
int timeToWaitBetweenAlarmInformations = 2000;  //miliseconds
// int voiceLevel = 5;                             //Audio Output                    // !!!!!not Recommended, Changing variables (between 22 to 25)
int startScreenWaiting = 3000;                  //milisecond
int timeToWaitEmergencyScreen = 8000;           //miliseconds

// --- morning variables ---
int morningDrugHour = 8;
int morningDrugMinute = 0;
String morningMedicine = "Parol";
int howManyDosesMorning = 2;

// --- noon variables ---
int noonDrugHour = 13;
int noonDrugMinute = 30;
String noonMedicine = "Galatimin";
int howManyDosesNoon = 1;

// --- evening variables ---
int eveningDrugHour = 20;
int eveningDrugMinute = 0;
String eveningMedicine = "Sacrine";
int howManyDosesEvening = 3;

// --- General Reminding ---
int YourSonsOrDaugtersHour = 17;
int YourSonsOrDaugtersMinute = 0;

// ---son and daughter name---
String son = "Ahmet";
String daughter = "Azra";

// ---Tel Number and Adress For Emergency---
String tel = "05312202343";
String adress = "34th St.New York";









// Declaring  objects to use libraries.
TMRpcm Audio;
DS3231 myRTC;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// We adjust the display of the clock on the LCD.
bool h12 = false;
bool PM_Time = false;

const int chipSelect = 4;         //  SD card chip is selected.
const int pressButton = 3;        // Interrupt PIN INT1 for ADRESS and TELEPHONE
volatile bool adressTel = false;  



//This class is used for creating flexibility
class Time {
public:

  byte year;
  byte month;
  byte day;
  byte date;
  byte hour;
  byte minute;
  byte second;
};







String dayOfWeek[] = {  // This string array is used to display days on LCD
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday",
  "Sunday"
};


byte customChar[8] = {  // It is a starting shape displayed on LCD
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b10001,
  0b10001,
  0b11111,
  0b00000
};


byte customChar2[8] = {  // It is a starting shape displayed on LCD
  0b01110,
  0b01110,
  0b10101,
  0b11011,
  0b01010,
  0b10001,
  0b11011,
  0b11011
};


byte customChar3[8] = { // It is a starting shape displayed on LCD
  0b11011,
  0b10001,
  0b11011,
  0b11111,
  0b11111,
  0b11011,
  0b10001,
  0b11011
};


byte customChar4[8] = { // It is a starting shape displayed on LCD
  0b10001,
  0b01010,
  0b00100,
  0b10110,
  0b01101,
  0b00100,
  0b01010,
  0b10001
};

Time RealTime;

int shiftingRight = 10;  // It is an iteretor used in morning alarm function loop.




void setup() {
  lcd.begin(16, 2);  //Beginning LCD

  Serial.begin(9600);  //Beginning Serial Monitor

  Wire.begin();  //beginning I2C protocol

  pinMode(pressButton, INPUT);                     // It is used to choose pin type "output/input"
  attachInterrupt(INT1, emergencyButton, RISING);  // The interrupt code is a special code for the arduino and the user can choose 2 pins on the Arduino uno to use the interrupt, these are the 2nd and 3rd pins. Also known as INT0 and INT1.
                                                   // Here, the 3rd pin, INT1 is used. In order to interrupt, a digital change must be detected on this selected pin. It is 0 to 1 or 1 to 0.
                                                   // Changes have certain names. (RISING/FALLING/CHANGE) Arduino responds according to this change. After this change is detected, Arduino generally quits its usual job and goes to the code block with the function seen in the Interrupt code and does the work there.
  if (!SD.begin(chipSelect)) {                     // Controlling SD card situation
    Serial.println("Cannot find SD card");         //Here, if the arduino can't find the sd card, the code goes into an endless loop and actually our program doesn't seem to be able to start from the outside.
    while (1)
      ;
  }
  Audio.speakerPin = 9;    //selected output pin for Voice PWM signals.
  lcd.setBacklight(0x08);  // This code adjust the  backlight.

  // ----Starting Screen----

  lcd.print("Welcome to ALAS");   //When we run the program, the LCD screen starts to work and a splash screen appears where the name of the project, WELCOME TO ALAS (Alzheimer Assistant) is written.
  lcd.createChar(0, customChar);  //At the bottom, as a sweet touch to the program, the figures above are printed on the screen, then waits for 3 seconds and passes to the void code block.
  lcd.createChar(3, customChar4);
  lcd.createChar(1, customChar2);
  lcd.createChar(2, customChar3);
  lcd.setCursor(6, 1);
  lcd.print((char)1);
  lcd.setCursor(8, 1);
  lcd.print((char)0);
  lcd.setCursor(10, 1);
  lcd.print((char)2);
  lcd.setCursor(4, 1);
  lcd.print((char)3);
  delay(startScreenWaiting);
  lcd.clear();


  // ============================================== CHANGE THE COMMENT LINES BELOW TO SET STARTING DATE - HOUR ===========================================
  // The RTC DS3231 clock module is set once. In the form of hours, days, months, years, there is no need to run this code again, because the RTC clock module has its own bios battery.
  //This battery makes it possible for the watch to count itself even when the device is not working.



  //  myRTC.setSecond(13);
  //  myRTC.setYear(23);  
  //  myRTC.setMonth(7);  
  //  myRTC.setDate(12);
  //  myRTC.setDoW(3);
  //  myRTC.setHour(16);
  //  myRTC.setMinute(2);
}

void loop() {

  CheckForAlarm();  // The function check the alarms every 1 loop.

  UpdateRealTime();  // The function update time every 1 loop.

  UpdateLCD();  // The function update LCD every 1 loop.

  adressAndTel();  // Actually, Interrupt start to work 1 time. adressAndTel function is executed every loop. If button is pressed one more time, the function stop working.
}

void CheckForAlarm() {  // Controlling alarms (every 1 loop, Arduino check all alarms, and if all situations fit, Arduino run these functions.
  CheckForMorningAlarm();
  CheckForNoonAlarm();
  CheckForEveningAlarm();
  GeneralReminding();
}

void CheckForMorningAlarm() {
  if (RealTime.hour == morningDrugHour && RealTime.minute == morningDrugMinute) {  
    lcd.clear();
    Audio.setVolume(5);
    Audio.play("parol.wav");
    lcd.setCursor(0, 0);  // The code is used to set location for writing.
    lcd.print("Medication Hour");
    delay(timeToWaitBetweenAlarmInformations);
    lcd.setCursor(0, 1);
    lcd.print(morningMedicine);
    for (int i = 0; i < shiftingRight; i++) {
      lcd.scrollDisplayRight();
      delay(400);
    }
    lcd.clear();
    lcd.print("     ");
    lcd.print(howManyDosesMorning);
    lcd.print("Doses");
    delay(timeToWaitBetweenAlarmInformations);
    lcd.clear();  // Here, CheckForMorningAlarm() block's have different arrangements, firstly, voice file is ran. Secondly, LCD arrangements. Third, for interesting view, shifting arrangements.
  }
}

void CheckForNoonAlarm() {
  if (RealTime.hour == noonDrugHour && RealTime.minute == noonDrugMinute) {
    lcd.clear();
    Audio.setVolume(5);
    Audio.play("galat.wav");
    lcd.setCursor(0, 0);
    lcd.print("Medication Hour");
    delay(timeToWaitBetweenAlarmInformations);
    lcd.setCursor(0, 1);
    lcd.print(noonMedicine);
    for (int i = 0; i < shiftingRight; i++) {
      lcd.scrollDisplayRight();
      delay(400);
    }
    lcd.clear();
    lcd.print("     ");
    lcd.print(howManyDosesNoon);
    lcd.print("Doses");
    delay(timeToWaitBetweenAlarmInformations);
    lcd.clear();
  }
}

void CheckForEveningAlarm() {
  if (RealTime.hour == eveningDrugHour && RealTime.minute == eveningDrugMinute) {
    lcd.clear();
    Audio.setVolume(5);
    Audio.play("sacrine.wav");
    lcd.setCursor(0, 0);
    lcd.print("Medication Hour");
    delay(timeToWaitBetweenAlarmInformations);
    lcd.setCursor(0, 1);
    lcd.print(eveningMedicine);
    for (int i = 0; i < shiftingRight; i++) {
      lcd.scrollDisplayRight();
      delay(400);
    }
    lcd.clear();
    lcd.print("     ");
    lcd.print(howManyDosesEvening);
    lcd.print("Doses");
    delay(timeToWaitBetweenAlarmInformations);
    lcd.clear();
  }
}

void GeneralReminding() {
  if (RealTime.hour == YourSonsOrDaugtersHour && RealTime.minute == YourSonsOrDaugtersMinute) {  // Here, that alarm general reminding, Actually, above, alarms are same this alarm. Only, Aim is different. It is remind family member.
    lcd.clear();
    Audio.setVolume(5);
    Audio.play("grem.wav");
    lcd.setCursor(0, 0);
    lcd.print("Your Daughter:");
    lcd.setCursor(0, 1);
    lcd.print(daughter);
    delay(timeToWaitBetweenAlarmInformations);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Your Son:");
    lcd.setCursor(0, 1);
    lcd.print(son);
    delay(timeToWaitBetweenAlarmInformations);
    lcd.clear();
    lcd.print(son);
    lcd.print(" ");
    lcd.print("and");
    lcd.print(" ");
    lcd.print(daughter);
    for (int i = 0; i < shiftingRight; i++) {
      lcd.scrollDisplayRight();
      delay(400);
    }
    delay(1000);
    lcd.clear();
  }
}

void UpdateRealTime() {
  RealTime.year = myRTC.getYear();
  RealTime.month = myRTC.getMonth(h12);
  RealTime.date = myRTC.getDate();
  RealTime.day = myRTC.getDoW();
  RealTime.hour = myRTC.getHour(h12, PM_Time);
  RealTime.minute = myRTC.getMinute();
  RealTime.second = myRTC.getSecond();
}

void UpdateLCD() {  // here, this code block update date and time..



  lcd.setCursor(0, 1);
  lcd.print(dayOfWeek[RealTime.day - 1]);

  lcd.setCursor(0, 0);
  lcd.print(RealTime.date);
  lcd.print("/");
  lcd.print(RealTime.month);
  lcd.print("/");
  lcd.print(RealTime.year);
  lcd.print(" ");
  lcd.print(RealTime.hour);
  lcd.print(":");
  lcd.print(RealTime.minute);
  lcd.print(":");
  lcd.print(RealTime.second);
  delay(1000);
  lcd.clear();
}

void emergencyButton() {  // Intterrupt function, while pressing the button, changing the boolean variable and adressAndTel() is started to work by arduino. stopping adress and tel. pressing the button one more time.
  if (adressTel) {
    adressTel = false;
  } else {
    adressTel = true;
  }
}

void adressAndTel() {  // Emergency button is like a key in terms of adressAndTel() function. Here, this code blocks' aim for emergency situation.
  if (adressTel) {

    lcd.clear();
    Audio.setVolume(5);
    Audio.play("tele.wav");
    lcd.setCursor(0, 0);
    lcd.print("Tel.Number");
    lcd.setCursor(0, 1);
    lcd.print(tel);
    delay(timeToWaitEmergencyScreen);
    lcd.clear();
    Audio.play("adrs.wav");
    lcd.setCursor(0, 0);
    lcd.print("Address");
    lcd.setCursor(0, 1);
    lcd.print(adress);
    delay(timeToWaitEmergencyScreen);
    lcd.clear();
  }
}
