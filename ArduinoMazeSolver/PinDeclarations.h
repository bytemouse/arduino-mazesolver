#pragma once

const byte ledPins[] = { 4, 5, 6, 8 }; //Pins were the debugging LED are connected
unsigned char sensorPins[] = { 0, 1, 2, 3, 4, 5 }; //analog pins were the QTR-8A sensor is connected
SoftwareSerial bluetoothSerial(7, 10); //Serial connecton pins of the bluetooth module