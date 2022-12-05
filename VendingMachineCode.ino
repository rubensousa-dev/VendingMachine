/*
  Author : Ruben Sousa
  Project Name : Vending Machine
  Date : 15/01/2019
*/

#include <SPI.h>
#include <MFRC522.h>
#include <Stepper.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "RedMP3.h"

#define SS_PIN 53
#define RST_PIN 5

#define MP3_RX 65 //RX of Serial MP3 module connect to D7 of Arduino
#define MP3_TX 7  // TX of the serial MP3 module connect to D8 of Arduino
MP3 mp3(MP3_RX, MP3_TX);

#define STEPS 200
Stepper stepper(STEPS, 46, 47, 48, 49);
Stepper stepper1(STEPS, 44, 43, 42, 41);
Stepper stepper2(STEPS, 30, 31, 32, 33);
Stepper stepper3(STEPS, 35, 36, 37, 38);

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
LiquidCrystal lcd(27, 26, 24, 25, 22, 23);

int buttonPressed;

#define button1 2
#define button2 3
#define button3 4
#define button4 6

int8_t index = 0x01; //the first song in the TF card
int8_t index1 = 0x02;
int8_t index2 = 0x03;
int8_t index3 = 0x04;
int8_t index4 = 0x05;
int8_t index5 = 0x06;
int8_t index6 = 0x07;
int8_t index7 = 0x08;
int8_t index8 = 0x09;
int8_t index9 = 0x10;
int8_t index10 = 0x11;
int8_t index11 = 0x12;
int8_t index12 = 0x13;
int8_t index13 = 0x14;
int8_t volume = 0x1a; //0~0x1e (30 adjustable level)

float price = 0;
float finalCredit = 0;
float credit = 0;

bool check = false;

String name = "";
String content = "";

int stepIN1Pin = 46;
int stepIN2Pin = 47;
int stepIN3Pin = 48;
int stepIN4Pin = 49;

//Stepper Connect 2
int stepIN1Pin1 = 44;
int stepIN2Pin1 = 43;
int stepIN3Pin1 = 42;
int stepIN4Pin1 = 41;

//Stepper Connect 3
int stepIN1Pin2 = 30;
int stepIN2Pin2 = 31;
int stepIN3Pin2 = 32;
int stepIN4Pin2 = 33;

//Stepper Connect 4
int stepIN1Pin3 = 35;
int stepIN2Pin3 = 36;
int stepIN3Pin3 = 37;
int stepIN4Pin3 = 38;

//define steps
int stepsPerRevolution = 2048; // amount of steps per revolution

Stepper myStepper0(stepsPerRevolution, stepIN1Pin, stepIN3Pin, stepIN2Pin, stepIN4Pin);
//2
Stepper myStepper1(stepsPerRevolution, stepIN1Pin1, stepIN3Pin1, stepIN2Pin1, stepIN4Pin1);
//3
Stepper myStepper2(stepsPerRevolution, stepIN1Pin2, stepIN3Pin2, stepIN2Pin2, stepIN4Pin2);
//4
Stepper myStepper3(stepsPerRevolution, stepIN1Pin3, stepIN3Pin3, stepIN2Pin3, stepIN4Pin3);

void setup()
{
    lcd.begin(16, 2);
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();

    // Assign Stepper Speed
    myStepper0.setSpeed(15);
    myStepper1.setSpeed(15);
    myStepper2.setSpeed(15);
    myStepper3.setSpeed(15);

    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT_PULLUP);
    pinMode(button4, INPUT_PULLUP);
}

void loop()
{

    InicialMessage();
    Buttons();
    RfidCard();
}

void InicialMessage()
{
    lcd.clear();
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("Welcome");
    lcd.setCursor(0, 2);
    lcd.print("Choose One");
}

