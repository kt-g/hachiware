#include "DFRobot_DF2301Q.h"
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*tx =*/10, /*rx =*/11);
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif

#define Led 8

DFRobotDFPlayerMini myDFPlayer;
DFRobot_DF2301Q_I2C asr;

void printDetail(uint8_t type, int value);
void setupDFPlayerMini(); 
void setupVoiceRecognition();


void setup()
{
  Serial.begin(115200);
  setupVoiceRecognition();
  setupDFPlayerMini();
}

void loop()
{
  uint8_t CMDID = asr.getCMDID();
    switch (CMDID) {
    case 103:                                                  //If the command is “Turn on the light”
      digitalWrite(Led, HIGH);                                 //Turn on the LED
      myDFPlayer.play(1);
      Serial.println("received'Turn on the light',command flag'103'");  //Serial transmits "received"Turn on the light",command flag"103
      printDetail(myDFPlayer.readType(), myDFPlayer.read());
      break;

    case 104:                                                  //If the command is “Turn off the light”
      digitalWrite(Led, LOW);                                  //Turn off the LED
      myDFPlayer.play(2);
      Serial.println("received'Turn off the light',command flag'104'");  //The serial transmits "received"Turn off the light",command flag"104""
      printDetail(myDFPlayer.readType(), myDFPlayer.read());
      break;

    default:
      if (CMDID != 0) {
        Serial.print("CMDID = ");  //Printing command ID
        Serial.println(CMDID);
      }
  }
  delay(3000);
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  
}

void setupVoiceRecognition() {

  pinMode(Led, OUTPUT);
  digitalWrite(Led, LOW);

  while (!(asr.begin())) {
    Serial.println("Communication with device failed, please check connection");
    delay(3000);
  }

  /**
   * @brief Set voice volume
   * @param voc - Volume value(1~7)
   */
  asr.setVolume(7);

  /**
     @brief Set mute mode
     @param mode - Mute mode; set value 1: mute, 0: unmute
  */
  asr.setMuteMode(0);

  /**
     @brief Set wake-up duration
     @param wakeTime - Wake-up duration (0-255)
  */
  asr.setWakeTime(255);

  /**
     @brief Get wake-up duration
     @return The currently-set wake-up period
  */
  uint8_t wakeTime = 0;
  wakeTime = asr.getWakeTime();
  Serial.print("wakeTime = ");
  Serial.println(wakeTime);

  asr.playByCMDID(1);   // Wake-up command

  Serial.println(F("Voice Recognition online."));

}

void setupDFPlayerMini() {
  #if (defined ESP32)
    FPSerial.begin(9600, SERIAL_8N1, /*rx =*/D3, /*tx =*/D2);
  #else
    FPSerial.begin(9600);
  #endif
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(15);  //Set volume value. From 0 to 30
}

