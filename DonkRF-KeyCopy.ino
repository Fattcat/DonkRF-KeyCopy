#include <SD.h>
#include <SPI.h>
#include <RH_ASK.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

 //             AUTOR               \\
//  Created by Dominik Hulin 2023    \\

// GitHub : github.com/Fattcat
// YOuTube : https://www.youtube.com/channel/UCKfToKJFq-uvI8svPX0WzYQ

//  //  //   CONNECTION   \\  \\  \\

//Arduino Nano   |   SD kartový modul   |   OLED displej   |   Tlačidlo
// ----------------------------------------------------------------------- \\
// D2 (Interrupt)|                      |                  |   
// D3 (CS)       |   CS (Chip Select)   |                  |
// D4 (MOSI)     |   DI (Data In)       |                  |
// D5 (SCK)      |   SCK (Clock)        |                  |
// D6            |                      |   SDA (Serial Data)  |
// D7            |                      |   SCL (Serial Clock) |
// D10 (CS)      |                      |                  |   Button Pin
// 5V            |   VCC (5V)           |   VCC (Power)    |
// GND           |   GND                |   GND (Ground)   |   GND
// ----------------------------------------------------------------------- \\

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RH_ASK rf_driver;
char receivedMessage[10];

File myFile;
int fileCount = 0;
bool buttonPressed = false; // Pridali sme premennú pre sledovanie stlačenia tlačidla

void setup() {
  Serial.begin(9600);

  if (SD.begin(10)) {
    Serial.println("SD karta inicializovaná.");
  } else {
    Serial.println("Chyba pri inicializácii SD karty.");
  }

  if (rf_driver.init()) {
    Serial.println("RF komunikácia inicializovaná.");
  } else {
    Serial.println("Chyba pri inicializácii RF komunikácie.");
  }

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}

void loop() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  if (rf_driver.recv(buf, &buflen)) {
    buf[buflen] = '\0';
    strcpy(receivedMessage, (char*)buf);

    // Kontrola stlačenia tlačidla
    if (digitalRead(7) == LOW) {
      buttonPressed = true;
    } else {
      buttonPressed = false;
    }

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Zachytene: ");
    display.println(receivedMessage);
    display.println("Ukladam do SD karty...");
    display.display();

    if (!buttonPressed) {
      myFile = SD.open("frequency" + String(fileCount) + ".txt", FILE_WRITE);

      if (myFile) {
        myFile.println(receivedMessage);
        myFile.close();
        Serial.println("Frekvencia zapísaná do súboru " + String(fileCount) + ".txt");
        fileCount++;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Freq ulozena!");
        display.display();
        delay(3000);
        display.clearDisplay();
        display.display();
      } else {
        Serial.println("Chyba pri otváraní súboru.");
      }
    }
  }
  delay(1000);
}
