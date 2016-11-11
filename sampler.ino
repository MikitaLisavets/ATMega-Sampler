#include "SD.h"
#include "TMRpcm.h"
#include "SPI.h"

#define SD_ChipSelectPin A0
#define ALL_BTNS_COUNT 16
#define MIN_REPEAT 50
#define MAX_REPEAT 200
#define DEFAULT_REPEAT 150

const int ROWS = 4;
const int COLS = 4;

int btn1 = A1;
int btn2 = A2;
int pot1 = A3;
int pot2 = A4;
int led = A5;

int rowPins[ROWS] = {6, 7, 8, 10};
int colPins[COLS] = {2, 3, 4, 5};
char *fileNames[ALL_BTNS_COUNT] = {
  "/1.wav",  "/2.wav",  "/3.wav",  "/4.wav",
  "/5.wav",  "/6.wav",  "/7.wav",  "/8.wav",
  "/9.wav",  "/10.wav", "/11.wav", "/12.wav",
  "/13.wav", "/14.wav", "/15.wav", "/16.wav"  
  };
  
char *paths[ALL_BTNS_COUNT] = {
  "1",  "2",  "3",  "4",
  "5",  "6",  "7",  "8",
  "9",  "10", "11", "12",
  "13", "14", "15", "16"  
  };

char *path;
boolean shotPads[ALL_BTNS_COUNT] = {false};
int repeatPads[ALL_BTNS_COUNT] = {
  DEFAULT_REPEAT, DEFAULT_REPEAT, DEFAULT_REPEAT, DEFAULT_REPEAT,
  DEFAULT_REPEAT, DEFAULT_REPEAT, DEFAULT_REPEAT, DEFAULT_REPEAT,
  DEFAULT_REPEAT, DEFAULT_REPEAT, DEFAULT_REPEAT, DEFAULT_REPEAT,
  DEFAULT_REPEAT, DEFAULT_REPEAT, DEFAULT_REPEAT, DEFAULT_REPEAT
 };
int currentButton;

TMRpcm audio;

void setup() {
  Serial.begin(9600);
  audio.speakerPin = 9;

  for(int i = 0; i < COLS; i++) {
    pinMode(colPins[i], INPUT_PULLUP);
  }

  for(int i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], HIGH);
  }
   
  pinMode(pot1, INPUT);
  pinMode(pot2, INPUT);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(led, OUTPUT);


  audio.quality(1);  
  audio.setVolume(3);

  SD.begin(SD_ChipSelectPin);

}

void loop() {
  checkSounds();
  controls();
}

void controls() {
  while(1) {
    readBtns();
    audio.setVolume(map(analogRead(pot1), 0, 1024, 2, 6));
    if (digitalRead(btn1) == 0 && digitalRead(btn2) == 0) {
      return;
    }
  }
}

void checkSounds() {
  unsigned long currentTime;
  int val, index;
  while(1) {
 
    digitalWrite(led, HIGH);

    currentTime = millis();
    while (millis() - currentTime < 100) {
      index = 0;
      for(int i = 0; i < ROWS; i++) {
        digitalWrite(rowPins[i], LOW);
        for(int j = 0; j < COLS; j++) {
          val = digitalRead(colPins[j]);
          if (val == 0) {
            path = paths[index];
            return;
          }
          index++;
        }
        digitalWrite(rowPins[i], HIGH);
      }
    }

    digitalWrite(led, LOW);

    currentTime = millis();
    while (millis() - currentTime < 100) {
      index = 0;
      for(int i = 0; i < ROWS; i++) {
        digitalWrite(rowPins[i], LOW);
        for(int j = 0; j < COLS; j++) {
          val = digitalRead(colPins[j]);
          if (val == 0) {
            path = paths[index];
            return;
          }
          index++;
        }
        digitalWrite(rowPins[i], HIGH);
      }
    } 
  }
}


void readBtns() {
  unsigned long currentTime;
  int val, index = 0;
  char buff[100];
  for(int i = 0; i < ROWS; i++) {
    digitalWrite(rowPins[i], LOW);
    for(int j = 0; j < COLS; j++) {
      val = digitalRead(colPins[j]);
      if (val == 0) {
        if (digitalRead(btn1) == 0) {
          shotPads[index] = true;
        }
        if (digitalRead(btn2) == 0) {
          shotPads[index] = false;
          repeatPads[index] = map(abs(1024 - analogRead(pot2)), 0, 1024, MIN_REPEAT, MAX_REPEAT);
        }
        if (shotPads[index] == true && currentButton == index && audio.isPlaying()) {
          continue;
        } else {
          digitalWrite(led, HIGH);
          strcpy(buff, path);
          strcat(buff, fileNames[index]);
          audio.SAMPLE_RATE = 1000;
          audio.play(buff);
          delay(repeatPads[index]);
          currentButton = index;
          digitalWrite(led, LOW);
        }
      }
      index++;
    }
    digitalWrite(rowPins[i], HIGH);
  }
}