void Buttons()
{

    if (digitalRead(button1) == LOW)
    {
        if (check == true)
        {
            buy(1);
        }
        else
        {
            Serial.println("ON1");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Cookies");
            lcd.setCursor(0, 2);
            lcd.print("1.20 eur");
            price = 1.20;
            mp3.playWithVolume(index, volume);
            delay(3000);
        }
        check = false;
    }
    else if (digitalRead(button2) == LOW)
    {
        if (check == true)
        {
            buy(2);
        }
        else
        {
            Serial.println("ON2");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Salt cookies");
            lcd.setCursor(0, 2);
            lcd.print("1 eur");
            price = 1;
            mp3.playWithVolume(index1, volume);
            delay(3000);
        }
        check = false;
    }
    else if (digitalRead(button3) == LOW)
    {
        if (check == true)
        {
            buy(3);
        }
        else
        {
            Serial.println("ON3");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Oreos");
            lcd.setCursor(0, 2);
            lcd.print("0.80 eur");
            price = 0.80;
            mp3.playWithVolume(index2, volume);
            delay(3000);
        }
        check = false;
    }
    if (digitalRead(button4) == LOW)
    {
        if (check == true)
        {
            buy(4);
        }
        else
        {
            Serial.println("ON4");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("KitKat");
            lcd.setCursor(0, 2);
            lcd.print("0.50 eur");
            price = 0.50;
            mp3.playWithVolume(index3, volume);
            delay(3000);
        }
        check = false;
    }
}

void RfidCard()
{

    // Look for new cards
    if (!rfid.PICC_IsNewCardPresent())
    {
        return;
    }
    // Select one of the cards
    else if (!rfid.PICC_ReadCardSerial())
    {
        return;
    }
    //Mostra UID na serial
    Serial.print("UID da tag :");
    String content = "";
    byte letter;
    for (byte i = 0; i < rfid.uid.size; i++)
    {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
        content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(rfid.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Mensagem : ");
    content.toUpperCase();

    if (content.substring(1) == "DE AD BE EF") //UID 1  CODIGO DA CHAVE
    {
        Serial.println("Rita");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hi Rita");
        name = "Rita";
        lcd.setCursor(0, 1);
        lcd.print("5.30 eur");
        credit = 5.30;
        mp3.playWithVolume(index4, volume);
        check = true;
        delay(5000);
    }
    else
    {
        check = false;
    }

    if (content.substring(1) == "03 4F 1A 3C") //UID 2 - Cartao CODIGO DA 2 CHAVE
    {
        Serial.println("Fred");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hi Fred");
        name = "Fred";
        lcd.setCursor(0, 1);
        lcd.print("8.89 eur");
        credit = 8.89;
        mp3.playWithVolume(index5, volume);
        check = true;
        delay(5000);
    }
    else
    {
        check = false;
    }
}

void buy(int btnPressed)
{

    switch (btnPressed)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Choose One");
        delay(5000);
    case 1: //button1
        if (name == "Rita")
        {
            myStepper0.step(stepsPerRevolution);
            finalCredit = credit - price;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(finalCredit);
            mp3.playWithVolume(index6, volume);
            delay(5000);
        }
        if (name == "Fred")
        {
            myStepper0.step(stepsPerRevolution);
            finalCredit = credit - price;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(finalCredit);
            mp3.playWithVolume(index7, volume);
            delay(5000);
        }
        btnPressed = 0;
        break;

    case 2: //button2
        if (name == "Rita")
        {
            myStepper1.step(stepsPerRevolution);
            finalCredit = credit - price;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(finalCredit);
            mp3.playWithVolume(index8, volume);
            delay(5000);
        }
        if (name == "Fred")
        {
            myStepper1.step(stepsPerRevolution);
            finalCredit = credit - price;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(finalCredit);
            mp3.playWithVolume(index9, volume);
            delay(5000);
        }
        btnPressed = 0;
        break;

    case 3: //button3
        if (name == "Rita")
        {
            myStepper2.step(stepsPerRevolution);
            finalCredit = credit - price;
            lcd.setCursor(0, 0);
            lcd.print(finalCredit);
            mp3.playWithVolume(index10, volume);
            delay(10000);
        }
        if (name == "Fred")
        {
            myStepper2.step(stepsPerRevolution);
            finalCredit = credit - price;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(finalCredit);
            mp3.playWithVolume(index11, volume);
            delay(5000);
        }
        btnPressed = 0;
        break;

    case 4: //button4
        if (name == "Rita")
        {
            myStepper3.step(stepsPerRevolution);
            finalCredit = credit - price;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(finalCredit);
            mp3.playWithVolume(index12, volume);
            delay(5000);
        }
        if (name == "Fred")
        {
            myStepper3.step(stepsPerRevolution);
            finalCredit = credit - price;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(finalCredit);
            mp3.playWithVolume(index13, volume);
            delay(5000);
        }
        btnPressed = 0;
        break;
    }
}